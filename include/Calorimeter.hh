// Calorimeter.h
#ifndef Calorimeter_h
#define SCalorimeter_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "AnaConfigManager.hh"

class Calorimeter : public G4VUserDetectorConstruction {
public:

    // constructor 
    Calorimeter(const ConfigReader& config, AnaConfigManager& anaConfigManager);
    // destructor
    ~Calorimeter();
    // Other public methods and member variables 
    G4VPhysicalVolume* Construct();
    void ConstructCalorimeterSD();

private:
    const ConfigReader& fConfig;
    AnaConfigManager& fAnaConfigManager;
    G4int fCaloState;
    G4String fType;
    G4int fNcrystals;
    G4String fCaloMat;
    G4String fWorldMaterial;
    G4LogicalVolume* fLogicCrystal;
    G4LogicalVolume* fLogicFrontDet;
    G4LogicalVolume* fLogicBackDet;


};

#endif

