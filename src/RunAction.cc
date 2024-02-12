// MyRunAction.cc
#include "RunAction.hh"
#include "ConfigReader.hh"
#include "G4AnalysisManager.hh"

RunAction::RunAction() {
    // Constructor: Initialize analysis manager or other members as needed
}

RunAction::~RunAction() {
    // Destructor
}

void RunAction::BeginOfRunAction(const G4Run* run) {
    // Called at the start of each run

    // Read the config file 
    ConfigReader configReader("config.ini");
    configReader.ReadConfig();

    // Initialize the analysis manager and create ntuples here
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetActivation(true);
    analysisManager->SetVerboseLevel(1);

    // create a file with name based on run number
    std :: ostringstream oss;
    oss << "Result_Run_" << run->GetRunID();
    G4String fileName=oss.str();
    analysisManager->OpenFile(fileName);

    // Define ntuples
    SetUpTtrees();

    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
    G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
    G4cout << "### fName : " << fileName << G4endl; 
    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;

    
}

void RunAction::EndOfRunAction(const G4Run* run) {
    // Called at the end of each run

    // don't execute if there have been no events 
    G4int NbOfEvents = run->GetNumberOfEvent();
    if (NbOfEvents == 0) return;

    // show Rndm status
    //CLHEP::HepRandom::showEngineStatus();

    // Save and close analysis files here
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();


    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
    G4cout << "### Run " << run->GetRunID() << " Ended." << G4endl;
    G4cout << "....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......" << G4endl;
}

void RunAction::SetUpTtrees(){
    ConfigReader configReader("config.ini");
    configReader.ReadConfig();

    //retrieve Tree information
    std::vector<TreeInfo> treesInfo = configReader.GetTreesInfo();

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    for (const auto& treeInfo : treesInfo) {
        analysisManager->CreateNtuple(treeInfo.name, treeInfo.title);
        ntupleNameToIdMap[treeInfo.name] = treeInfo.id;
        auto branchesInfo = configReader.GetBranchesInfo(treeInfo.name);
        for (const auto& branchInfo : branchesInfo) {
            if (branchInfo.type == "D") {
                analysisManager->CreateNtupleDColumn(branchInfo.name);
            } else if (branchInfo.type == "I") {
                analysisManager->CreateNtupleIColumn(branchInfo.name);
            }
            // Handle other types as necessary
        }
        analysisManager->FinishNtuple();
    }
}