// Solenoid.cc
#include "Solenoid.hh"
#include "Materials.hh"
#include "ConfigReader.hh"

#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Exception.hh"

#include "G4PolarizationManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

Solenoid::Solenoid(const ConfigReader& config) : G4VUserDetectorConstruction() {
    // Read configuration values and initialize the subdetector
    fCoreRad = config.GetConfigValueAsDouble("Solenoid", "coreRad")*mm;
    fCoreLength = config.GetConfigValueAsDouble("Solenoid", "coreLength")*mm;
    fConvThick = config.GetConfigValueAsDouble("Solenoid", "convThick")*mm;
    fPolStatus = config.GetConfigValue("PhysicsList", "polarizationStatus");
    //fBeamLineStatus = config.GetConfigValue("BeamLine", "beamLineStatus");
    fType = config.GetConfigValue("Solenoid", "type");
    fWorldMaterial = config.GetConfigValue("World", "material");
    fPolDeg = config.GetConfigValueAsDouble("GPS","polDeg");
}

Solenoid::~Solenoid() {}

G4VPhysicalVolume* Solenoid::Construct() {

    //---------------------------------------------------------------
    //Geometry Parameters
    //---------------------------------------------------------------
    G4double shieldRad = 75.0*mm; 
    G4double coneLength = 50.0*mm;
    G4double coilThick = fCoreLength + 25.0*mm;
    G4double shieldThick = fCoreLength - 25*mm;
    G4double coreGap = 12.5*mm; // TP2: dist core and cone TP1: dist core and conv
    
    G4double rMax, rOpen, rOuterCoil, coneDist, magThick;
    if (fType == "TP2"){
      rMax = 162.0*mm;
      rOpen = 30.0*mm;
      rOuterCoil = 161*mm;
      coneDist = fCoreLength/2. + coreGap;
      magThick = 2.*(coneLength+coreGap)+fCoreLength;
    } else if (fType == "TP1"){
      rMax = 196.0*mm;
      rOpen = 36.84308*mm;
      rOuterCoil = 170*mm;
      coneDist = fCoreLength/2. + coreGap + fConvThick;
      magThick = 2.*(coneLength+coreGap+fConvThick)+fCoreLength;
    } else {
        G4String description = "You have chosen an invalid solenoid Type in the config.ini. The only valid types are TP1 and TP2!";
        G4Exception("Construct", "InvalidSolenoidType", FatalException, description);
    }

    //---------------------------------------------------------------
    // materials
    //---------------------------------------------------------------

    G4Material* iron = Materials::GetInstance()->GetMaterial("G4_Fe");
    G4Material* lead = Materials::GetInstance()->GetMaterial("G4_Pb");
    G4Material* copper = Materials::GetInstance()->GetMaterial("G4_Cu");
    G4Material* tungsten = Materials::GetInstance()->GetMaterial("G4_W");
    G4Material* worldMat = Materials::GetInstance()->GetMaterial(fWorldMaterial);
    
    //---------------------------------------------------------------
    // mother volume of the solenoid  
    //---------------------------------------------------------------
    G4Tubs* solidSolenoid = new G4Tubs("solidSolenoid",
                                        0.0*mm, // inner radius
                                        rMax+1.0*mm,  // outer radius
                                        magThick/2., // half length in z
                                        0.0*deg,  // starting angle
                                        360.0*deg ); // total angle

    G4LogicalVolume * logicSolenoid = new G4LogicalVolume(solidSolenoid, //its solid
                                                        worldMat, 	 //its material
                                                        "motherSolenoid" , //its name
                                                        0,0,0);
    
    G4VPhysicalVolume* physSolenoid = new G4PVPlacement(0,	//rotation
                      G4ThreeVector(0.0*mm, 0.0*mm, 0.0*mm),// translation position
                      logicSolenoid,      //its logical volume
                      "PhysicalSolenoid",   //its name  (2nd constructor)
                      0,         //its mother volume
                      false,              //no boolean operation
                      0);                 //copy number

    logicSolenoid->SetVisAttributes(G4VisAttributes::GetInvisible());
    //---------------------------------------------------------------
    // housing of the solenoid
    //---------------------------------------------------------------
    G4double DzArrayMagnet   [] = {-magThick/2., -coneDist, -fCoreLength/2., fCoreLength/2.,  coneDist, magThick/2.};
    G4double RminArrayMagnet [] = {rOpen,  fCoreRad,  fCoreRad, fCoreRad,  fCoreRad,  rOpen};
    G4double RmaxArrayMagnet [] = {rMax, rMax, rMax, rMax, rMax, rMax};
    
    G4Polycone* solidMagnet = new G4Polycone("solidMagnet", 	 //its name
              0.0*deg, 		 //its start angle
              360.0*deg,		 //its opening angle
              6, 		         //its nZ
              DzArrayMagnet, 	 //z value
              RminArrayMagnet, 	 //rmin
              RmaxArrayMagnet ); 	 //rmax

    G4LogicalVolume* logicMagnet = new G4LogicalVolume(solidMagnet, //its solid
                     iron, 	 //its material
                     "Magnet" ,		 //its name
                     0,0,0);

    new G4PVPlacement(0,	//rotation
                      G4ThreeVector(0.0*mm, 0.0*mm, 0.0*mm),// translation position
                      logicMagnet,      //its logical volume
                      "PhysicalMagnet",   //its name  (2nd constructor)
                      logicSolenoid,         //its mother volume
                      false,              //no boolean operation
                      0);                 //copy number

    G4VisAttributes * MagnetVis= new G4VisAttributes( G4Colour(255/255. ,102/255. ,102/255. ));
    MagnetVis->SetVisibility(true);
    MagnetVis->SetLineWidth(1);
    logicMagnet->SetVisAttributes(MagnetVis);
    //---------------------------------------------------------------
    // copper coils 
    //---------------------------------------------------------------
    G4Tubs* solidCuTube= new G4Tubs("solidCuTube", //name
                                    shieldRad, // inner radius
                                    rOuterCoil,  // outer radius
                                    coilThick/2., // half length in z
                                    0.0*deg,  // starting angle
                                    360.0*deg ); // total angle

    G4LogicalVolume* logicCuTube = new G4LogicalVolume(solidCuTube, //its solid
               copper,              //its material
               "CuTube" ,		         //its name
               0,0,0);

     new G4PVPlacement(0,	//rotation
           G4ThreeVector(0.0*mm, 0.0*mm, 0.0*mm), // translation position
             logicCuTube,      // its logical volume
             "PhysicalCuTube",   // its name  (2nd constructor)
             logicMagnet,     // its mother volume
             false,              // no boolean operation
             0);
    G4VisAttributes * CopperCoilVis= new G4VisAttributes( G4Colour(255/255. ,0/255. ,255/255. ));
    CopperCoilVis->SetVisibility(true);
    CopperCoilVis->SetLineWidth(1);
    logicCuTube->SetVisAttributes(CopperCoilVis);
    //---------------------------------------------------------------
    // lead shielding 
    //---------------------------------------------------------------

    G4Tubs* solidPbTube= new G4Tubs("solidPbTube", // name
                                    fCoreRad, // inner radius
                                    shieldRad, // outer radius
                                    shieldThick/2., // half length in z
                                    0.0*deg, // start angle
                                    360.0*deg ); // total angle

    G4LogicalVolume* logicPbTube = new G4LogicalVolume(solidPbTube, 	 //its solid
    						lead, 		 //its material
    						"PbTube" ,		 //its name
    						0,0,0);

    new G4PVPlacement(0,	//rotation
    				G4ThreeVector(0.0*mm, 0.0*mm, 0.0*mm),
    				logicPbTube,      //its logical volume
    			    "PhysicalPbTube",   //its name  (2nd constructor)
    			    logicMagnet,     //its mother volume
    			    false,              //no boolean operation
    			    0);                 //copy number

    G4VisAttributes * LeadTubeVis= new G4VisAttributes( G4Colour(0/255. ,102/255. ,204/255. ));
    LeadTubeVis->SetVisibility(true);
    LeadTubeVis->SetLineWidth(1);
    logicPbTube->SetVisAttributes(LeadTubeVis);

    //---------------------------------------------------------------
    // conversion target
    //---------------------------------------------------------------
    if (fConvThick > 0){
        G4Tubs* solidConversion = new G4Tubs("solidConversion", // name
                                          0.0*mm, // inner radius
                                          fCoreRad, // outer radius
                                          fConvThick/2., // half length in z
                                          0.0*deg, // starting angle
                                          360.0*deg ); // total angle

      G4LogicalVolume* logicConversion = new G4LogicalVolume(solidConversion, 	 //its solid
                  tungsten,          //its material
                  "ConversionTarget" ,	 //its name
                  0,0,0);

      new G4PVPlacement(0,	//rotation
                  G4ThreeVector(0.0*mm, 0.0*mm, -coreGap-fConvThick/2-fCoreLength/2),
                logicConversion,         //its logical volume
                "PhysicalConversion",   //its name  (2nd constructor)
                logicSolenoid,              //its mother volume
                false,                 //no boolean operation
                0);                       //copy number

      G4VisAttributes * ConversionTargetVis= new G4VisAttributes( G4Colour(105/255. ,105/255. ,105/255. ));
      ConversionTargetVis->SetVisibility(true);
      ConversionTargetVis->SetLineWidth(2);
      ConversionTargetVis->SetForceSolid(true);
      logicConversion->SetVisAttributes(ConversionTargetVis);
    }
    //---------------------------------------------------------------
    // iron core 
    //---------------------------------------------------------------

    G4Tubs* solidCore = new G4Tubs("solidCore", // name of the solid 
                                    0.0*mm, // inner radius 
                                    fCoreRad, // outer radius 
                                    fCoreLength/2, // half length  
                                    0.0*deg, //starting angle 
                                    360.0*deg );// end angle 

    G4LogicalVolume* logicCore = new G4LogicalVolume(solidCore, // solid
                                                     iron, // material 
                                                    "logicCore"); // name of logical volume 

    new G4PVPlacement(0, // no rotation
                      G4ThreeVector(), // (0,0,0)
                      logicCore, // its logical volume 
                      "physBox", // name of physical volume
                      logicSolenoid, // its mother volume 
                      false, // no boolean operation
                      0); // copy number 

    if (fPolStatus == "On"){
        // register logical Volume in PolarizationManager with polarization
        G4PolarizationManager * polMgr = G4PolarizationManager::GetInstance();
        polMgr->SetVolumePolarization(logicCore,G4ThreeVector(0.,0.,fPolDeg));
    }
    else{
        G4cout << "YOU ARE NOT USING POLARIZATION !!!!!" << G4endl;
    }
    G4VisAttributes * IronCoreVis= new G4VisAttributes( G4Colour(51/255. ,51/255. ,255/255. ));
    IronCoreVis->SetVisibility(true);
    IronCoreVis->SetLineWidth(2);
    IronCoreVis->SetForceSolid(true);
    logicCore->SetVisAttributes(IronCoreVis);
    return physSolenoid;
}