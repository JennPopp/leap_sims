// Materials.cc
#include "Materials.hh"
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
    G4NistManager* nistManager = G4NistManager::Instance();
    // Define materials here, e.g., G4Material* Si = GetMaterial("G4_Si");
    G4Material* iron = nistManager->FindOrBuildMaterial("G4_Fe");
}
