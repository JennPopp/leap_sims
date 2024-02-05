#include "BaseSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"

BaseSensitiveDetector::BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier)
: G4VSensitiveDetector(name), layerIdentifier(layerIdentifier) {}

BaseSensitiveDetector::~BaseSensitiveDetector() {}

G4bool BaseSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    // Common data processing logic, using layerIdentifier for differentiation

    G4double ene = step->GetPostStepPoint()->GetTotalEnergy()/MeV;
    fEnergySum += ene;

    // Example: Print energy deposition for this step
    G4cout << "Energy at the end of this step: " << ene << " MeV" << G4endl;

    return true;
}