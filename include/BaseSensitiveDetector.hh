// BaseSensitiveDetector.hh 

#ifndef BaseSensitiveDetector_h
#define BaseSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <string>
#include "AnaConfigManager.hh" 

class BaseSensitiveDetector : public G4VSensitiveDetector {
public:
    BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager);
    virtual ~BaseSensitiveDetector();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // Getter methods
    G4double GetEnergySum() const {return fEnergySum;}
    G4int GetTotalCount() const {return fNtot;}
    G4int GetGammaCount() const {return fNgamma;}
    G4double GetGammaEnergySum() const {return fEgammaSum;}
    G4int GetElectronCount() const {return fNe;}
    G4double GetElectronEnergySum() const {return fEeSum;}
    int GetTupleID() const {return fTupleID;}

    // method to reset the member variables 
    void Reset();

private:
    // Member variables initialization
    G4double fEnergySum = 0;
    G4int fNtot = 0;
    G4int fNgamma = 0;
    G4double fEgammaSum = 0;
    G4int fNe = 0;
    G4double fEeSum = 0;

    // Additional private members
    const std::string fLayerIdentifier;
    int fTupleID;
    AnaConfigManager& fAnaConfigManager;
    const std::string fOutputMode;
    
};

#endif // BaseSensitiveDetector_h
