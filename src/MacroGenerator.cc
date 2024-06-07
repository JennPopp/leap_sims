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
    
    macroFile << "/run/printProgress 1000" << std::endl; 
    macroFile << "/run/initialize" << std::endl; 

    // polarization manager, only callable ofter initialization -----------------------------------
    auto solenoidStatus = config.GetConfigValueAsInt("Solenoid","solenoidStatus");
    if (solenoidStatus){
        macroFile << "/polarization/manager/verbose 1" << std::endl;
        macroFile << "/polarization/volume/list" << std::endl;
    }
    // print the registered magnetic fields -----------------------------------------------------
    macroFile << "/solenoid/listMagneticFields"<<std::endl;


    // change final range of electron loss function to 0.01--------------------------------------------------------------------
    macroFile <<"/process/eLoss/StepFunction" << " 0.2 0.01 mm" << std::endl;

    // gps commands -------------------------------------------------------------------------------
    int nBunch = config.GetConfigValueAsInt("GPS","nBunch");
    macroFile <<"/gps/number " << nBunch << std::endl;

    std::string particle = config.GetConfigValue("GPS","particle");
    macroFile <<"/gps/particle " << particle << std::endl;

    std::string position = config.GetConfigValue("GPS","position");
    macroFile <<"/gps/pos/centre " << position << " mm" << std::endl; 

    std::string posType = config.GetConfigValue("GPS","posType");
    std::string spotSize = config.GetConfigValue("GPS","spotSize");

    if (posType == "Plane"){
        macroFile << "/gps/pos/type " << posType  <<std::endl;
        macroFile << "/gps/pos/shape Circle" << std::endl;
        macroFile << "/gps/pos/radius " << spotSize << " mm" << std::endl;
    } else if (posType == "Square") {
        macroFile << "/gps/pos/type " << "Plane"  <<std::endl;
        macroFile << "/gps/pos/shape Square" << std::endl;
        macroFile << "/gps/pos/halfx " << spotSize << " mm" << std::endl;
        macroFile << "/gps/pos/halfy " << spotSize << " mm" << std::endl;
    } else{
        macroFile << "/gps/pos/type " << posType  <<std::endl;
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
        //macroFile << "/gps/energy " << energy << " MeV "<< std::endl;
        macroFile << "/gps/ene/mono " << energy << " MeV "<< std::endl;

        std::string sigmaE = config.GetConfigValue("GPS","sigmaE");
        macroFile << "/gps/ene/sigma " << sigmaE << " MeV "<< std::endl; 

    }else if (eneType == "User"){
        macroFile << "/gps/hist/type energy" << std::endl;

        std::string histname = config.GetConfigValue("GPS", "histname");
        macroFile << "/control/execute " << histname << std::endl;
    }

    std::string runType = config.GetConfigValue("Run","type");
    std::string Nevents = config.GetConfigValue("Run","Nevents");
    double polDeg = config.GetConfigValueAsDouble("GPS","polDeg"); 
    std::string Bz = config.GetConfigValue("Solenoid","Bz");
    double polDegSol = config.GetConfigValueAsDouble("Solenoid","polDeg");
    std::string flip = config.GetConfigValue("Run","flip");
    auto Bstat = config.GetConfigValueAsInt("Solenoid","BField");
    // if runType is asymmetry two runs are started, if runtype is single just one run is started
    // if flip == source the polarization of the electrons is flipped 
    // if flip == core the polarization of core and B-Field is flipped
    

    if (runType=="asymmetry" && flip=="source"){
        // one run in each polarization direction !----------------------------------------------------  

        macroFile << "/gps/polarization 0. 0. " << std::to_string(polDeg) << std::endl;

        macroFile << "/run/beamOn " << Nevents << std::endl;

        macroFile << "/gps/polarization 0. 0. -" << std::to_string(polDeg) << std::endl;

        macroFile << "/run/beamOn " << Nevents << std::endl;

    } else if (runType=="asymmetry" && flip=="core" && solenoidStatus==1 &&( Bstat==1 || polDegSol>0.0)){
        // one run with magnet and iron polarization in one direction and one in the other 
        
        if (polDeg != 0){
            macroFile << "/gps/polarization 0. 0. " << std::to_string(polDeg) << std::endl;
        }

        // for the first run the value spezified in the init is used e.g. 2.2 T, same for pol degree 
        macroFile << "/run/beamOn " << Nevents << std::endl;

        //Then the Bfield and or the pol degree needs to be changed to the negative value 
        if (Bstat){
            macroFile << "/solenoid/setBz -" << Bz << " tesla" << std::endl;
            macroFile << "/solenoid/listMagneticFields"<<std::endl;
        }
        
        if (polDegSol>0.0){
            macroFile << "/polarization/volume/set logicCore 0. 0. -"<< std::to_string(polDegSol) << std::endl;
        }
        macroFile << "/run/beamOn " << Nevents << std::endl;
        
        

    } else {
        // single run with N events  
        macroFile << "/run/beamOn " << Nevents << std::endl;
    } 
    // Close the file
    macroFile.close();
}