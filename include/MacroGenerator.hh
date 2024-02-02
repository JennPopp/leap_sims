#ifndef MACROGENERATOR_HH
#define MACROGENERATOR_HH

#include "ConfigReader.hh"
#include <string>

class MacroGenerator {
public:
    // Constructor and destructor as needed
    MacroGenerator();
    ~MacroGenerator();

    // Static function to generate macro based on config
    static void generateMacro(const ConfigReader& config, const std::string& tempMacroFilename );
};


#endif // MACROGENERATOR_HH