// MyRunAction.cc
#include "RunAction.hh"
#include "AnaConfigManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SDManager.hh"
#include <vector>
#include "BaseSensitiveDetector.hh" // Include your sensitive detector header
#include "CaloFrontSensitiveDetector.hh"
#include "CaloCrystalSD.hh"
#include <iostream>

// ANSI escape code for red text
const std::string red = "\033[31m";

// ANSI escape code to reset text color
const std::string reset = "\033[0m";


// Constructor implementation
RunAction::RunAction(AnaConfigManager& anaConfigManager)
    : G4UserRunAction(),
      fAnaConfigManager(anaConfigManager),
      fOutputMode(anaConfigManager.GetOutputMode()), // Initialize from AnaConfigManager
      fTreesInfo(anaConfigManager.GetTreesInfo()) { // Initialize from AnaConfigManager
    // constructor body
}

RunAction::~RunAction() {
    
}

void RunAction::BeginOfRunAction(const G4Run* run) {
    // Called at the start of each run
    //std::cout << red << "Call to BeginOfRunAction." << reset << std::endl;
    G4String outputFileName = fAnaConfigManager.GetOutputFileName();
    // Initialize the analysis manager and create ntuples here
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Clear(); // Clear existing analysis objects
    fAnaConfigManager.SetUp(run, outputFileName); 

    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
    G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;

    
}

void RunAction::EndOfRunAction(const G4Run* run) {
    // Called at the end of each run

    // don't execute if there have been no events 
    G4int NbOfEvents = run->GetNumberOfEvent();
    if (NbOfEvents == 0) return;

    //Run Summary 
     if (fOutputMode == "SumRun") {
        G4SDManager* sdManager = G4SDManager::GetSDMpointer();
        
        for (const auto& treeInfo : fTreesInfo) {
            if (treeInfo.name == "inFrontCalo" || treeInfo.name == "behindCalo"){
                CaloFrontSensitiveDetector* mySD = static_cast<CaloFrontSensitiveDetector*>(sdManager->FindSensitiveDetector(treeInfo.name));
                if (!mySD) continue;
                std::vector<double> energySum = mySD->GetEnergySum();
                std::vector<int> Ntot = mySD->GetTotalCount();
                //G4cout << "CaloFrontSensitiveDetector Address in EventAction: " << mySD << ", Size: " << energySum.size() << G4endl;
                //G4cout << "-------ooooooooooooo-----------ooooooooooooooooooo---------ooooooooooooooo----- energySum has length " << energySum.size() << G4endl;
                fAnaConfigManager.FillCaloFrontTuple_summary(mySD->GetTupleID(), Ntot, energySum);

            } else if (treeInfo.name == "CaloCrystal" ){
                G4SDManager* sdManager = G4SDManager::GetSDMpointer();
                CaloCrystalSD* mySD = static_cast<CaloCrystalSD*>(sdManager->FindSensitiveDetector(treeInfo.name));
                if (!mySD) continue;
                std::vector<double> Edep = mySD->GetEdepTot();
                std::vector<double> Edep_ct= mySD->GetEdepTot_ct();
                //G4cout << "EdepSUM: " << Edep[0] << G4endl;
                fAnaConfigManager.FillCaloCrystNtuple_summary(mySD->GetTupleID(), Edep, Edep_ct);
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


    // show Rndm status
    //CLHEP::HepRandom::showEngineStatus();
    
    //save the config data in a ttree. Has to be last ntuple to be created! 
    fAnaConfigManager.SetupMetadataTTree();

    // Save and close analysis files here
    fAnaConfigManager.Save();


    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
    G4cout << "### Run " << run->GetRunID() << " Ended." << G4endl;
    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
}


