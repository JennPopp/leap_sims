// Solenoid.h
#ifndef SubDetector_h
#define SubDetector_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "AnaConfigManager.hh"

class Solenoid : public G4VUserDetectorConstruction {
public:

    // constructor 
    Solenoid(const ConfigReader& config, AnaConfigManager& anaConfigManager);
    // destructor
    ~Solenoid();
    // Other public methods and member variables 
    G4VPhysicalVolume* Construct();
    void ConstructSolenoidSD();

private:
    const ConfigReader& fConfig;
    AnaConfigManager& fAnaConfigManager;
    G4double fCoreRad;
    G4double fCoreLength;
    G4double fConvThick; 
    G4double fPolDeg;
    G4String fType; // TP1 was used for design study, TP2 is in the lab
    G4int fPolStatus;
    //G4String fBeamLineStatus;
    G4String fWorldMaterial;
    G4LogicalVolume* fLogicVacStep1;
    G4LogicalVolume* fLogicVacStep2;
};

#endif
