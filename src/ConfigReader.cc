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

std::vector<TreeInfo> ConfigReader::ReadTreesInfo() const {
    // Logic to read tree configurations from fConfigValues

    // declare the tree vector 
    std::vector<TreeInfo> trees;

    // check all the geometry and detector states 
    // for now just Solenoid 

    G4cout << "GetTreesInfo() is called" << G4endl;
    G4int solenoidStatus = GetConfigValueAsInt("Solenoid","solenoidStatus");
    if (solenoidStatus){
        G4int ICdet = GetConfigValueAsInt("Solenoid","inFrontCore");
        G4cout << "The detector in front of the core of the Solenoid has status "<< ICdet << G4endl;
        if (ICdet>0){
            trees.push_back(TreeInfo("inFrontCore", "inFrontCore", 0));
        }
        G4int bCDet = GetConfigValueAsInt("Solenoid","behindCore");
        G4cout << "The detector behind the core of the Solenoid has status "<< bCDet << G4endl;
        if (bCDet){
            trees.push_back(TreeInfo("behindCore","behindCore",ICdet));
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
    }else{ // use summary mode
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