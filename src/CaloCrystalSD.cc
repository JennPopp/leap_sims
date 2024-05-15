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
      fTlengthTot(9)

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
        // summe up tranck length for charged particles 
        if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0){ 
            G4double Tlength = step->GetStepLength();
            fTlengthTot[crystNo] += Tlength;
        }
        // always add to total energy sum and total number of particles 
        G4double Edep = step->GetTotalEnergyDeposit();
        fEdepTot[crystNo] += Edep;
        
    }

    return true;
}

void CaloCrystalSD::Reset() {
    std::fill(fEdepTot.begin(),fEdepTot.end(),0.0);
    std::fill(fTlengthTot.begin(),fTlengthTot.end(),0.0);
}
