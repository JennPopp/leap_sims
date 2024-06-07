#include "SolenoidMessenger.hh"
#include "Solenoid.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Field.hh"




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

  fListFieldsCmd = new G4UIcmdWithoutParameter("/solenoid/listMagneticFields", this);
  fListFieldsCmd->SetGuidance("List all registered magnetic fields.");

}

SolenoidMessenger::~SolenoidMessenger() {
    delete fSetBzCmd;
    delete fListFieldsCmd;
    delete fDirectory;
}

void SolenoidMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fSetBzCmd) {
        fSolenoid->SetBz(fSetBzCmd->GetNewDoubleValue(newValue));
    } else if (command == fListFieldsCmd) {
        ListMagneticFields();
    }
}

void SolenoidMessenger::ListMagneticFields() {
    G4TransportationManager* transportMgr = G4TransportationManager::GetTransportationManager();
    G4FieldManager* fieldMgr = transportMgr->GetFieldManager();
    const G4Field* field = fieldMgr->GetDetectorField();

    if (const G4MagneticField* magField = dynamic_cast<const G4MagneticField*>(field)) {
        G4cout << "Magnetic Field registered." << G4endl;

        // Example: Handling a uniform magnetic field
        if (const G4UniformMagField* uniformField = dynamic_cast<const G4UniformMagField*>(magField)) {
            G4double point[4] = {0.0, 0.0, 0.0, 0.0}; // Example point (x, y, z, t)
            G4double fieldValue[3]; // Bx, By, Bz
            uniformField->GetFieldValue(point, fieldValue);
            G4cout << "Uniform Magnetic Field Value: "
                   << fieldValue[0] << " tesla, "
                   << fieldValue[1] << " tesla, "
                   << fieldValue[2] << " tesla" << G4endl;
        } else {
            G4cout << "Non-uniform magnetic field detected. Field values at specific points:" << G4endl;
            // Sample the field at a specific point
            G4double samplePoint[4] = {0.0, 0.0, 0.0, 0.0}; // Example point (x, y, z, t)
            G4double fieldValue[6];
            magField->GetFieldValue(samplePoint, fieldValue);
            G4cout << "Magnetic Field at (0,0,0): "
                   << fieldValue[0] << " tesla, "
                   << fieldValue[1] << " tesla, "
                   << fieldValue[2] << " tesla" << G4endl;
        }
    } else {
        G4cout << "No magnetic fields registered." << G4endl;
    }
}