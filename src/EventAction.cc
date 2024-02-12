// EventAction.cc
#include "EventAction.hh"
#include "BaseSensitiveDetector.hh" // Include your sensitive detector header
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"

EventAction::EventAction() : G4UserEventAction(), fConfig("config.ini") {
    fOutputMode = fConfig.GetOutputMode();
    std::vector<TreeInfo> fTreesInfo = fConfig.GetTreesInfo();
}

EventAction::~EventAction() {
}

void EventAction::BeginOfEventAction(const G4Event*) {
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();

    if(fOutputMode == "summary"){
        for (const auto& treeInfo : fTreesInfo) {
            BaseSensitiveDetector* mySD = static_cast<BaseSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
            if (mySD) {
                mySD->Reset(); // Reset accumulated data for each sensitive detector
            }
        }
    }
}

void EventAction::EndOfEventAction(const G4Event*)
{
    if (fOutputMode == "summary"){
        //------------------------------------------------------------
        // Retrieve data from your sensitive detector here
        //------------------------------------------------------------

        G4SDManager* sdManager = G4SDManager::GetSDMpointer();
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        // loop over all the sensitive detector layers in use 
        for (const auto& treeInfo : fTreesInfo) {
            
            // get the sensitive detector 
            BaseSensitiveDetector* mySD = static_cast<BaseSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
            if (!mySD) continue;

            // Now use the getter methods to access the data
            G4double energySum = mySD->GetEnergySum();
            G4double Ntot = mySD->GetTotalCount();
            G4double EgammaSum = mySD->GetGammaEnergySum();
            G4double Ngamma = mySD->GetGammaCount();
            G4double EeSum = mySD->GetElectronEnergySum();
            G4double Ne = mySD->GetElectronCount();

            //------------------------------------------------------------
            // Fill it into the trees ! 
            //------------------------------------------------------------

            G4int tupleID = mySD->GetTupleID();

            analysisManager->FillNtupleDColumn(tupleID, 0, energySum);
            analysisManager->FillNtupleIColumn(tupleID, 1, Ntot);
            analysisManager->FillNtupleDColumn(tupleID, 2, EgammaSum);
            analysisManager->FillNtupleIColumn(tupleID, 3, Ngamma);
            analysisManager->FillNtupleDColumn(tupleID, 4, EeSum);
            analysisManager->FillNtupleIColumn(tupleID, 5, Ne);
            analysisManager->FinishNtuple();

        }// end of for loop over all SDs 
    } // end if summary mode 
    

    // And fill it into the trees 
}