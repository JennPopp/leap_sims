// Materials.hh
 
#ifndef Materials_h
#define Materials_h 1

#include "G4Material.hh"

class Materials {
public:
    static Materials* GetInstance();
    G4Material* GetMaterial(const G4String& materialName);

private:
    Materials();
    static Materials* fInstance;
    void DefineMaterials();
};

#endif