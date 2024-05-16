#include "CaloCrystalSD.hh"
#include "ConfigReader.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"


CaloCrystalSD::CaloCrystalSD(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager)
    : G4VSensitiveDetector(name),
      fLayerIdentifier(layerIdentifier),
      fTupleID(tupleID),
      fAnaConfigManager(anaConfigManager),
      fShowerDevStat(anaConfigManager.GetShowerDevStat()),
      fEdepTot(9),
      fEdepTot_ct(9)

{
    //constructor body
}

CaloCrystalSD::~CaloCrystalSD() {}

G4bool CaloCrystalSD::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    // Common data processing logic, using layerIdentifier for differentiation   
    auto touchable = step->GetPreStepPoint()->GetTouchable();
    // G4cout << " Edep in this step " << step->GetTotalEnergyDeposit()<< G4endl;
    // G4cout << "Lenght of the step" << step->GetStepLength()<< G4endl;
    // G4cout << "name of the prestepvolume"<< step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName()<< G4endl;
    if (fShowerDevStat) {
        fAnaConfigManager.FillCaloCrystNtuple_detailed(fTupleID, touchable, step);
    } else { // outputmode == summary as default 
        int crystNo = touchable->GetReplicaNumber(3);
        // Here the energy cherenkov threshold will be considered 
        if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0){ 
            G4double Etot = step->GetTrack()->GetTotalEnergy();
            if(Etot > 0.64243){
                //G4cout << " Etot in this step " << Etot << G4endl;
                // G4double Tlength = step->GetStepLength(); this is for trecklenght 
                G4double Edep_ct = step->GetTotalEnergyDeposit();
                fEdepTot_ct[crystNo] += Edep_ct;
            }
        }
        // always add to total energy sum and total number of particles 
        G4double Edep = step->GetTotalEnergyDeposit();
        fEdepTot[crystNo] += Edep;
        
    }

    return true;
}

void CaloCrystalSD::Reset() {
    std::fill(fEdepTot.begin(),fEdepTot.end(),0.0);
    std::fill(fEdepTot_ct.begin(),fEdepTot_ct.end(),0.0);
}
