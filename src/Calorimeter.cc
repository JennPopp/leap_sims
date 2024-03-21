//Calorimeter.cc 

#include "Calorimeter.hh"
#include "Materials.hh"
#include "ConfigReader.hh"
#include "CaloFrontSensitiveDetector.hh"
#include "CaloCrystalSD.hh"

#include "AnaConfigManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Exception.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


// The constructor: 
Calorimeter::Calorimeter(const ConfigReader& config, AnaConfigManager& anaConfigManager)
  : fConfig(config), fAnaConfigManager(anaConfigManager) {
    // Read configuration values and initialize the subdetector
    fType = config.GetConfigValue("Calorimeter", "type");
    if (fType != "full"){
        fNcrystals = config.GetConfigValueAsInt("Calorimeter", "nCrystals");
    } else {
        fNcrystals = 9;
    }
    fCaloMat = config.GetConfigValue("Calorimeter","caloMaterial");
    fWorldMaterial = config.GetConfigValue("World", "material");
}

//The destructor:
Calorimeter::~Calorimeter(){}

// Function to construct the calo mother volume 
G4LogicalVolume* Calorimeter::ConstructCalo() {
    //---------------------------------------------------------------
    //Geometry Parameters
    //---------------------------------------------------------------
    G4double crystThick = 45.*cm; // length of the lead glass crystals
    G4double crystXY = 3.8*cm; // width in x/y of crystals
    G4double detThick = 1.0*mm; // thickness of virtual detectors 

    G4double alairgapthick = 0.001 *mm;   // thickness of the air gap between
                                          //the aluwrapping and the crystal
    G4double aluwrapthick = 0.01  *mm;   // wikipedia: alu foil thickness
                                         //between 0.004 and 0.02 mm

    G4double alairgapxy = crystXY + 2*alairgapthick;
    G4double alairgaplength = crystThick + alairgapthick;
    G4double aluwrapxy = alairgapxy + 2*aluwrapthick;
    G4double aluwraplength = alairgaplength + aluwrapthick;

    //defining the size of the Calorimetercell and the virtual calorimeter (mother volume of the calorimetercells)
    //G4int NbofCalor = 9; //here later free parameter to select number of crystals
    G4double calorcellxy = aluwrapxy;
    G4double calorcelllength = aluwraplength + 2*detThick;

    G4double virtcalorxy;
    G4double frontPlateZ, frontPlateY,frontPlateX, tbPlateZ;

    if (fType == "full"){
      frontPlateZ=10*mm;
      frontPlateX=160*mm;
      frontPlateY=130*mm;
      tbPlateZ=693*mm;
      fVirtCaloLength=tbPlateZ;
      virtcalorxy = frontPlateX;
    }
    else{
      fVirtCaloLength = calorcelllength;
      if (fNcrystals == 1){
        virtcalorxy = calorcellxy;
      }
      else if (fNcrystals == 9){
        virtcalorxy = 3*calorcellxy;
      }
      else {
        virtcalorxy = calorcellxy;
        G4String description = "You have chosen an invalid number of crystals! N needs to be either 1 or 9.";
        G4Exception("Construct", "InvalidNumberOfCrystals", FatalException, description);
      }
    }

  //---------------------------------------------------------------
  // materials
  //---------------------------------------------------------------
  G4Material* Air       = Materials::GetInstance()->GetMaterial("Air");
  G4Material* Al        = Materials::GetInstance()->GetMaterial("G4_Al");
  G4Material* Vacuum    = Materials::GetInstance()->GetMaterial("Galactic");
  G4Material* PEEK = Materials::GetInstance()->GetMaterial("PEEK");
  G4Material* worldMat = Materials::GetInstance()->GetMaterial(fWorldMaterial);
  G4Material* caloMat = Materials::GetInstance()->GetMaterial(fCaloMat);

  //---------------------------------------------------------------
  // mother volume of the whole calorimeter
  //---------------------------------------------------------------

  G4Box* solidCaloMother = new G4Box("solidCaloMother",  //Name
                                virtcalorxy/2.,   // x size
                                virtcalorxy/2.,     // y size
                                fVirtCaloLength/2.); // z size

  G4LogicalVolume* logicCaloMother = new G4LogicalVolume(solidCaloMother,    //its solid
                                          worldMat,    //its material
                                          "logicCaloMother");       //its name

  //make virtual mother volume invisible
  logicCaloMother->SetVisAttributes(G4VisAttributes::GetInvisible());

  //---------------------------------------------------------------
  // placing the calorimeter cells , those are the virtual mother volumes 
  // where crystals, wrapping, air gap, sensitive detector volumes  etc 
  // are placed in 
  //---------------------------------------------------------------

  G4Box* solidCaloCell = new G4Box("solidCaloCell",  //Name
                                    calorcellxy/2.,   // x size
                                    calorcellxy/2.,     // y size
                                    calorcelllength/2.); // z size


  G4LogicalVolume* logicCaloCell = new G4LogicalVolume(solidCaloCell,    //its solid
                                          worldMat,    //its material
                                          "logicCaloCell");       //its name

  G4double calocellZpos;
  
  if(fType=="full"){calocellZpos=-tbPlateZ/2+frontPlateZ+9*mm+calorcelllength/2+1*mm;;}
  else{calocellZpos=0;}

  if(fNcrystals==9){
    //the array for the placement of the 9 calorimetercells in the virtual calorimeter
    G4double CalorRX[9]={-calorcellxy, 0, calorcellxy,-calorcellxy, 0, calorcellxy, -calorcellxy, 0, calorcellxy};
    G4double CalorRY[9]={calorcellxy,calorcellxy,calorcellxy, 0,0,0,-calorcellxy,-calorcellxy,-calorcellxy};

    for (G4int i=0;i<=8;i++){
      new G4PVPlacement(0,		       //no rotation
                        G4ThreeVector(CalorRX[i],CalorRY[i],calocellZpos),  //its position
                        logicCaloCell,            //its logical volume
                        "physCaloCell",    //its name
                        logicCaloMother,               //its mother
                        false,                     //no boolean operat
                        i,                              //copy number       //copy number
                        true);                     // check overlap    
      }
  } else if (fNcrystals == 1){
    new G4PVPlacement(0,		       //no rotation
                      G4ThreeVector(0,0,calocellZpos),  //its position
                      logicCaloCell,            //its logical volume
                      "physCaloCell",    //its name
                      logicCaloMother,               //its mother
                      false,                     //no boolean operat
                      0);                        //copy number       //copy number
  }

  logicCaloCell->SetVisAttributes(G4VisAttributes::GetInvisible());

  //---------------------------------------------------------------
  // reflective aluminium wrapping  
  //---------------------------------------------------------------

  auto solidAluWrap= new G4Box("solidAluWrapping",  //Name
                                  aluwrapxy/2.,   // x size
                                  aluwrapxy/2.,     // y size
                                  aluwraplength/2.); // z size


  auto logicAluWrap = new G4LogicalVolume(solidAluWrap,    //its solid
                                          Al,    //its material
                                          "logicAluWrapping");       //its name

  new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,detThick/2),    //its position // old 0.,0.,-vacthick/2
                            logicAluWrap,            //its logical volume
                            "physAluWrapping",                 //its name
                            logicCaloCell,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

  G4VisAttributes * AluVis= new G4VisAttributes( G4Colour(119/255. ,136/255. ,153/255. ));
  AluVis->SetVisibility(true);
  AluVis->SetLineWidth(2);
  AluVis->SetForceSolid(false);
  logicAluWrap->SetVisAttributes(AluVis);

  //---------------------------------------------------------------
  // air gap between aluminium and crystal 
  //---------------------------------------------------------------
  auto solidAirGap = new G4Box("solidAirGap",  //Name
                                alairgapxy/2.,   // x size
                                alairgapxy/2.,     // y size
                                alairgaplength/2.); // z size


  auto logicAirGap = new G4LogicalVolume(solidAirGap,    //its solid
                                        Air,    //its material
                                        "logicAirGap");       //its name

  new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,-(aluwrapthick)/2),    //its position // old 0.,0.,aluwrapthick/2
                    logicAirGap,            //its logical volume
                    "AlAirGap",                 //its name
                    logicAluWrap,               //its mother
                    false,                     //no boolean operat
                    0);                        //copy number

  //---------------------------------------------------------------
  // crystal, here lead glass   
  //---------------------------------------------------------------
  auto solidCrystal = new G4Box("solidCrystal",  //Name
                                 crystXY/2.,   // x size
                                 crystXY/2.,     // y size
                                 crystThick/2.); // z size

  fLogicCrystal = new G4LogicalVolume(solidCrystal,    //its solid
                                        caloMat,    //its material
                                        "logicCrystal");       //its name

  new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,alairgapthick/2),    //its position
                    fLogicCrystal,            //its logical volume
                    "solidCrystal",                 //its name
                    logicAirGap,               //its mother
                    false,                     //no boolean operat
                    0);                        //copy number

  G4VisAttributes * CrystalVis= new G4VisAttributes( G4Colour(224/255. ,255/255. ,255/255. ));
  CrystalVis->SetVisibility(true);
  CrystalVis->SetLineWidth(2);
  CrystalVis->SetForceSolid(true);
  fLogicCrystal->SetVisAttributes(CrystalVis);

  //---------------------------------------------------------------
  // virtual detectors in front of and behind the crystals    
  //---------------------------------------------------------------

  G4Box* solidVacStep = new G4Box("solidVacStepCalo",  //Name
                                  crystXY/2.,
                                  crystXY/2,
                                  detThick/2.);

  fLogicFrontDet = new G4LogicalVolume(solidVacStep,    //its solid
                                        Vacuum,    //its material
                                        "logicCaloFrontDet");       //its name

  new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,-(aluwraplength/2+detThick/2.)),    //its position 
                    fLogicFrontDet,            //its logical volume
                    "physCaloFrontDet",                 //its name
                    logicCaloCell,               //its mother //old fCaloCellLV
                    false,                     //no boolean operat
                    0,                         //copy number
                    true);                     // check for overlaps    

  fLogicBackDet = new G4LogicalVolume(solidVacStep,    //its solid
                                          Vacuum,    //its material
                                          "VacStep4");       //its name

   new G4PVPlacement(0,                   //no rotation
                        G4ThreeVector(0.,0.,(aluwrapthick+alairgaplength)/2),    //its position 
                        fLogicBackDet,            //its logical volume
                        "VacStep4",                 //its name
                        logicAluWrap,               //its mother //old fCaloCellLV
                        false,                     //no boolean operat
                        0);                        //copy number

  //---------------------------------------------------------------
  // If the full calorimeter is simulated -> also build housing    
  //---------------------------------------------------------------
  if(fType=="full"){
    //
    // Plastic front plate with holes in it --------------------------
    //

    auto FrontPlateBox = new G4Box("FrontPlateBox",  //Name
                                  frontPlateX/2.,
                                  frontPlateY/2,
                                  frontPlateZ/2.);

    auto FrontPlateLV = new G4LogicalVolume(FrontPlateBox,    //its solid
                                        PEEK,    //its material
                                        "FrontPlateLV");       //its name

    G4double FrontPlateZPos = -tbPlateZ/2+frontPlateZ/2+9*mm;

    new G4PVPlacement(0,                   //no rotation
                      G4ThreeVector(0.,0.,FrontPlateZPos),    //its position //old 0.,0.,aluwraplength/2
                              FrontPlateLV,            //its logical volume
                              "FrontPlate",                 //its name
                              logicCaloMother,               //its mother
                              false,                     //no boolean operat
                              0);                        //copy number

    G4VisAttributes* FrontPlateVis = new G4VisAttributes(G4Colour(0,128/255,1));
    FrontPlateVis->SetForceSolid(true);
    FrontPlateLV->SetVisAttributes(FrontPlateVis);

    auto FrontPlaneHoleTub = new G4Tubs("FrontPlaneHoleTub",
                                      0.0*mm, // inner radius
                                      18.5*mm ,  // outer radius
                                      frontPlateZ/2., // half length in z
                                      0.0*deg,  // starting angle
                                      360.0*deg ); // total angle

    auto FrontPlaneHoleLV = new G4LogicalVolume(FrontPlaneHoleTub,    //its solid
                                        worldMat,    //its material
                                        "FrontPlateHoleLV");       //its name

    G4double FrontHolDist = 39.0*mm ;
    G4double FrontHolPosX[9]={-FrontHolDist, 0, FrontHolDist,-FrontHolDist, 0, FrontHolDist, -FrontHolDist, 0, FrontHolDist};
    G4double FrontHolPosY[9]={FrontHolDist,FrontHolDist,FrontHolDist, 0,0,0,-FrontHolDist,-FrontHolDist,-FrontHolDist};

    for (G4int i=0;i<=8;i++){
      new G4PVPlacement(0,		       //no rotation
                G4ThreeVector(FrontHolPosX[i],FrontHolPosY[i],0),  //its position
                FrontPlaneHoleLV,            //its logical volume
                "FrontPlaneHole",    //its name
                FrontPlateLV,               //its mother
                false,                     //no boolean operat
                i);                        //copy number       //copy number
    }

    //
    // Top and bottom aluminium plates -----------------------------------------------------------
    //
    G4double tbPlateY= 20*mm;
    auto TBPlateBox = new G4Box("TBPlateBox",  //Name
                                frontPlateX/2.,
                                tbPlateY/2,
                                tbPlateZ/2.);

    auto FrontPlateCut = new G4Box("TBPlateBox",  //Name
                                frontPlateX+2/2.,
                                3*mm,
                                frontPlateZ/2.);

    G4ThreeVector FPCutTrans(0, -tbPlateY/2+2*mm, FrontPlateZPos);

    G4SubtractionSolid* TBPlateSolid = new G4SubtractionSolid("TBPlateSolid", // Name
                                                                TBPlateBox, // 1. Volume
                                                                FrontPlateCut, // 2. Volume
                                                                0, // rotation
                                                                FPCutTrans // translation
                                                                );

    auto TBPlateLV = new G4LogicalVolume(TBPlateSolid,    //its solid
                                        Al,    //its material
                                        "TBPlateLV");       //its name

    new G4PVPlacement(0,                   //no rotation
                      G4ThreeVector(0.,frontPlateX/2-tbPlateY/2.,0.),    //its position //
                              TBPlateLV,            //its logical volume
                              "TopPlate",                 //its name
                              logicCaloMother,               //its mother
                              false,                     //no boolean operat
                              0);                        //copy number

    G4RotationMatrix* zRot = new G4RotationMatrix;  // Rotates X and y axes only
    zRot->rotateZ(M_PI*rad);

    new G4PVPlacement(zRot,                   //no rotation
                      G4ThreeVector(0.,-frontPlateX/2+tbPlateY/2.,0.),    //its position //
                              TBPlateLV,            //its logical volume
                              "TopPlate",                 //its name
                              logicCaloMother,               //its mother
                              false,                     //no boolean operat
                              1);                        //copy number
  
  }

return logicCaloMother;
} 

void Calorimeter::ConstructCalorimeterSD(){
  if (fConfig.GetConfigValueAsInt("Calorimeter","frontDetector")){
    std::string ntupleName = "inFrontCalo";
    auto& mapping = fAnaConfigManager.GetNtupleNameToIdMap();
    auto it = mapping.find(ntupleName);
      if (it != mapping.end()) {
          int ID = it->second;
          G4cout << "--------0000000000000----------- ::: THE TUPLE ID IS:" << ID << G4endl;
          auto sdIC = new CaloFrontSensitiveDetector(ntupleName, ntupleName, ID, fAnaConfigManager);
          G4SDManager::GetSDMpointer()->AddNewDetector(sdIC);
          // Retrieve the logical volume for this layer and set its SD
          fLogicFrontDet->SetSensitiveDetector(sdIC );
      }
  }
  if (fConfig.GetConfigValueAsInt("Calorimeter","crystDetector")){
    std::string ntupleName = "CaloCrystal";
    auto& mapping = fAnaConfigManager.GetNtupleNameToIdMap();
    auto it = mapping.find(ntupleName);
      if (it != mapping.end()) {
          int ID = it->second;
          G4cout << "--------0000000000000----------- ::: THE TUPLE ID IS:" << ID << G4endl;
          auto sdCC = new CaloCrystalSD(ntupleName, ntupleName, ID, fAnaConfigManager);
          G4SDManager::GetSDMpointer()->AddNewDetector(sdCC);
          // Retrieve the logical volume for this layer and set its SD
          fLogicCrystal->SetSensitiveDetector(sdCC );
      }
  }
}