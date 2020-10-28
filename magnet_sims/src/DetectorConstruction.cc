//
/// \brief Implementation of the DetectorConstruction class
//

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"
#include "G4PolarizationManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4UserLimits.hh"



DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction(),
 absmat(0),fWorldMaterial(0),fDefaultWorld(true),
 fSolidWorld(0),fLogicWorld(0),fPhysiWorld(0),
 fSolidAbsorber(0),fLogicAbsorber(0),fPhysiAbsorber(0),
 fDetectorMessenger(0)//,
// fMagnetFieldValue(0.0), fSolidMagnet(0), fLogicMagnet(0), fPhysiMagnet(0),
 //fMagnetSizeX(0), fMagnetSizeY(0), fMagnetSizeZ(0), fMagnetZPos(0)
{
  // default parameter values of the calorimeter
  absthick = 2.0*mm;
  absrad    = 2.5*cm;
  fZposAbs           = 0.*cm;
  ComputeCalorParameters();

  // materials
  DefineMaterials();
  SetWorldMaterial   ("Galactic");
//   SetWorldMaterial   ("Air20");
  SetAbsorberMaterial("Galactic");
//   SetAbsorberMaterial("Air20");

  // create commands for interactive definition of the calorimeter
  fDetectorMessenger = new DetectorMessenger(this);
}



DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}



G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructCalorimeter();
}



void DetectorConstruction::DefineMaterials()
{
  //This function illustrates the possible ways to define materials

  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;

  G4int ncomponents;
  G4double fractionmass;
  G4double temperature, pressure;

  //
  // define Elements
  //


  G4Element* N  = new G4Element("Nitrogen",symbol="N",  z= 7, a=  14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen",  symbol="O",  z= 8, a=  16.00*g/mole);






  new G4Material("Tungsten", z=74, a=183.85*g/mole, density= 19.30*g/cm3);




  // define a material from elements.   case 2: mixture by fractional mass
  //

  G4Material* Air = new G4Material("Air", density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);

  G4Material* Air20 =
    new G4Material("Air20", density= 1.205*mg/cm3, ncomponents=2,
                   kStateGas, 293.*kelvin, 1.*atmosphere);
  Air20->AddElement(N, fractionmass=0.7);
  Air20->AddElement(O, fractionmass=0.3);



//  G4Element* Fe = new G4Element("Iron"  , "Fe", z=26, a=  55.845*g/mole);
//  G4Element* W  = new G4Element("Tungsten","W", z=74, a= 183.850*g/mole);


  //
  // example of vacuum
  //

  density     = universe_mean_density;    //from PhysicalConstants.h
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  new G4Material("Galactic", z=1, a=1.01*g/mole,density,
                 kStateGas,temperature,pressure);
}



void DetectorConstruction::ComputeCalorParameters()
{
  // Compute derived parameters of the calorimeter
  fZstartAbs = fZposAbs-0.5*absthick;
  fZendAbs   = fZposAbs+0.5*absthick;

  if (fDefaultWorld) {
//      fWorldSizeZ = 1.5*absthick; fWorldSizeXY= 1.2*absrad;
//      fWorldSizeZ = 800.0*cm;
     fWorldSizeZ = 400.0*cm;
     fWorldSizeXY= 300.0*cm;
  }
}



G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter()
{
  // Cleanup old geometry
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // World
  //
  fSolidWorld = new G4Box("World",                                //its name
                   fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);   //its size

  fLogicWorld = new G4LogicalVolume(fSolidWorld,          //its solid
                                   fWorldMaterial,        //its material
                                   "World");              //its name

  fPhysiWorld = new G4PVPlacement(0,                      //no rotation
                                   G4ThreeVector(),       //at (0,0,0)
                                 fLogicWorld,             //its logical volume
                                 "World",                 //its name
                                 0,                       //its mother  volume
                                 false,                   //no boolean operation
                                 0);                        //copy number

  // Absorber
  //
  fSolidAbsorber = new G4Tubs("Absorber", //Name
                              0.,         // inner radius
                              absrad,     // outer radius
                              absthick/2., // half length in z
                              0.0*rad,    // starting phi angle
                              twopi*rad); // angle of the segment

  fLogicAbsorber = new G4LogicalVolume(fSolidAbsorber,    //its solid
                                            absmat, //its material
                                          "Absorber");       //its name

  fPhysiAbsorber = new G4PVPlacement(0,                   //no rotation
                        G4ThreeVector(0.,0., fZposAbs),    //its position
                                fLogicAbsorber,     //its logical volume
                                "Absorber",         //its name
                                fLogicWorld,        //its mother
                                false,             //no boulean operat
                                0);                //copy number

  //vacuum step 1
  //
  gap1 = 12.5*mm;
  vacthick = 1.0*mm;
  vac1z = absthick/2.0 + gap1 -vacthick/2.0 - 0.5*mm;

  fSolidVacStep1 = new G4Tubs("VacStep",  //Name
                              0.,         // inner radius
                              absrad,     // outer radius
                              vacthick/2., // half length in z
                              0.0*deg,    // starting phi angle
                              360.0*deg); // angle of the segment

  fLogicVacStep1 = new G4LogicalVolume(fSolidVacStep1,    //its solid
                                       fWorldMaterial,    //its material
                                       "VacStep");       //its name

  fPhysiVacStep1 = new G4PVPlacement(0,                   //no rotation
                       G4ThreeVector(0.,0., vac1z),    //its position
                               fLogicVacStep1,            //its logical volume
                               "VacStep",                 //its name
                               fLogicWorld,               //its mother
                               false,                     //no boulean operat
                               0);                        //copy number


// magnetized iron
//
G4double a, z, density;
G4Material* Iron =  new G4Material("Iron",     z=26, a= 55.85*g/mole, density= 7.870*g/cm3);
 magmat=Iron;
 magthick = 150.0*mm; // thickness of the iron block
 magz = absthick/2.0 + gap1 + magthick/2.0; // z-position of center of iron block
G4double Bz = 2.234*tesla; // z-value of magnetic field
 polvec=G4ThreeVector(0.,0.,-1);


fSolidPolMag = new G4Tubs("PolMag",  //Name
                            0.,         // inner radius
                            absrad,     // outer radius
                            magthick/2., // half length in z
                            0.0*deg,    // starting phi angle
                            360.0*deg); // angle of the segment

fLogicPolMag = new G4LogicalVolume(fSolidPolMag,    //its solid
                                     magmat,    //its material
                                     "PolMag");       //its name

fPhysiPolMag = new G4PVPlacement(0,                   //no rotation
                     G4ThreeVector(0.,0., magz),    //its position
                             fLogicPolMag,            //its logical volume
                             "PolMag",                 //its name
                             fLogicWorld,               //its mother
                             false,                     //no boulean operat
                             0);                        //copy number

// create a magnetic field and its manager
magvec = G4ThreeVector(0.,0.,-Bz);
G4MagneticField*  magField;
magField = new G4UniformMagField(magvec);
G4FieldManager* localFieldMgr = new G4FieldManager(magField);
// associate it with the logical volume of the iron block
 fLogicPolMag->SetFieldManager(localFieldMgr, true);

//create a polarization manager and associate it with the logical volume of the iron Block

G4PolarizationManager * polMgr = G4PolarizationManager::GetInstance();
polMgr->SetVolumePolarization(fLogicPolMag, polvec);




 //vacuum step 2
 //
 G4double gap2 = 10.0*mm; // distance between polarization magnet and photon detector
  vac2z = absthick/2.0 + gap1 + magthick + gap2; // central z-position

 fSolidVacStep2 = new G4Tubs("VacStep2",  //Name
                             0.,         // inner radius
                             absrad,     // outer radius
                             vacthick/2., // half length in z
                             0.0*deg,    // starting phi angle
                             360.0*deg); // angle of the segment

 fLogicVacStep2 = new G4LogicalVolume(fSolidVacStep2,    //its solid
                                      fWorldMaterial,    //its material
                                      "VacStep2");       //its name
 fPhysiVacStep2 = new G4PVPlacement(0,                   //no rotation
                      G4ThreeVector(0.,0., vac2z),    //its position
                              fLogicVacStep2,            //its logical volume
                              "VacStep2",                 //its name
                              fLogicWorld,               //its mother
                              false,                     //no boulean operat
                              0);                        //copy number



  PrintCalorParameters();

  return fPhysiWorld;
}



void DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n" << fWorldMaterial    << G4endl;
  G4cout << "\n" << absmat << G4endl;
G4cout << "\n" << magmat << G4endl;
 G4cout << "\n" << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
  G4cout << "\n The  WORLD "<< G4endl;
G4cout << "\t material: "<< fWorldMaterial->GetName()<< G4endl;
G4cout << "\t Size in Z: "<< G4BestUnit(fWorldSizeZ,"Length")<< G4endl;
G4cout << "\t Size in XY: "<< G4BestUnit(fWorldSizeXY,"Length")<< G4endl;

G4cout << "\n The Reconversion Target "<< G4endl;
G4cout << "\t material: "<< absmat->GetName()<< G4endl;
G4cout << "\t thickness: "<< G4BestUnit(absthick,"Length")<< G4endl;
G4cout << "\t radius: "<< G4BestUnit(absrad,"Length")<< G4endl;
G4cout << "\t Z-position of centre: "<< G4BestUnit(fZposAbs,"Length")<< G4endl;

G4cout << "\n The First Vacuum Step "<< G4endl;
G4cout << "\t thickness: "<< G4BestUnit(vacthick,"Length")<< G4endl;
G4cout << "\t Z-position of centre: "<< G4BestUnit(vac1z,"Length")<< G4endl;

G4cout << "\n The Magnetized Iron Block "<< G4endl;
G4cout << "\t material: "<< magmat->GetName()<< G4endl;
G4cout << "\t thickness: "<< G4BestUnit(magthick,"Length")<< G4endl;
G4cout << "\t Z-position of centre: "<< G4BestUnit(magz,"Length")<< G4endl;
G4cout << "\t Z-component of B-field: "<< G4BestUnit(magvec[2],"Magnetic flux density")<< G4endl;
G4cout << "\t Z-component of Polarization: "<< polvec[2] << G4endl;

G4cout << "\n The Second Vacuum Step "<< G4endl;
G4cout << "\t thickness: "<< G4BestUnit(vacthick,"Length")<< G4endl;
G4cout << "\t Z-position of centre: "<< G4BestUnit(vac2z,"Length")<< G4endl;
G4cout << "\n" << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
/*
  G4cout << "\n The  WORLD   is made of "  << G4BestUnit(fWorldSizeZ,"Length")
         << " of " << fWorldMaterial->GetName();
  G4cout << ". The transverse size (XY) of the world is "
         << G4BestUnit(fWorldSizeXY,"Length") << G4endl;


  G4cout << " The ABSORBER is made of "
         <<G4BestUnit(absthick,"Length")
         << " of " << absmat->GetName();
  G4cout << ". It's radius (XY) is "
         << G4BestUnit(absrad,"Length") << G4endl;
  G4cout << " Z position of the middle of the absorber: "
         << G4BestUnit(fZposAbs,"Length");
  G4cout << G4endl;
  */
}



void DetectorConstruction::SetAbsorberMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial && absmat != pttoMaterial) {
    absmat = pttoMaterial;
    if(fLogicAbsorber) fLogicAbsorber->SetMaterial(absmat);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
}



void DetectorConstruction::SetWorldMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial && fWorldMaterial != pttoMaterial) {
    fWorldMaterial = pttoMaterial;
    if(fLogicWorld) fLogicWorld->SetMaterial(fWorldMaterial);
  //  if(fLogicMagnet) fLogicMagnet->SetMaterial(fWorldMaterial);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
}



void DetectorConstruction::SetAbsorberThickness(G4double val)
{
  absthick = val;
  ComputeCalorParameters();
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}



void DetectorConstruction::SetAbsorberSizeXY(G4double val)
{
  absrad = val;
  ComputeCalorParameters();
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}



void DetectorConstruction::SetWorldSizeZ(G4double val)
{
  fWorldSizeZ = val;
  fDefaultWorld = false;
  ComputeCalorParameters();
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}



void DetectorConstruction::SetWorldSizeXY(G4double val)
{
  fWorldSizeXY = val;
  fDefaultWorld = false;
  ComputeCalorParameters();
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}



void DetectorConstruction::SetAbsorberZpos(G4double val)
{
  fZposAbs  = val;
  ComputeCalorParameters();
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
