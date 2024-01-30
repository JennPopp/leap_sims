// DetectorConstruction.cc
#include "DetectorConstruction.hh"
#include "Solenoid.hh"
#include "Materials.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

namespace leap
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::~DetectorConstruction() {}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // world volume 
  G4double worldSize = 500*mm;
  G4Box* solidWorld = new G4Box("solidWorld", worldSize, worldSize, worldSize);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, Materials::GetInstance()->GetMaterial("G4_AIR"), "logicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0);


  // Place the solenoid into the world 
  // create an instance of the solenoid class
  Solenoid solenoid;

  G4VPhysicalVolume* solenoidVolume = solenoid.Construct();
  new G4PVPlacement(0,
                    G4ThreeVector(), 
                    solenoidVolume->GetLogicalVolume(), 
                    solenoidVolume->GetName(), 
                    logicWorld, 
                    false, 
                    0);

  return physWorld;
}
}