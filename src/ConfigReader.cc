#include "ConfigReader.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <exception>
#include "G4ThreeVector.hh"

ConfigReader::ConfigReader(const std::string& configFile)
    : fConfigFile(configFile) {}

bool ConfigReader::ReadConfig() {
    std::ifstream file(fConfigFile);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file: " << fConfigFile << std::endl;
        return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        // Section line
        if (line[0] == '[') {
            section = line.substr(1, line.find(']') - 1);
            continue;
        }

        // Key-value line
        size_t delimiterPos = line.find('=');
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        fConfigValues[section][key] = value;
    }

    return true;
}

std::string ConfigReader::GetConfigValue(const std::string& section, const std::string& key) const {
    auto sectionIt = fConfigValues.find(section);
    if (sectionIt != fConfigValues.end()) {
        const auto& keyMap = sectionIt->second;
        auto keyIt = keyMap.find(key);
        if (keyIt != keyMap.end()) {
            return keyIt->second;
        }
    }
    return ""; // Return an empty string or handle the error if the key is not found
}
double ConfigReader::GetConfigValueAsDouble(const std::string& section, const std::string& key) const{
    std::string valueStr = GetConfigValue(section, key);
    try {
        return std::stod(valueStr);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
    }
    return 0.0; // Default value or throw an exception
}
G4ThreeVector ConfigReader::GetConfigValueAsG4ThreeVector(const std::string& section, const std::string& key) const {
    std::string valueStr = GetConfigValue(section, key);
    std::istringstream iss(valueStr);

    double x, y, z;
    iss >> x >> y >> z;

    if (iss.fail()) {
        std::cerr << "Failed to parse G4ThreeVector for " << section << ":" << key << std::endl;
        return G4ThreeVector(); // Return a default vector or handle the error appropriately
    }

    return G4ThreeVector(x, y, z);
}

int ConfigReader::GetConfigValueAsInt(const std::string& section, const std::string& key) const {
    std::string valueStr = GetConfigValue(section, key);
    std::istringstream iss(valueStr);
    int value;
    if (!(iss >> value)) {
        std::cerr << "Error: Failed to convert [" << section << "] " << key << " to int: " << valueStr << std::endl;
        return 0; // Return a default value or handle the error as needed
    }
    return value;
}

std::string ConfigReader::ReadOutputMode() const {
        
        try {
            std::string     mode = GetConfigValue("Output", "mode");
            if (!mode.empty()) {
                return mode;
            }
        } catch (const std::exception& e) {
            // Handle the exception if needed, e.g., logging the error message
        }
        return "summary"; // default value
    }
int ConfigReader::ReadShowerDevStat() const {
        
        try {
            int     state = GetConfigValueAsInt("Calorimeter", "showerDev");
            if (state) {
                return state;
            }
        } catch (const std::exception& e) {
            // Handle the exception if needed, e.g., logging the error message
        }
        return 0; // default value
    }
std::vector<TreeInfo> ConfigReader::ReadTreesInfo() const {
    // Logic to read tree configurations from fConfigValues

    // declare the tree vector 
    std::vector<TreeInfo> trees;

    // check all the geometry and detector states 
    // for now just Solenoid 
    G4int ICdet, bCDet, CFdet, CCdet, CBdet;

    G4cout << "GetTreesInfo() is called" << G4endl;
    G4int solenoidStatus = GetConfigValueAsInt("Solenoid","solenoidStatus");
    if (solenoidStatus){
        ICdet = GetConfigValueAsInt("Solenoid","inFrontCore");
        G4cout << "The detector in front of the core of the Solenoid has status "<< ICdet << G4endl;
        if (ICdet>0){
            trees.push_back(TreeInfo("inFrontCore", "inFrontCore", 0));
        }
        bCDet = GetConfigValueAsInt("Solenoid","behindCore");
        G4cout << "The detector behind the core of the Solenoid has status "<< bCDet << G4endl;
        if (bCDet){
            trees.push_back(TreeInfo("behindCore","behindCore",ICdet));
        }
    }
    G4int caloStatus = GetConfigValueAsInt("Calorimeter","calorimeterStatus");
    if(caloStatus){
        CFdet = GetConfigValueAsInt("Calorimeter","frontDetector");
        G4cout << "The detector in front of the Calorimeter has status "<< CFdet << G4endl;
        if(CFdet){
            G4int treeID = solenoidStatus*(ICdet+bCDet);
            trees.push_back(TreeInfo("inFrontCalo", "inFrontCalo", treeID));
        }
        CCdet = GetConfigValueAsInt("Calorimeter","crystDetector");
        G4cout << "The Calorimeter-Crystal detector has status "<< CCdet << G4endl;
        if(CCdet){
            G4int treeID = solenoidStatus*(ICdet+bCDet)+CFdet;
            trees.push_back(TreeInfo("CaloCrystal", "CaloCrystal", treeID));
        }
        CBdet = GetConfigValueAsInt("Calorimeter","backDetector");
        G4cout << "The detector at the back of the Calorimeter has status "<< CBdet << G4endl;
        if(CBdet){
            G4int treeID = solenoidStatus*(ICdet+bCDet)+CFdet+CCdet;
            trees.push_back(TreeInfo("behindCalo", "behindCalo", treeID));
        }
    }
    G4cout << "trees size: " << trees.size() << G4endl;
    return trees;
}

std::vector<BranchInfo> ConfigReader::GetBranchesInfo(const std::string& treeName) const {
    std::vector<BranchInfo> branches;
    
    std::string mode = ReadOutputMode();

    //later I will use the name to check whether It gets
    // the standart currently below or calo info  
    if (treeName == "CaloCrystal" ){
            if(GetConfigValueAsInt("Calorimeter","showerDev")==1){
                branches = {
                    {"pdg","I"},
                    {"E","D"},
                    {"Edep","D"},
                    {"x","D"},
                    {"y","D"},
                    {"z","D"},
                    {"TrackID","I"},
                    {"ParentID","I"},
                    {"EventID","I"},
                    {"crystNo","I"}
                };
            }else{
                branches = { 
                {"Edep_0","D"},
                {"Edep_1","D"},
                {"Edep_2","D"},
                {"Edep_3","D"},
                {"Edep_4","D"},
                {"Edep_5","D"},
                {"Edep_6","D"},
                {"Edep_7","D"},
                {"Edep_8","D"}
                };
            }
            return branches ;
        }


    if (mode == "detailed"){
        branches = {
            {"pdg", "I"},
            {"E", "D"},
            {"x", "D"},
            {"y", "D"},
            {"z", "D"},
            {"startx", "D"},
            {"starty", "D"},
            {"startz", "D"},
            {"px", "D"},
            {"py", "D"},
            {"pz", "D"},
            {"Polx", "D"},
            {"Poly", "D"},
            {"Polz", "D"},
            {"TrackID", "D"},
            {"ParentID", "D"},
            {"EventID", "D"}
        };
        if (treeName == "inFrontCalo" || treeName =="behindCalo" ){
            branches.push_back({"crystNo","I"});
        }
    }else{ // use summary mode
        if (treeName == "inFrontCalo" || treeName == "behindCalo"){
            branches = {
                {"Esum_0","D"},
                {"Esum_1","D"},
                {"Esum_2","D"},
                {"Esum_3","D"},
                {"Esum_4","D"},
                {"Esum_5","D"},
                {"Esum_6","D"},
                {"Esum_7","D"},
                {"Esum_8","D"},
                {"NP_0","D"},
                {"NP_1","D"},
                {"NP_2","D"},
                {"NP_3","D"},
                {"NP_4","D"},
                {"NP_5","D"},
                {"NP_6","D"},
                {"NP_7","D"},
                {"NP_8","D"}

            };
            return branches;
        }
        branches = {
            {"Esum", "D"},
            {"NP", "I"},
            {"EGammaSum", "D"},
            {"NGamma", "I"},
            {"EeSum", "D"},
            {"Ne", "I"}
        };
    }

    return branches;
}

const std::map<std::string, std::map<std::string, std::string>>& ConfigReader::GetConfigValues() const {
    return fConfigValues;
}

std::string ConfigReader::ReadOutputFileName() const {
    try {
            std::string OutputFileName = GetConfigValue("Output", "fileName");
            if (!OutputFileName.empty()) {
                return OutputFileName;
            }
        } catch (const std::exception& e) {
            G4cout<< "No output file name was found in the config file, use default."<< G4endl;
        }
    return "result"; // default value
    
}