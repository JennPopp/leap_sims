// Solenoid.cc
#include "Solenoid.hh"
#include "Materials.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

Solenoid::Solenoid() : G4VUserDetectorConstruction() {}

Solenoid::~Solenoid() {}

G4VPhysicalVolume* Solenoid::Construct() {
    G4Material* iron = Materials::GetInstance()->GetMaterial("G4_Fe");

    G4double coreRad = 25*mm;
    G4double coreLength = 150*mm;

    G4Tubs* solidCore = new G4Tubs("solidCore", // name of the solid 
                                    0.0*mm, // inner radius 
                                    coreRad, // outer radius 
                                    coreLength/2, // half length  
                                    0.0*deg, //starting angle 
                                    360.0*deg );// end angle 

    G4LogicalVolume* logicCore = new G4LogicalVolume(solidCore, // solid
                                                     iron, // material 
                                                    "logicCore"); // name of logical volume 

    G4VPhysicalVolume* physCore = new G4PVPlacement(0, // no rotation
                                                    G4ThreeVector(), // (0,0,0)
                                                    logicCore, // its logical volume 
                                                    "physBox", // name of physical volume
                                                    0, // its mother volume 
                                                    false, // no boolean operation
                                                    0); // copy number 

    return physCore;
}