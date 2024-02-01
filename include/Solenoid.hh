// Solenoid.h
#ifndef SubDetector_h
#define SubDetector_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"

class Solenoid : public G4VUserDetectorConstruction {
public:

    // constructor 
    Solenoid(const ConfigReader& config);
    // destructor
    ~Solenoid();
    // Other public methods and member variables 
    G4VPhysicalVolume* Construct();

private:
    G4double fCoreRad;
    G4double fCoreLength;
    G4double fConvThick; 
    G4String fType; // TP1 was used for design study, TP2 is in the lab
    G4String fPolStatus;
    G4String fWorldMaterial;
};

#endif