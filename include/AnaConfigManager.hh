#ifndef AnaConfigManager_h
#define AnaConfigManager_h 1

#include "ConfigReader.hh" 
#include "G4TouchableHistory.hh"

class G4Step;
class G4Run;

class AnaConfigManager {
    public:
    // constructor
    AnaConfigManager(const ConfigReader& config);
    // destructor
    ~AnaConfigManager();

    //member functions  
    void SetUp(const G4Run* aRun, G4String outFileName);
    void BookNtuples();
    void Save() const;

    void FillBaseNtuple_summary(int tupleID,
                                const std::vector<int> particleCounts,
                                const std::vector<G4double> energySums) const;
    void FillBaseNtuple_detailed(int tupleID, G4Step* step) const;
    void FillCaloFrontTuple_detailed(int tupleID,const G4VTouchable* history, G4Step* step) const;
    void FillCaloFrontTuple_summary(int tupleID,
                                    const std::vector<int> NP,
                                    const std::vector<double> Esum) const;
    void FillCaloCrystNtuple_detailed(int tupleID,const G4VTouchable* history, G4Step* step) const;
    void FillCaloCrystNtuple_summary(int tupleID,
                                    const std::vector<double> Edep,
                                    const std::vector<double> Tlength) const;
    void SetupMetadataTTree();

    //getter methods 
    const std::string& GetOutputMode() const {
        return fOutputMode;
    }
    const std::string& GetOutputFileName() const {
        return fOutputFileName;
    }
    const std::vector<TreeInfo>& GetTreesInfo() const {
        return fTreesInfo;
    }
    std::map<std::string, int>& GetNtupleNameToIdMap(){
    return fNtupleNameToIdMap;
    }
    const int GetShowerDevStat() const{
        return fShowerDevStat;
    }

    private:
    // using the convention of putting an f in front of member variables 
    const ConfigReader& fConfig;
    const std::string fOutputMode;
    const std::string fOutputFileName;
    const int fShowerDevStat;
    const std::vector<TreeInfo> fTreesInfo; // tree info is structure with name, title and id
    std::map<std::string, int> fNtupleNameToIdMap; // need this for defining sensitive volumes in the subdetector classes

}; 


















#endif