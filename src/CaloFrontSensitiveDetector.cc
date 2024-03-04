#include "CaloFrontSensitiveDetector.hh"
#include "ConfigReader.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"


CaloFrontSensitiveDetector::CaloFrontSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager)
    : G4VSensitiveDetector(name),
      fLayerIdentifier(layerIdentifier),
      fTupleID(tupleID),
      fAnaConfigManager(anaConfigManager),
      fOutputMode(anaConfigManager.GetOutputMode()),
      fEnergySum(9),
      fNtot(9)

{
    G4cout << "Constructing CaloFrontSensitiveDetector. Address: " << this << ", Size: " << fEnergySum.size() << G4endl;
}

CaloFrontSensitiveDetector::~CaloFrontSensitiveDetector() {}

G4bool CaloFrontSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    //G4cout << "ProcessHits called for volume: " << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    // Common data processing logic, using layerIdentifier for differentiation
    //G4cout << "ProcessHits called: Momentum direction" << step->GetPostStepPoint()->GetMomentumDirection().z() << G4endl;
    if (step->GetPreStepPoint()->GetMomentumDirection().z() > 0 ){
        
        if (fOutputMode == "detailed") {
            auto touchable = step->GetPreStepPoint()->GetTouchable();
            fAnaConfigManager.FillCaloFrontTuple_detailed(fTupleID, touchable, step);
        } else { // outputmode == summary as default 
            auto touchable = step->GetPreStepPoint()->GetTouchable();
            int crystNo = touchable->GetReplicaNumber(1);
            G4double ene = step->GetPostStepPoint()->GetTotalEnergy()/MeV;
            // always add to total energy sum and total number of particles 
            //G4cout << fEnergySum[crystNo] << "+=" << ene << G4endl;
            fEnergySum[crystNo] += ene;
            
            fNtot[crystNo] += 1; 
        }
    } 
    

    return true;
}

void CaloFrontSensitiveDetector::Reset() {
    std::fill(fEnergySum.begin(),fEnergySum.end(),0.0);
    std::fill(fNtot.begin(),fNtot.end(),0); 
}