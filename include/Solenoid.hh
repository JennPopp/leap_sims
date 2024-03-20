// Solenoid.h
#ifndef Solenoid_h
#define Solenoid_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "AnaConfigManager.hh"
#include "SolenoidMessenger.hh"

class Solenoid{
public:

    // constructor 
    Solenoid(const ConfigReader& config, AnaConfigManager& anaConfigManager);
    // destructor
    ~Solenoid();
    // Other public methods and member variables 
    G4LogicalVolume* ConstructSolenoid();
    void ConstructSolenoidSD();
    void ConstructSolenoidBfield();
    void SetBz(G4double newBz);

     // Getter method for magThick
    G4double GetMagThick() const { return fMagThick; }

private:
    const ConfigReader& fConfig;
    AnaConfigManager& fAnaConfigManager;
    SolenoidMessenger* fMessenger;
    G4double fCoreRad;
    G4double fCoreLength;
    G4double fConvThick; 
    G4double fPolDeg;
    G4String fType; // TP1 was used for design study, TP2 is in the lab
    G4int fPolStatus;
    G4int fBeamLineStatus;
    G4int fLanexStatus;
    G4int fTableStatus;
    G4String fWorldMaterial;
    G4LogicalVolume* fLogicVacStep1;
    G4LogicalVolume* fLogicVacStep2;
    G4LogicalVolume* fLogicCore;
    G4double fBz;
    G4double fMagThick;
};

#endif
