# include "AnaConfigManager.hh"
# include "ConfigReader.hh"
#include <vector>
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

AnaConfigManager::AnaConfigManager(const ConfigReader& config)
  : fConfig(config),
    fOutputMode(config.ReadOutputMode()),
    fTreesInfo(config.ReadTreesInfo()) {
    
    G4cout << "\n----> The output mode is " << fOutputMode << "\n" << G4endl;
   
    G4cout << "\n----> The registered detectors are :" << G4endl;
    for (const auto& treeInfo : fTreesInfo) {
        G4cout << treeInfo.name << G4endl;
        fNtupleNameToIdMap[treeInfo.name] = treeInfo.id;
    }
}

AnaConfigManager::~AnaConfigManager() {}

void AnaConfigManager::SetUp( const G4Run* aRun, 
                              G4String outFileName) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetActivation(true);
    analysisManager->SetVerboseLevel(4);

    std :: ostringstream oss;
    oss << "run"<< aRun->GetRunID()<< "_"<< outFileName ;
    G4String fileName=oss.str();
    
    // Open file histogram file
    //G4cout << "THE ANALYSIS MANAGER CALLS OPENFILE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<< G4endl;
    analysisManager->OpenFile(fileName);

    // Book the ntuples (GEANT4 lingo for Ttrees)
    BookNtuples();

};

void AnaConfigManager::BookNtuples() {
    G4cout << "Booking Ntuples ..." << G4endl;
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    for (const auto& treeInfo : fTreesInfo) {
        analysisManager->CreateNtuple(treeInfo.name, treeInfo.title);
        auto branchesInfo = fConfig.GetBranchesInfo(treeInfo.name);
        for (const auto& branchInfo : branchesInfo) {
            if (branchInfo.type == "D") {
                analysisManager->CreateNtupleDColumn(treeInfo.id,branchInfo.name);
            } else if (branchInfo.type == "I") {
                analysisManager->CreateNtupleIColumn(treeInfo.id,branchInfo.name);
            }
        }
        analysisManager->FinishNtuple();
    }
};

void AnaConfigManager::Save() const {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();

    G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;

};

void AnaConfigManager::FillBaseNtuple_detailed(int tupleID, G4Step* step) const {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(tupleID,0, step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());

    analysisManager->FillNtupleDColumn(tupleID,1, step->GetPostStepPoint()->GetTotalEnergy()/CLHEP::MeV);

    analysisManager->FillNtupleDColumn(tupleID,2, step->GetPostStepPoint()->GetPosition().x());
    analysisManager->FillNtupleDColumn(tupleID,3, step->GetPostStepPoint()->GetPosition().y());
    analysisManager->FillNtupleDColumn(tupleID,4, step->GetPostStepPoint()->GetPosition().z());

    analysisManager->FillNtupleDColumn(tupleID,5, step->GetTrack()->GetVertexPosition().x());
    analysisManager->FillNtupleDColumn(tupleID,6, step->GetTrack()->GetVertexPosition().y());
    analysisManager->FillNtupleDColumn(tupleID,7, step->GetTrack()->GetVertexPosition().z());

    analysisManager->FillNtupleDColumn(tupleID,8, step->GetPostStepPoint()->GetMomentumDirection().x());
    analysisManager->FillNtupleDColumn(tupleID,9, step->GetPostStepPoint()->GetMomentumDirection().y());
    analysisManager->FillNtupleDColumn(tupleID,10, step->GetPostStepPoint()->GetMomentumDirection().z());

    analysisManager->FillNtupleDColumn(tupleID,11, step->GetTrack()->GetPolarization().x());
    analysisManager->FillNtupleDColumn(tupleID,12, step->GetTrack()->GetPolarization().y());
    analysisManager->FillNtupleDColumn(tupleID,13, step->GetTrack()->GetPolarization().z());

    analysisManager->FillNtupleDColumn(tupleID,14, step->GetTrack()->GetTrackID());
    analysisManager->FillNtupleDColumn(tupleID,15, step->GetTrack()->GetParentID());
    analysisManager->FillNtupleDColumn(tupleID,16, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

    analysisManager->AddNtupleRow(tupleID);
};

void AnaConfigManager::FillBaseNtuple_summary(int tupleID,
                                                const std::vector<int> particleCounts,
                                                const std::vector<G4double> energySums) const {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleDColumn(tupleID, 0, energySums[0]); // energySum
    analysisManager->FillNtupleIColumn(tupleID, 1, particleCounts[0]); // Ntot
    analysisManager->FillNtupleDColumn(tupleID, 2, energySums[1]);
    analysisManager->FillNtupleIColumn(tupleID, 3, particleCounts[1]);
    analysisManager->FillNtupleDColumn(tupleID, 4, energySums[2]);
    analysisManager->FillNtupleIColumn(tupleID, 5, particleCounts[2]);
    analysisManager->AddNtupleRow(tupleID);
};

void AnaConfigManager::SetupMetadataTTree() {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    const std::map<std::string, std::map<std::string, std::string>>& nestedConfig = fConfig.GetConfigValues();

    // Create a new ntuple for metadata
    int tupleID = analysisManager->CreateNtuple("Metadata", "Configuration Metadata");
    int keyColumnId = analysisManager->CreateNtupleSColumn("Key");
    int valueColumnId = analysisManager->CreateNtupleSColumn("Value");

    // Finalize the ntuple
    analysisManager->FinishNtuple(tupleID);

    // Iterate through the nested configuration map
    for (const auto& sectionPair : nestedConfig) {
        const std::string& sectionName = sectionPair.first;
        const auto& keysValues = sectionPair.second;

        for (const auto& kv : keysValues) {
            std::string fullKey = sectionName + "." + kv.first; // Concatenate section and key
            std::string value = kv.second;

            // Fill the ntuple with the full key and value
            analysisManager->FillNtupleSColumn(tupleID, keyColumnId, fullKey);
            analysisManager->FillNtupleSColumn(tupleID, valueColumnId, value);
            analysisManager->AddNtupleRow(tupleID);
        }
    }
    
}