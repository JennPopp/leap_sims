#ifndef CaloCrystalSD_h
#define CaloCrystalSD_h 1

#include "G4VSensitiveDetector.hh"
#include <string>
#include "AnaConfigManager.hh" 

class CaloCrystalSD: public G4VSensitiveDetector {
public:
    CaloCrystalSD(const G4String& name, const std::string& layerIdentifier, int tupleID, AnaConfigManager& anaConfigManager);
    virtual ~CaloCrystalSD();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // Getter methods
    std::vector<double> GetEdepTot() const {return fEdepTot;}
    std::vector<double> GetTlengthTot() const {return fTlengthTot;}
    int GetTupleID() const {return fTupleID;}

    // method to reset the member variables 
    void Reset();

private:
    // Member variables initialization
    std::vector<double> fEdepTot;
    std::vector<double> fTlengthTot;

    // Additional private members
    const std::string fLayerIdentifier;
    int fTupleID;
    AnaConfigManager& fAnaConfigManager;
    const int fShowerDevStat;
    
};

#endif // CaloCrystalSD