// BeamLine.hh
#ifndef BeamLine_h
#define BeamLine_h 1 

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "AnaConfigManager.hh"

class BeamLine{

public:

    //constructor
    BeamLine(const ConfigReader& config, AnaConfigManager& anaConfigManager);
    //destructor
    ~BeamLine();
    //other public methods
    G4LogicalVolume* ConstructBeamLine();
    void ConstructBeamLineSD();

    G4double GetLengthBL() const { return fLengthBL; }

private:

    const ConfigReader& fConfig;
    AnaConfigManager& fAnaConfigManager;
    G4int fBeamLineState;
    G4String fWorldMaterial;
    G4LogicalVolume* fLogicBLMother;
    G4double fLengthBL;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif