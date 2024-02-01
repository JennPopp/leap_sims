// Materials.cc
#include "Materials.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"

Materials* Materials::fInstance = nullptr;

Materials::Materials() {
    DefineMaterials();
}

Materials* Materials::GetInstance() {
    if (!fInstance) {
        fInstance = new Materials();
    }
    return fInstance;
}

G4Material* Materials::GetMaterial(const G4String& materialName) {
    G4NistManager* nistManager = G4NistManager::Instance();
    return nistManager->FindOrBuildMaterial(materialName);
}

void Materials::DefineMaterials() {

    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons;
    G4double density;
    G4int ncomponents, natoms;
    G4String name, symbol;
    G4double fractionmass;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // define elements
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    G4Element* O       = new G4Element(name="Oxygen"  ,symbol="O"   , z= 8  , a = 15.099*g/mole);
    G4Element* N       = new G4Element(name="Nitrogen",symbol="N"   , z= 7  , a = 14.007*g/mole);
    G4Element* Ar      = new G4Element(name="Argon"   ,symbol="Ar"  , z=18  , a = 39.948*g/mole);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // define materials
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    G4NistManager* nistManager = G4NistManager::Instance();
    // Materials available with nist manager 
    G4Material* iron = nistManager->FindOrBuildMaterial("G4_Fe");
    G4Material* lead = nistManager->FindOrBuildMaterial("G4_Pb");
    G4Material* copper = nistManager->FindOrBuildMaterial("G4_Cu");
    G4Material* tungsten = nistManager->FindOrBuildMaterial("G4_W");

    // Air
    G4Material* Air = new G4Material("Air" , density = 1.290*mg/cm3, ncomponents = 3);
    Air->AddElement(N, 78*perCent);
    Air->AddElement(O, 21*perCent);
    Air->AddElement(Ar, 1*perCent);

    // Vacuum
    G4Material* Galactic = new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
}
