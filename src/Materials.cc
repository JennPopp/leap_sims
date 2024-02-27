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
    G4Element* C       = new G4Element(name="Carbon"  ,symbol="C"   , z= 6  , a = 12.011*g/mole);
    G4Element* H       = new G4Element(name="Hydrogen",symbol="H"   , z= 1  , a = 1.008*g/mole);
    G4Element* As      = new G4Element(name="Arsenic" ,symbol="As"  , z= 33 , a = 74.922*g/mole);
    G4Element* Al      = new G4Element(name="Aluminum",symbol="Al"  , z= 13 , a = 26.982*g/mole);
    G4Element* Si      = new G4Element(name="Silicon" ,symbol="Si"  , z= 14 , a = 28.086*g/mole);
    G4Element* N       = new G4Element(name="Nitrogen",symbol="N"   , z= 7  , a = 14.007*g/mole);
    G4Element* Ar      = new G4Element(name="Argon"   ,symbol="Ar"  , z=18  , a = 39.948*g/mole);
    G4Element* Ce      = new G4Element(name="Cer"     ,symbol="Ce"  , z=58  , a = 140.116*g/mole);
    G4Element* Pb      = new G4Element(name="Blei"    ,symbol="Pb"  , z=82  , a = 207.2*g/mole);
    G4Element* Na      = new G4Element(name="Natrium" ,symbol="Na"  , z=11  , a = 22.9898*g/mole);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // define materials
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    G4NistManager* nistManager = G4NistManager::Instance();
    // Materials available with nist manager 
    G4Material* iron = nistManager->FindOrBuildMaterial("G4_Fe");
    G4Material* lead = nistManager->FindOrBuildMaterial("G4_Pb");
    G4Material* copper = nistManager->FindOrBuildMaterial("G4_Cu");
    G4Material* tungsten = nistManager->FindOrBuildMaterial("G4_W");
    G4Material* steel = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    G4Material* lanex = nistManager->FindOrBuildMaterial("G4_GADOLINIUM_OXYSULFIDE");
    G4Material* PbO = nistManager->FindOrBuildMaterial("G4_LEAD_OXIDE");
    G4Material* SiO2 = nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* K2O = nistManager->FindOrBuildMaterial("G4_POTASSIUM_OXIDE");

    // Air
    G4Material* Air = new G4Material("Air" , density = 1.290*mg/cm3, ncomponents = 3);
    Air->AddElement(N, 78*perCent);
    Air->AddElement(O, 21*perCent);
    Air->AddElement(Ar, 1*perCent);

    // Vacuum
    G4Material* Galactic = new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);


    density = 3.74*g/cm3;
    G4Material* As2O3 = new G4Material(name="Arsenictrioxide", density, ncomponents=2);
    As2O3->AddElement(As, natoms=2);
    As2O3->AddElement(O , natoms=3);

    density = 8.3*g/cm3; //wikipedia
    G4Material* Pb3O4 = new G4Material(name="RedLead", density, ncomponents=2);
    Pb3O4->AddElement(Pb, natoms=3);
    Pb3O4->AddElement(O , natoms=4);

    density = 2.27*g/cm3; //wikipedia
    G4Material* Na2O = new G4Material(name="Sodiumoxide", density, ncomponents=2);
    Na2O->AddElement(Na, natoms=2);
    Na2O->AddElement(O , natoms=1);

    density = 6.773*g/cm3; //wikipedia
    G4Material* Cer = new G4Material(name="Cer", density, ncomponents=1);
    Cer->AddElement(Ce , natoms=1);

    // Lead Glass TF1 with the chemical composition Maryna gave to FS
    G4Material* TF1 = new G4Material("TF1", density= 3.860*g/cm3, ncomponents=5);
    TF1->AddMaterial(PbO   , fractionmass=0.512);
    TF1->AddMaterial(SiO2  , fractionmass=0.413);
    TF1->AddMaterial(K2O   , fractionmass=0.035);
    TF1->AddMaterial(Na2O  , fractionmass=0.035);
    TF1->AddMaterial(As2O3 , fractionmass=0.005);

    // Lead Glass TF101 with the chemical composition Maryna gave to FS
    G4Material* TF101 = new G4Material("TF101", density= 3.860*g/cm3, ncomponents=4);
    TF101->AddMaterial(Pb3O4 , fractionmass=0.5123);
    TF101->AddMaterial(SiO2  , fractionmass=0.4157); //here 0.4153 but it do not add up to 1 therefore I just use 0.4157
    TF101->AddMaterial(K2O   , fractionmass=0.07);
    TF101->AddMaterial(Cer   , fractionmass=0.002);

    // PEEK: polyether ether ketone, radiation hard polymer
    G4Material* PEEK = new G4Material("PEEK", density=1320*kg/m3, ncomponents=3);
    PEEK->AddElement(C, natoms=19);
    PEEK->AddElement(H, natoms=14);
    PEEK->AddElement(O, natoms=3);
}
