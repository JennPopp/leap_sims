#ifndef BaseSensitiveDetector_h
#define BaseSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <string>

class BaseSensitiveDetector : public G4VSensitiveDetector {
public:
    BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier);
    virtual ~BaseSensitiveDetector();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

protected:
    std::string layerIdentifier;

private:
    // Example of additional member variable to store energy sum
    G4double fEnergySum;
};

#endif
