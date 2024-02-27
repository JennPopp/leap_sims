// MyRunAction.cc
#include "RunAction.hh"
#include "AnaConfigManager.hh"
#include "G4AnalysisManager.hh"

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


