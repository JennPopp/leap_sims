#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <string>
#include <map>
#include <vector>
#include "G4ThreeVector.hh"

//structure that holds info about the branches 
struct BranchInfo {
    std::string name;
    std::string type; // "D" for double, "I" for integer, etc.
};

// Structure to hold information about each TTree
struct TreeInfo {
    std::string name;
    std::string title;
    int id;

    TreeInfo(const std::string& n, const std::string& t, int i) 
        : name(n), title(t), id(i) {}
};

class ConfigReader {
public:
    ConfigReader(const std::string& configFile);
    bool ReadConfig();
    std::string GetConfigValue(const std::string& section, const std::string& key) const;
    double GetConfigValueAsDouble(const std::string& section, const std::string& key) const;
    G4ThreeVector GetConfigValueAsG4ThreeVector(const std::string& section, const std::string& key) const;
    int GetConfigValueAsInt(const std::string& section, const std::string& key) const;
    std::string ReadOutputMode()  const;
    const std::map<std::string, std::map<std::string, std::string>>& GetConfigValues() const;
    std::string ReadOutputFileName() const;
    int ReadShowerDevStat() const;
    //methods for reading tree and branch configurations 
    std::vector<TreeInfo> ReadTreesInfo() const;
    std::vector<BranchInfo> GetBranchesInfo(const std::string& treeName) const;

private:
    std::string fConfigFile;
    std::map<std::string, std::map<std::string, std::string>> fConfigValues;
};

#endif // CONFIGREADER_HH
