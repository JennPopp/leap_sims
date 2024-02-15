// EventAction.cc
#include "EventAction.hh"
#include "BaseSensitiveDetector.hh" // Include your sensitive detector header
#include "AnaConfigManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include <vector>

EventAction::EventAction(AnaConfigManager& anaConfigManager)
    : G4UserEventAction(),
      fAnaConfigManager(anaConfigManager),
      fOutputMode(anaConfigManager.GetOutputMode()), // Initialize from AnaConfigManager
      fTreesInfo(anaConfigManager.GetTreesInfo()) { // Initialize from AnaConfigManager

    // constructor body
}

EventAction::~EventAction() {
}

void EventAction::BeginOfEventAction(const G4Event*) {
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();

    if(fOutputMode == "summary"){
        for (const auto& treeInfo : fTreesInfo) {
            BaseSensitiveDetector* mySD = static_cast<BaseSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
            G4cout << "Reset: " << treeInfo.name<< G4endl;
            if (mySD) {
                mySD->Reset(); // Reset accumulated data for each sensitive detector
            }
        }
    }
}

void EventAction::EndOfEventAction(const G4Event*) {
    if (fOutputMode == "summary") {
        G4SDManager* sdManager = G4SDManager::GetSDMpointer();

        for (const auto& treeInfo : fTreesInfo) {
            BaseSensitiveDetector* mySD = static_cast<BaseSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
            if (!mySD) continue;

            G4double energySum = mySD->GetEnergySum();
            G4int Ntot = mySD->GetTotalCount();
            G4double EgammaSum = mySD->GetGammaEnergySum();
            G4int Ngamma = mySD->GetGammaCount();
            G4double EeSum = mySD->GetElectronEnergySum();
            G4int Ne = mySD->GetElectronCount();

            // These vectors are automatically passed by const reference due to the function signature
            std::vector<int> particleCounts = {Ntot, Ngamma, Ne};
            std::vector<G4double> energySums = {energySum, EgammaSum, EeSum};

            fAnaConfigManager.FillBaseNtuple_summary(mySD->GetTupleID(), particleCounts, energySums);
        }
    }
}