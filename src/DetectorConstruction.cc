// DetectorConstruction.cc
#include "DetectorConstruction.hh"
#include "Solenoid.hh"
#include "Materials.hh"
#include "ConfigReader.hh"
#include "G4SDManager.hh"
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
    fBeamLine = new BeamLine(config, anaConfigManager);


    //initialize other geometry member variables 
    fWorldMaterial = config.GetConfigValue("World", "material");
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
  delete fBeamLine;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // world volume................................................................ 
  G4double worldSize = 4000*mm;
  G4Material* worldMat = Materials::GetInstance()->GetMaterial(fWorldMaterial);
  G4Box* solidWorld = new G4Box("solidWorld", worldSize, worldSize, worldSize);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0);

  // place the beamline .........................................................
  if(fConfig.GetConfigValueAsInt("BeamLine","beamLineStatus")){
    G4LogicalVolume* logicBeamLine = fBeamLine->ConstructBeamLine();
    G4double lengthBL = fBeamLine->GetLengthBL();
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,-lengthBL/2.0+1*mm), 
                      logicBeamLine, 
                      "physicalBeamLine", 
                      logicWorld, 
                      false, 
                      0); 

  }
  // The solenoid ...............................................................

  G4double magThick = 0;
  if (fConfig.GetConfigValueAsInt("Solenoid","solenoidStatus")){
    G4LogicalVolume* logicSolenoid = fSolenoid->ConstructSolenoid();
    magThick = fSolenoid->GetMagThick();
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,magThick/2.0), 
                      logicSolenoid, 
                      "physicalSolenoid", 
                      logicWorld, 
                      false, 
                      0); 
  }
  G4cout << ".....................................................................................................magThick is "<< magThick << G4endl;
  if (fConfig.GetConfigValueAsInt("Calorimeter","calorimeterStatus")){
    G4LogicalVolume* logicCalo = fCalo->ConstructCalo();
    G4double caloLength = fCalo->GetVirtCaloLength();
    G4cout << ".....................................................................................................caloLength is "<< caloLength << G4endl;
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,magThick+fDist2Pol+caloLength/2.), 
                      logicCalo, 
                      "physicalCalo", 
                      logicWorld, 
                      false, 
                      0); 
  }

  return physWorld;
}

void DetectorConstruction::ConstructSDandField(){
  if (fConfig.GetConfigValueAsInt("Solenoid","solenoidStatus")){
    fSolenoid->ConstructSolenoidSD();
    
    if(fConfig.GetConfigValueAsInt("Solenoid","BField")){
      fSolenoid->ConstructSolenoidBfield();
    }
  } 

  if(fConfig.GetConfigValueAsInt("Calorimeter","calorimeterStatus")){
    fCalo->ConstructCalorimeterSD();
  }
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  sdManager->ListTree();
}
} // namespace leap