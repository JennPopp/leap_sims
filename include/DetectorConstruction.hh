
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "ConfigReader.hh"
#include "Solenoid.hh"

namespace leap
{



class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(const ConfigReader& config);
    ~DetectorConstruction() override;

    // methods from base class
    G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

  private:
    // Configuration reader
    const ConfigReader& fConfig;

    // pointers to subdetectors 
    Solenoid* fSolenoid;

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

