// Calorimeter.h
#ifndef Calorimeter_h
#define Calorimeter_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "AnaConfigManager.hh"

class Calorimeter {
public:

    // constructor 
    Calorimeter(const ConfigReader& config, AnaConfigManager& anaConfigManager);
    // destructor
    ~Calorimeter();
    // Other public methods and member variables 
    G4LogicalVolume* ConstructCalo();
    void ConstructCalorimeterSD();

    G4double GetVirtCaloLength() const { return fVirtCaloLength; }

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
    G4double fVirtCaloLength;
    G4double ffrontZPos;// front position des 

};

#endif

