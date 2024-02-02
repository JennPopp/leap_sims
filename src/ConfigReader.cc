#include "ConfigReader.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
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