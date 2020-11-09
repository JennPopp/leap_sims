//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4PolarizationManager.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fAbsorberPV(nullptr),
   fMagnetPV(nullptr),
   fCheckOverlaps(true)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Iron and Tungsten material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_W");

  // Vacuum
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double worldSizeXY = 3.*m;
  G4double worldSizeZ  = 4.*m;
  G4double absthick = 2.*mm;
  G4double absrad=25.*mm;
  G4double magthick =  15.*cm;
  G4double vacthick  = 1.*mm;
  G4double gap1=12.5*mm;
  G4double gap2=10.*mm;

  auto ZposAbs=0*cm;
  auto ZposMag=absthick/2.0 + gap1 + magthick/2.0;
  auto ZposVac1=absthick/2.0 +gap1-1.*mm-vacthick/2.0;
  auto ZposVac2=absthick/2.0+gap1+magthick+gap2+vacthick/2.0;


  // Get materials
  auto worldMat = G4Material::GetMaterial("Galactic");
  //auto absMat = G4Material::GetMaterial("G4_W");
  auto absMat = G4Material::GetMaterial("Galactic");
  auto magMat = G4Material::GetMaterial("G4_Fe");

  if ( ! worldMat || ! absMat || ! magMat ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 worldMat,  // its material
                 "World");         // its name

  auto worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

  //
  // Absorber
  //
  auto absorberS
    = new G4Tubs("Absorber", //Name
                0.,         // inner radius
                absrad,     // outer radius
                absthick/2., // half length in z
                0.0*rad,    // starting phi angle
                twopi*rad); // angle of the segment

  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,     // its solid
                 absMat,  // its material
                 "Absorber");   // its name
  fAbsorberPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 absorberLV,          // its logical volume
                 "Absorber",    // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps


   //
   //vacuum step 1
   //
   auto
   fVacStepS1 = new G4Tubs("VacStep",  //Name
                               0.,         // inner radius
                               absrad,     // outer radius
                               vacthick/2., // half length in z
                               0.0*deg,    // starting phi angle
                               360.0*deg); // angle of the segment

  auto
   fVacStepLV1 = new G4LogicalVolume(fVacStepS1,    //its solid
                                        worldMat,    //its material
                                        "VacStep");  //its name

   fVacStepPV1 = new G4PVPlacement(0,                   //no rotation
                        G4ThreeVector(0.,0., ZposVac1),    //its position
                                fVacStepLV1,            //its logical volume
                                "VacStep",                 //its name
                                worldLV,               //its mother
                                false,                     //no boolean operat
                                0);                        //copy number

  //
  // Magnet
  //
 auto magnetS
   = new G4Tubs("magnetS", //Name
               0.,         // inner radius
               absrad,     // outer radius
               magthick/2., // half length in z
               0.0*rad,    // starting phi angle
               twopi*rad); // angle of the segment

  auto magnetLV
    = new G4LogicalVolume(
                 magnetS,        // its solid
                 magMat, // its material
                 "magnetLV");          // its name

  fMagnetPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., ZposMag), // its position
                 magnetLV,       // its logical volume
                 "magnetPV",           // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

 auto polvec=G4ThreeVector(0.,0.,1.0);
 G4PolarizationManager * polMgr = G4PolarizationManager::GetInstance();
 polMgr->SetVolumePolarization(magnetLV, polvec);

 //
 //vacuum step 2
 //
auto fVacStepS2 = new G4Tubs("VacStep2",  //Name
                             0.,         // inner radius
                             absrad,     // outer radius
                             vacthick/2., // half length in z
                             0.0*deg,    // starting phi angle
                             360.0*deg); // angle of the segment

 auto fVacStepLV2 = new G4LogicalVolume(fVacStepS2,    //its solid
                                      worldMat,    //its material
                                      "VacStep2");       //its name

 fVacStepPV2 = new G4PVPlacement(0,                   //no rotation
                      G4ThreeVector(0.,0., ZposVac2),    //its position
                              fVacStepLV2,            //its logical volume
                              "VacStep2",                 //its name
                              worldLV,               //its mother
                              false,                     //no boolean operat
                              0);                        //copy number


  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "\n The  WORLD "<< G4endl
    << "\t material: "<< worldMat->GetName()<< G4endl
    << "\t Size in Z: "<< G4BestUnit(worldSizeZ,"Length")<< G4endl
    << "\t Size in XY: "<< G4BestUnit(worldSizeXY,"Length")<< G4endl

    << "\n The Reconversion Target "<< G4endl
    << "\t material: "<< absMat->GetName()<< G4endl
    << "\t thickness: "<< G4BestUnit(absthick,"Length")<< G4endl
    << "\t radius: "<< G4BestUnit(absrad,"Length")<< G4endl
    << "\t Z-position of centre: "<< G4BestUnit(ZposAbs,"Length")<< G4endl

    << "\n The First Vacuum Step "<< G4endl
    << "\t thickness: "<< G4BestUnit(vacthick,"Length")<< G4endl
    << "\t Z-position of centre: "<< G4BestUnit(ZposVac1,"Length")<< G4endl

    << "\n The Magnetized Iron Block "<< G4endl
    << "\t material: "<< magMat->GetName()<< G4endl
    << "\t thickness: "<< G4BestUnit(magthick,"Length")<< G4endl
    << "\t Z-position of centre: "<< G4BestUnit(ZposMag,"Length")<< G4endl
    //<< "\t Z-component of B-field: "<< G4BestUnit(magvec[2],"Magnetic flux density")<< G4endl
    << "\t Z-component of Polarization: "<< polvec[2] << G4endl

    << "\n The Second Vacuum Step "<< G4endl
    << "\t thickness: "<< G4BestUnit(vacthick,"Length")<< G4endl
    << "\t Z-position of centre: "<< G4BestUnit(ZposVac2,"Length")<< G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  magnetLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
