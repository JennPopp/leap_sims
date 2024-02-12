//BaseSensitiveDetector.hh

#ifndef BaseSensitiveDetector_h
#define BaseSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include <string>
#include "ConfigReader.hh"

class BaseSensitiveDetector : public G4VSensitiveDetector {
public:
    BaseSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID);
    virtual ~BaseSensitiveDetector();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // Getter methods
    G4double GetEnergySum() const { return fEnergySum; }
    G4double GetTotalCount() const { return fNtot; }
    G4double GetGammaCount() const { return fNgamma; }
    G4double GetGammaEnergySum() const { return fEgammaSum; }
    G4double GetElectronCount() const { return fNe; }
    G4double GetElectronEnergySum() const { return fEeSum; }
    int GetTupleID() const { return fTupleID; }

    // method to reset the member variables 
    void Reset();

protected:
    std::string fLayerIdentifier;
    ConfigReader fConfig;
    std::string fOutputMode;
    G4AnalysisManager* fAnalysisManager;
    int fTupleID;
    

private:
    // using convention to add an f infront of member variables
    G4double fEnergySum;
    G4double fNtot;
    G4double fNgamma;
    G4double fEgammaSum;
    G4double fNe;
    G4double fEeSum;

    //
    
};

#endif
