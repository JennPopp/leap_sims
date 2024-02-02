#include "MacroGenerator.hh"
#include "ConfigReader.hh"

#include <fstream>
#include <sstream>




void MacroGenerator::generateMacro(const ConfigReader& config, const std::string& tempMacroFilename) {
    // Open a file stream to write the macro
    std::ofstream macroFile(tempMacroFilename);
    if (!macroFile.is_open()) {
        G4cerr << "Could not open macro file " << tempMacroFilename << " for writing." << G4endl;
        return;
    }
    macroFile << "/control/verbose 2" << std::endl;
    macroFile << "/run/verbose 2" << std::endl;
   
    int rndsd1 = config.GetConfigValueAsInt("RandomSeeds","rndsds1");
    int rndsd2 = config.GetConfigValueAsInt("RandomSeeds","rndsds2");
    macroFile << "/random/setSeeds " << rndsd1 << " " << rndsd2 << std::endl; 
    
    macroFile << "/run/printProgress 1" << std::endl; 
    macroFile << "/run/initialize" << std::endl; 

    // polarization manager, only callable ofter initialization -----------------------------------
    macroFile << "/polarization/manager/verbose 1" << std::endl;
    macroFile << "/polarization/volume/list" << std::endl;

    // gps commands -------------------------------------------------------------------------------
    int nBunch = config.GetConfigValueAsInt("GPS","nBunch");
    macroFile <<"/gps/number " << nBunch << std::endl;

    std::string particle = config.GetConfigValue("GPS","particle");
    macroFile <<"/gps/particle " << particle << std::endl;

    std::string position = config.GetConfigValue("GPS","position");
    macroFile <<"/gps/pos/centre " << position << std::endl; 

    macroFile << "/gps/pos/type Beam" << std::endl;

    std::string spotSize = config.GetConfigValue("GPS","spotSize");
    macroFile << "/gps/pos/sigma_x "<< spotSize << std::endl;
    macroFile << "/gps/pos/sigma_y "<< spotSize << std::endl;

    macroFile << "/gps/ang/type beam2d"<< std::endl;

    std::string divergence = config.GetConfigValue("GPS","divergence");
    macroFile << "/gps/ang/sigma_x "<< divergence << std::endl;
    macroFile << "/gps/ang/sigma_y "<< divergence << std::endl;
    
    //shoots in positive Z direction
    macroFile << "/gps/ang/rot1 0 1 0"<< std::endl;
    macroFile << "/gps/ang/rot2 1 0 0"<< std::endl;

    std::string eneType = config.GetConfigValue("GPS", "eneType");
    macroFile << "/gps/ene/type " << eneType <<std::endl;

    std::string energy;
    if (eneType == "Gauss"){
        
        energy = config.GetConfigValue("GPS", "energy");
        macroFile << "/gps/energy " << energy << std::endl;

        std::string sigmaE = config.GetConfigValue("GPS","sigmaE");
        macroFile << "/gps/ene/sigma " << sigmaE << std::endl; 

    }else if (eneType == "User"){
        macroFile << "/gps/hist/type energy" << std::endl;

        std::string histname = config.GetConfigValue("GPS", "histname");
        macroFile << "/control/execute" << histname << std::endl;
    }

    std::string polDeg = config.GetConfigValue("GPS","polDeg"); 

    // one run in each polarization direction !----------------------------------------------------  

    macroFile << "/gps/polarization 0. 0. " << polDeg << std::endl;

    macroFile << "/run/beamOn 1" << std::endl;

    macroFile << "/gps/polarization 0. 0. -" << polDeg << std::endl;

    macroFile << "/run/beamOn 1" << std::endl;
    
    // Close the file
    macroFile.close();
}