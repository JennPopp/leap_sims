//
/// \brief Definition of the DetectorConstruction class
//

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Cache.hh"

class G4Box;
class G4Tubs;
class G4VPhysicalVolume;
class G4Material;
class G4MaterialCutsCouple;
class G4UniformMagField;
class DetectorMessenger;
class G4GlobalMagFieldMessenger;
class G4UserLimits;



class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
   ~DetectorConstruction();

  public:

     void SetAbsorberMaterial (G4String);
     void SetAbsorberThickness(G4double);
     void SetAbsorberSizeXY   (G4double);

     void SetAbsorberZpos(G4double);

     void SetWorldMaterial(G4String);
     void SetWorldSizeZ   (G4double);
     void SetWorldSizeXY  (G4double);

     void SetMagField(G4double);

     virtual G4VPhysicalVolume* Construct();
    // virtual void ConstructSDandField();

    //get method
      const G4VPhysicalVolume* GetVacStep1PV() const;
      const G4VPhysicalVolume* GetVacStep2PV() const;
      const G4VPhysicalVolume* GetAbsorber()   const;

  public:

     void PrintCalorParameters();

     G4Material* GetAbsorberMaterial()  {return absmat;};
     G4double    GetAbsorberThickness() {return absthick;};
     G4double    GetAbsorberSizeXY()    {return absrad;};

     G4double    GetAbsorberZpos()      {return fZposAbs;};
     G4double    GetzstartAbs()         {return fZstartAbs;};
     G4double    GetzendAbs()           {return fZendAbs;};

     G4Material* GetWorldMaterial()     {return fWorldMaterial;};
     G4double    GetWorldSizeZ()        {return fWorldSizeZ;};
     G4double    GetWorldSizeXY()       {return fWorldSizeXY;};

     const G4VPhysicalVolume* GetphysiWorld() {return fPhysiWorld;};

     const G4MaterialCutsCouple* GetAbsorbMaterialCut()  const
                             {return fLogicAbsorber->GetMaterialCutsCouple();};

  //   G4double GetMagnetZend() { return fMagnetZPos + fMagnetSizeZ/2.0; };

  private:

     G4Material*        absmat;
     G4double           absthick;
     G4double           absrad;

     G4Material*        magmat;
     G4double          magthick;
     G4double           magz;
     G4ThreeVector      magvec;
     G4ThreeVector      polvec;

     G4double           fZposAbs;
     G4double           fZstartAbs, fZendAbs;

     G4Material*        fWorldMaterial;
     G4double           fWorldSizeZ;
     G4double           fWorldSizeXY;

     G4bool             fDefaultWorld;

     G4Box*             fSolidWorld;
     G4LogicalVolume*   fLogicWorld;
     G4VPhysicalVolume* fPhysiWorld;

     G4Tubs*            fSolidAbsorber;
     G4LogicalVolume*   fLogicAbsorber;
     G4VPhysicalVolume* fPhysiAbsorber;


     G4double           gap1;
     G4double           vacthick;
     G4double           vac1z;
     G4double           vac2z;

     G4Tubs*            fSolidVacStep1;
     G4LogicalVolume*   fLogicVacStep1;
     G4VPhysicalVolume* fPhysiVacStep1;

     G4Tubs*            fSolidPolMag;
     G4LogicalVolume*   fLogicPolMag;
     G4VPhysicalVolume* fPhysiPolMag;


     G4Tubs*            fSolidVacStep2;
     G4LogicalVolume*   fLogicVacStep2;
     G4VPhysicalVolume* fPhysiVacStep2;

     DetectorMessenger* fDetectorMessenger;
     G4Cache<G4GlobalMagFieldMessenger*> fFieldMessenger;



     /*G4double           fMagnetFieldValue; //2000 - 13000 Gauss
     G4Box*             fSolidMagnet;
     G4LogicalVolume*   fLogicMagnet;
     G4VPhysicalVolume* fPhysiMagnet;
     G4double           fMagnetSizeX, fMagnetSizeY, fMagnetSizeZ;
     G4double           fMagnetZPos;

     G4double            fTelscopeSensorSizeX, fTelscopeSensorSizeY, fTelscopeSensorSizeZ;
     G4double            fTelscopeFirstPlaneZPos, fTelscopePlanesDz ;
     G4int               fTelescopeNPlanes;
     G4Box*              fSolidTelescopeSensor;
     G4LogicalVolume*    fLogicTelescopeSensor;*/

     G4UserLimits       *fStepLimit;
  private:

     void DefineMaterials();
     void ComputeCalorParameters();
     G4VPhysicalVolume* ConstructCalorimeter();

     //void ConstructMagnet();
     //void ConstructTelescope();

};

// inline functions

inline const G4VPhysicalVolume* DetectorConstruction::GetVacStep1PV() const {
  return fPhysiVacStep1;
}
inline const G4VPhysicalVolume* DetectorConstruction::GetVacStep2PV() const {
  return fPhysiVacStep2;
}
inline const G4VPhysicalVolume* DetectorConstruction::GetAbsorber()   const{
  return fPhysiAbsorber;}

#endif
