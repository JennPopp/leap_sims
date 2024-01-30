// Solenoid.h
#ifndef SubDetector_h
#define SubDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class Solenoid : public G4VUserDetectorConstruction {
public:
    Solenoid();
    virtual ~Solenoid();

    virtual G4VPhysicalVolume* Construct();
};

#endif
