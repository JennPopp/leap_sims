// DetectorConstruction.cc
#include "DetectorConstruction.hh"
#include "Solenoid.hh"
#include "Materials.hh"
#include "ConfigReader.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

namespace leap
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(const ConfigReader& config, AnaConfigManager& anaConfigManager) 
  : G4VUserDetectorConstruction(),fConfig(config), fAnaConfigManager(anaConfigManager) {
    // Create instances of subdetectors and pass the confic object
    fSolenoid = new Solenoid(config, anaConfigManager);
    fCalo = new Calorimeter(config, anaConfigManager);

    //initialize other geometry member variables 
    if (fConfig.GetConfigValueAsInt("Solenoid","solenoidStatus")){
      fDist2Pol = config.GetConfigValueAsDouble("Calorimeter","dist2Pol")*mm;
    } else {
      fDist2Pol = 0*mm;
    }
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::~DetectorConstruction() {
  delete fSolenoid;
  delete fCalo; 
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // world volume 
  G4double worldSize = 2000*mm;
  G4Box* solidWorld = new G4Box("solidWorld", worldSize, worldSize, worldSize);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, Materials::GetInstance()->GetMaterial("G4_AIR"), "logicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0);


  // Place the solenoid into the world 
  // create an instance of the solenoid class
  if (fConfig.GetConfigValueAsInt("Solenoid","solenoidStatus")){
    G4VPhysicalVolume* solenoidVolume = fSolenoid->Construct();
    new G4PVPlacement(0,
                      G4ThreeVector(), 
                      solenoidVolume->GetLogicalVolume(), 
                      solenoidVolume->GetName(), 
                      logicWorld, 
                      false, 
                      0); 
  }

  return physWorld;
}

void DetectorConstruction::ConstructSDandField(){
  if (fConfig.GetConfigValueAsInt("Solenoid","solenoidStatus")){
    fSolenoid->ConstructSolenoidSD();
  } 
  if(fConfig.GetConfigValueAsInt("Solenoid","BField")){
    fSolenoid->ConstructSolenoidBfield();
  }
}
} // namespace leap