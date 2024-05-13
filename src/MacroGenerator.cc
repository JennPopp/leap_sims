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
    auto solenoidStatus = config.GetConfigValueAsInt("Solenoid","solenoidStatus");
    if (solenoidStatus){
        macroFile << "/polarization/manager/verbose 1" << std::endl;
        macroFile << "/polarization/volume/list" << std::endl;
    }
    
    // test the step sice stuff--------------------------------------------------------------------
   // macroFile <<"/testem/stepMax" << " 0.1 mm" << std::endl;

    // gps commands -------------------------------------------------------------------------------
    int nBunch = config.GetConfigValueAsInt("GPS","nBunch");
    macroFile <<"/gps/number " << nBunch << std::endl;

    std::string particle = config.GetConfigValue("GPS","particle");
    macroFile <<"/gps/particle " << particle << std::endl;

    std::string position = config.GetConfigValue("GPS","position");
    macroFile <<"/gps/pos/centre " << position << " mm" << std::endl; 

    std::string posType = config.GetConfigValue("GPS","posType");
    std::string spotSize = config.GetConfigValue("GPS","spotSize");

    macroFile << "/gps/pos/type " << posType  <<std::endl;

    if (posType == "Volume"){
        macroFile << "/gps/pos/shape Circle" << std::endl;
        macroFile << "/gps/pos/radius " << spotSize << " mm" << std::endl;
    }else{
        macroFile << "/gps/pos/sigma_x "<< spotSize << " mm" << std::endl;
        macroFile << "/gps/pos/sigma_y "<< spotSize << " mm" << std::endl;
    }

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
        macroFile << "/gps/energy " << energy << " MeV "<< std::endl;

        std::string sigmaE = config.GetConfigValue("GPS","sigmaE");
        macroFile << "/gps/ene/sigma " << sigmaE << std::endl; 

    }else if (eneType == "User"){
        macroFile << "/gps/hist/type energy" << std::endl;

        std::string histname = config.GetConfigValue("GPS", "histname");
        macroFile << "/control/execute" << histname << std::endl;
    }

    std::string runType = config.GetConfigValue("Run","type");
    std::string Nevents = config.GetConfigValue("Run","Nevents");
    std::string polDeg = config.GetConfigValue("GPS","polDeg"); 
    std::string Bz = config.GetConfigValue("Solenoid","Bz");

    if (runType=="asymmetry" && polDeg!="0"){
        // one run in each polarization direction !----------------------------------------------------  

        macroFile << "/gps/polarization 0. 0. " << polDeg << std::endl;

        macroFile << "/run/beamOn " << Nevents << std::endl;

        macroFile << "/gps/polarization 0. 0. -" << polDeg << std::endl;

        macroFile << "/run/beamOn " << Nevents << std::endl;

    } else if (runType=="asymmetry" && polDeg=="0"){
        // one run with magnet and iron polarization in one direction and one in the other 
        
        // for the first run the value spezified in the macro is used e.g. 2.2 T
        macroFile << "/run/beamOn " << Nevents << std::endl;

        //Then the Bfield needs to be changed to the negative value 
        macroFile << "/solenoid/setBz -" << Bz << " tesla" << std::endl;
        macroFile << "/run/beamOn " << Nevents << std::endl;

    } else {
        // single run with N events  
        macroFile << "/run/beamOn " << Nevents << std::endl;
    } 
    // Close the file
    macroFile.close();
}