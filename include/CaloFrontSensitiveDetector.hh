#ifndef CaloFrontSensitiveDetector_h
#define CaloFrontSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <string>
#include "AnaConfigManager.hh" 

class CaloFrontSensitiveDetector : public G4VSensitiveDetector {
public:
    CaloFrontSensitiveDetector(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager,const G4double frontZPos);
    virtual ~CaloFrontSensitiveDetector();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // Getter methods
    std::vector<double> GetEnergySum() const {return fEnergySum;}
    std::vector<int> GetTotalCount() const {return fNtot;}
    int GetTupleID() const {return fTupleID;}

    // method to reset the member variables 
    void Reset();

private:
    // Member variables initialization
    std::vector<double> fEnergySum;
    std::vector<int> fNtot;
    G4double fEinLim;
    G4double fEin_tot;
    G4double ffrontZPos; // posisiton of the front sensitive detector 
    // Additional private members
    const std::string fLayerIdentifier;
    int fTupleID;
    AnaConfigManager& fAnaConfigManager;
    const std::string fOutputMode;
    
};

#endif //CaloFrontSensitiveDetector_h