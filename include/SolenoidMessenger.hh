#ifndef SolenoidMessenger_h
#define SolenoidMessenger_h

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"

// Forward declaration
class Solenoid;

class SolenoidMessenger : public G4UImessenger {
private:
    Solenoid* fSolenoid; // Pointer to the solenoid
    G4UIcmdWithADouble* fSetBzCmd; // Command to set fBz
    G4UIdirectory* fDirectory;
    G4UIcmdWithoutParameter* fListFieldsCmd;

public:
    SolenoidMessenger(Solenoid* solenoid);
    ~SolenoidMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;
    void ListMagneticFields(); 
};

#endif // SolenoidMessenger_h
