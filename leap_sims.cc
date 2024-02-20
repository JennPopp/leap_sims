//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//


#include "DetectorConstruction.hh"
#include "ConfigReader.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "MacroGenerator.hh"
#include "AnaConfigManager.hh"
#include "GpsPrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
namespace leap {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " leap_sims [-c configFileName of type string] [-v visualState of type bool]"  << G4endl;
  }
}

using namespace leap;

int main(int argc,char** argv)
{
  // Get config name and visualization based on input or set default values 
  G4String configFileName;
  bool visualState = false; // Use bool for visualState
  for (G4int i = 1; i < argc; i = i + 2) {
    if (G4String(argv[i]) == "-c") {
      configFileName = argv[i + 1];
    } else if (G4String(argv[i]) == "-v") {
      // Check next argument to determine true or false explicitly
      G4String argValue = argv[i + 1];
      if (argValue == "true" || argValue == "1") {
        visualState = true;
      } else if (argValue == "false" || argValue == "0") {
        visualState = false;
      } else {
        PrintUsage(); // Invalid value for -v
        return 1;
      } 
    } else {
      PrintUsage();
      return 1;
    }
  }
  if (configFileName.empty()) { configFileName = "config.ini"; }

  // Read the configuration file 
  ConfigReader config(configFileName);
  if (!config.ReadConfig()) {
      std::cerr << "Failed to read configuration file." << std::endl;
      return 1;
  }
  
  // Detect interactive mode and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( visualState == 1 ) { ui = new G4UIExecutive(argc, argv); }

  // generate the macro based on config parameters
  G4String macroFileName;
  macroFileName = config.GetConfigValue("Input","macroName");
  MacroGenerator::generateMacro(config, macroFileName );

  // Construct the the analysis configuration manager 
  AnaConfigManager ana(config);

  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // Set mandatory initialization classes
  DetectorConstruction* detector = new DetectorConstruction(config, ana);
  runManager->SetUserInitialization(detector);

  PhysicsList* physList = new PhysicsList(config);
  runManager->SetUserInitialization(physList);

  // Set mandatory user action class
  runManager->SetUserAction(new GpsPrimaryGeneratorAction());
  RunAction* run ;
  runManager->SetUserAction(run = new RunAction(ana));
  runManager->SetUserAction(new EventAction(ana));

  // Initialize the run manager 
  runManager->Initialize();

// Initialize visualization
  //
  //G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();


  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macroFileName);
  }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
  delete visManager;
  delete runManager;
}
