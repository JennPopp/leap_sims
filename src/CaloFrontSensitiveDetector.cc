#include "CaloFrontSensitiveDetector.hh"
#include "ConfigReader.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"


CaloFrontSensitiveDetector::CaloFrontSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager, const G4double frontZPos)
    : G4VSensitiveDetector(name),
      fLayerIdentifier(layerIdentifier),
      fTupleID(tupleID),
      fAnaConfigManager(anaConfigManager),
      fOutputMode(anaConfigManager.GetOutputMode()),
      fEinLim(anaConfigManager.GetEinLim()),
      fEnergySum(9),
      fNtot(9),
      fEin_tot(0.),
      ffrontZPos(frontZPos)

{
    G4cout << "Constructing CaloFrontSensitiveDetector. Address: " << this << ", Size: " << fEnergySum.size() << G4endl;
}

CaloFrontSensitiveDetector::~CaloFrontSensitiveDetector() {}

G4bool CaloFrontSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    // G4cout << "ZPOS OF THE FORNT DETECTOR" << ffrontZPos << G4endl;
    // G4double preStepZ = step->GetPreStepPoint()->GetPosition().z();
    // G4double postStepZ = step->GetPostStepPoint()->GetPosition().z();
    // G4cout << "ZPOS OF THE PRESTEPPOINT " << preStepZ << G4endl;
    // G4cout << "ZPOS OF THE POSTSTEPPOINT " << postStepZ << G4endl;

    // This is needet to transform the z pos of the step to the local posion 
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4ThreeVector globalPosition = preStepPoint->GetPosition();
    G4double localZ = preStepPoint->GetTouchableHandle()->GetHistory()->GetTransform(2).TransformPoint(globalPosition).z();
    // G4cout << "LOCAL ZPOS OF THE PRESTEPPOINT " << localZ << G4endl;

    // Add some 
    //G4cout << "ProcessHits called for volume: " << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    // Common data processing logic, using layerIdentifier for differentiation
    //G4cout << "ProcessHits called: Momentum direction" << step->GetPostStepPoint()->GetMomentumDirection().z() << G4endl;
    if (preStepPoint->GetMomentumDirection().z() > 0 && (localZ == ffrontZPos-0.5 || localZ == -ffrontZPos-0.5 )){
        // No matter what the output format, always fill the histograms !  
        fAnaConfigManager.FillHistos(fTupleID,step);

        // definde the if condition to check the total energy is reached.
        G4double Estep = step->GetPostStepPoint()->GetTotalEnergy()/MeV;
        fEin_tot += Estep;
        // G4cout << "ElLimit: " << fEinLim<< G4endl;
        // G4cout << "Ein_total" << fEin_tot<< G4endl;
        if (fEinLim != 0.) {
            if (fEin_tot >= fEinLim){
                G4cout << "Total Energy of " << fEin_tot << " MeV has been reached -> event stopped" << G4endl;
                G4RunManager::GetRunManager()->AbortEvent();
            }
        }

        if (fOutputMode == "detailed") {
            auto touchable = preStepPoint->GetTouchable();
            fAnaConfigManager.FillCaloFrontTuple_detailed(fTupleID, touchable, step);
        } else { // outputmode == summary as default 
            auto touchable = preStepPoint->GetTouchable();
            int motherdepth = 1; 
            if (preStepPoint->GetPhysicalVolume()->GetName() == "VacStep4") {
                motherdepth = 1; // changed the mother volume of the Vacstep4 to calovirtuel volume so not motherdepth =2 ( for Alu as mother) but same as Fontdetector 1 
            }
            int crystNo = touchable->GetReplicaNumber(motherdepth);
            // G4cout << "ProcessHits called for volume: " << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() << motherdepth << crystNo << G4endl;
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
    fEin_tot = 0.;
}