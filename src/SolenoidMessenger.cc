#include "SolenoidMessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "Solenoid.hh"

SolenoidMessenger::SolenoidMessenger(Solenoid* solenoid)
  : G4UImessenger(),
    fSolenoid(solenoid)
{
  fDirectory = new G4UIdirectory("/solenoid/");
  fDirectory->SetGuidance("Solenoid control");

  fSetBzCmd = new G4UIcmdWithADoubleAndUnit("/solenoid/setBz", this);
  fSetBzCmd->SetGuidance("Set the magnetic field strength along z.");
  fSetBzCmd->SetParameterName("Bz", false);
  fSetBzCmd->SetUnitCategory("Magnetic flux density");
  fSetBzCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SolenoidMessenger::~SolenoidMessenger() {
    delete fSetBzCmd;
}

void SolenoidMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fSetBzCmd) {
        G4double newBzValue = fSetBzCmd->GetNewDoubleValue(newValue);
        fSolenoid->SetBz(newBzValue);
        fSolenoid->ConstructSolenoidBfield(); // Optionally reconstruct the magnetic field
    }
}