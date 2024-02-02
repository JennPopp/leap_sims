#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <string>
#include <map>
#include "G4ThreeVector.hh"

class ConfigReader {
public:
    ConfigReader(const std::string& configFile);
    bool ReadConfig();
    std::string GetConfigValue(const std::string& section, const std::string& key) const;
    double GetConfigValueAsDouble(const std::string& section, const std::string& key) const;
    G4ThreeVector GetConfigValueAsG4ThreeVector(const std::string& section, const std::string& key) const;
    int GetConfigValueAsInt(const std::string& section, const std::string& key) const;

private:
    std::string fConfigFile;
    std::map<std::string, std::map<std::string, std::string>> fConfigValues;
};

#endif // CONFIGREADER_HH
