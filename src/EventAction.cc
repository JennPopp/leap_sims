// EventAction.cc
#include "EventAction.hh"
#include "BaseSensitiveDetector.hh" // Include your sensitive detector header
#include "CaloFrontSensitiveDetector.hh"
#include "CaloCrystalSD.hh"
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

    // if(fOutputMode == "summary"){
    //     for (const auto& treeInfo : fTreesInfo) {
    //         if (treeInfo.name == "inFrontCalo"){
    //             CaloFrontSensitiveDetector* mySD = static_cast<CaloFrontSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
    //         }else if (treeInfo.name == "CaloCrystal"){
    //             CaloCrystalSD* mySD = static_cast<CaloCrystalSD*>(sdManager->FindSensitiveDetector(treeInfo.name));
    //         }else{
    //             BaseSensitiveDetector* mySD = static_cast<BaseSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
    //         }
            
    //         G4cout << "Reset: " << treeInfo.name<< G4endl;
    //         if (mySD) {
    //             mySD->Reset(); // Reset accumulated data for each sensitive detector
    //         }
    //     }
    // }
    if(fOutputMode == "summary"){
    for (const auto& treeInfo : fTreesInfo) {
        G4VSensitiveDetector* mySD = sdManager->FindSensitiveDetector(treeInfo.name);

        if (mySD) {
            G4cout << "Reset: " << treeInfo.name << G4endl;
            if (treeInfo.name == "inFrontCalo"){
                CaloFrontSensitiveDetector* specSD = dynamic_cast<CaloFrontSensitiveDetector*>(mySD);
                specSD->Reset();
            }else if (treeInfo.name == "CaloCrystal"){
                CaloCrystalSD* specSD = dynamic_cast<CaloCrystalSD*>(mySD);
                specSD->Reset();
            }else{
                BaseSensitiveDetector* specSD = dynamic_cast<BaseSensitiveDetector*>(mySD);
                specSD->Reset();
            }
        }
    }
}
  
}

void EventAction::EndOfEventAction(const G4Event*) {
    if (fOutputMode == "summary") {
        G4SDManager* sdManager = G4SDManager::GetSDMpointer();
        
        for (const auto& treeInfo : fTreesInfo) {
            if (treeInfo.name == "inFrontCalo"){
                CaloFrontSensitiveDetector* mySD = static_cast<CaloFrontSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
                if (!mySD) continue;
                std::vector<double> energySum = mySD->GetEnergySum();
                std::vector<int> Ntot = mySD->GetTotalCount();
                //G4cout << "CaloFrontSensitiveDetector Address in EventAction: " << mySD << ", Size: " << energySum.size() << G4endl;
                //G4cout << "-------ooooooooooooo-----------ooooooooooooooooooo---------ooooooooooooooo----- energySum has length " << energySum.size() << G4endl;
                fAnaConfigManager.FillCaloFrontTuple_summary(mySD->GetTupleID(), Ntot, energySum);

            } else if (treeInfo.name == "CaloCrystal" ){
                continue;
            }else{
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
    for (const auto& treeInfo : fTreesInfo) {
        if(treeInfo.name == "CaloCrystal") {
            G4SDManager* sdManager = G4SDManager::GetSDMpointer();
            CaloCrystalSD* mySD = static_cast<CaloCrystalSD*>(sdManager->FindSensitiveDetector(treeInfo.name));
            if (!mySD) continue;
            std::vector<double> Edep = mySD->GetEdepTot();
            fAnaConfigManager.FillCaloCrystNtuple_summary(mySD->GetTupleID(), Edep);

        }
    }
}