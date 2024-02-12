#include "BaseSensitiveDetector.hh"
#include "ConfigReader.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

BaseSensitiveDetector::BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID)
: G4VSensitiveDetector(name), fLayerIdentifier(layerIdentifier), fConfig("config.ini") { 
    fOutputMode = fConfig.GetOutputMode();
    fAnalysisManager = G4AnalysisManager::Instance();
}

BaseSensitiveDetector::~BaseSensitiveDetector() {}

G4bool BaseSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    // Common data processing logic, using layerIdentifier for differentiation
     
    if (fOutputMode == "detailed") {
        fAnalysisManager->FillNtupleIColumn(fTupleID,0, step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());

        fAnalysisManager->FillNtupleDColumn(fTupleID,1, step->GetPostStepPoint()->GetTotalEnergy()/CLHEP::MeV);

        fAnalysisManager->FillNtupleDColumn(fTupleID,2, step->GetPostStepPoint()->GetPosition().x());
        fAnalysisManager->FillNtupleDColumn(fTupleID,3, step->GetPostStepPoint()->GetPosition().y());
        fAnalysisManager->FillNtupleDColumn(fTupleID,4, step->GetPostStepPoint()->GetPosition().z());

        fAnalysisManager->FillNtupleDColumn(fTupleID,5, step->GetTrack()->GetVertexPosition().x());
        fAnalysisManager->FillNtupleDColumn(fTupleID,6, step->GetTrack()->GetVertexPosition().y());
        fAnalysisManager->FillNtupleDColumn(fTupleID,7, step->GetTrack()->GetVertexPosition().z());

        fAnalysisManager->FillNtupleDColumn(fTupleID,8, step->GetPostStepPoint()->GetMomentumDirection().x());
        fAnalysisManager->FillNtupleDColumn(fTupleID,9, step->GetPostStepPoint()->GetMomentumDirection().y());
        fAnalysisManager->FillNtupleDColumn(fTupleID,10, step->GetPostStepPoint()->GetMomentumDirection().z());

        fAnalysisManager->FillNtupleDColumn(fTupleID,11, step->GetTrack()->GetPolarization().x());
        fAnalysisManager->FillNtupleDColumn(fTupleID,12, step->GetTrack()->GetPolarization().y());
        fAnalysisManager->FillNtupleDColumn(fTupleID,13, step->GetTrack()->GetPolarization().z());

        fAnalysisManager->FillNtupleDColumn(fTupleID,14, step->GetTrack()->GetTrackID());
        fAnalysisManager->FillNtupleDColumn(fTupleID,15, step->GetTrack()->GetParentID());
        fAnalysisManager->FillNtupleDColumn(fTupleID,16, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

        fAnalysisManager->AddNtupleRow(fTupleID);
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