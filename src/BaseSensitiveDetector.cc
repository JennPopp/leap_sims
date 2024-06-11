#include "BaseSensitiveDetector.hh"
#include "ConfigReader.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

BaseSensitiveDetector::BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager)
    : G4VSensitiveDetector(name),
      fLayerIdentifier(layerIdentifier),
      fTupleID(tupleID),
      fAnaConfigManager(anaConfigManager),
      fOutputMode(anaConfigManager.GetOutputMode())

{
    //constructor body
}

BaseSensitiveDetector::~BaseSensitiveDetector() {}

G4bool BaseSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    
    
    // Common data processing logic, using layerIdentifier for differentiation
    if (step->GetPostStepPoint()->GetMomentumDirection().z() > 0){
        // No matter what the output format, always fill the histograms !  
        fAnaConfigManager.FillHistos(fTupleID,step);

        if (fOutputMode == "detailed") {
            fAnaConfigManager.FillBaseNtuple_detailed(fTupleID, step);
        } else { // outputmode == summary as default 
            // Get the PDG-ID to check what type of particle it is 
            int pID = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
            G4double ene = step->GetPostStepPoint()->GetTotalEnergy()/MeV;
            // always add to total energy sum and total number of particles 
            fEnergySum += ene;
            fNtot += 1; 
            //then add to respective particle sums 
            if (pID == 22){
                fEgammaSum += ene;
                fNgamma += 1;
            } else if (pID == 11){
                fEeSum += ene;
                fNe += 1;
            };
        }
    } 
    

    return true;
}

void BaseSensitiveDetector::Reset() {
    fEnergySum = 0;
    fNtot = 0;
    fNgamma = 0;
    fEgammaSum = 0;
    fNe = 0;
    fEeSum = 0;
}