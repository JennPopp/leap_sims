// BeamLine.cc

#include "BeamLine.hh"
#include "Materials.hh"
#include "ConfigReader.hh"
#include "BaseSensitiveDetector.hh"

#include "AnaConfigManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Exception.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


//The constructor 
BeamLine::BeamLine(const ConfigReader& config, AnaConfigManager& anaConfigManager)
 : fConfig(config), fAnaConfigManager(anaConfigManager) {
    // Read configuration values and initialize the subdetector
    fBeamLineState = config.GetConfigValueAsInt("BeamLine", "beamLineStatus");
    fWorldMaterial = config.GetConfigValue("World", "material");
}

// The destructor 
BeamLine::~BeamLine(){}

//Function to construct the BeamLine mother volume 
G4LogicalVolume* BeamLine::ConstructBeamLine(){
    // when the status is off this function won't be called in the
    // detector construction
    // when the state is two: construct the DESYII test beam setup
    // else construct the setup in 28m 
    if (fBeamLineState == 2) {
        // Felix you could add more parameters to the config file. 
        // like z distance(s) between telescope planes, or z distance
        // from telescope to the calorimeter, if it does not change you might
        // as well hardcode it here 

        // Geometry Parameters-----------------------------------------------
        G4double dist_to_calo = 956*m;
        G4double PlaineHight = 5.00 *cm;
        G4double PlaineThick = 0.5 *mm;
        G4double Nplanes = 6;
        G4double SFthick = 10 *mm ;
        G4double SFhight= 20 *mm;
        G4double SFwide = 50 *mm ; 
        //here I provide the distance of each telescope plane center to world z = 0 (Asuming Calo front at z = 0!!!)
        G4double SFpos = -1887*mm;
        G4double plane0 = -1864 *mm;
        G4double plane1 = -1718 *mm;
        G4double plane2 = -1628 *mm;
        G4double plane3 = -1188 *mm;
        G4double plane4 = -991 *mm;
        G4double plane5 = -964 *mm;

        //----Length of the beamline -> z length of the mother volume---------
        fLengthBL =  -SFpos + SFthick/2.;
        //----Positioning of the telescope planes and the SF (Scintilator Fingers) in the beamline mother 
        G4double planePos[6]={plane0+fLengthBL/2., 
                              plane1+fLengthBL/2.,
                              plane2+fLengthBL/2.,
                              plane3+fLengthBL/2.,
                              plane4+fLengthBL/2.,
                              plane5+fLengthBL/2.};
        G4double SFposBL = SFpos + fLengthBL/2.;
        //materials----------------------------------------------------------
        // make sure that the material you want to used is provided in Materials.cc
        G4Material* PEEK = Materials::GetInstance()->GetMaterial("PEEK");
        G4Material* worldMat = Materials::GetInstance()->GetMaterial(fWorldMaterial);
        G4Material* silicon = Materials::GetInstance()->GetMaterial("G4_Si");
        G4Material* plasticScintillator = Materials::GetInstance()->GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
        
        //---Mother Volume of the BeamLine------------------------------------
        G4Box* solidBLMother = new G4Box("solidBLMother",  //Name
                                            PlaineHight/2.,   // x size
                                            PlaineHight/2.,     // y size
                                            fLengthBL/2.); // z size
        // using the convention of putting an f in front of member variables
        fLogicBLMother = new G4LogicalVolume(solidBLMother,    //its solid
                                          worldMat,    //its material
                                          "logicBLMother");       //its name

        //make virtual mother volume invisible
        fLogicBLMother->SetVisAttributes(G4VisAttributes::GetInvisible());

        //---The telescope Planes----------------------------------------------
        G4Box* solidPlane = new G4Box("solidPlane",  //Name
                                            PlaineHight/2.,   // x size
                                            PlaineHight/2.,     // y size
                                            PlaineThick/2.); // z size

        G4LogicalVolume* logicPlane = new G4LogicalVolume(solidPlane,    //its solid
                                            silicon,    //its material
                                          "logicPlane");       //its name

        for (G4int i=0;i<=Nplanes-1;i++){
            new G4PVPlacement(0,		       //no rotation
                            G4ThreeVector(0,0,planePos[i]),  //its position
                            logicPlane,            //its logical volume
                            "physTelescopePlane",    //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            i,                              //copy number       //copy number
                            true);                     // check overlap    
            }
        //---The telescope Planes----------------------------------------------
        G4Box* solidSF= new G4Box("solidSF",  //Name
                                            SFwide/2.,   // x size
                                            SFhight/2.,     // y size
                                            SFthick/2.); // z size

        G4LogicalVolume* logicSF= new G4LogicalVolume(solidSF,    //its solid
                                            plasticScintillator,    //its material
                                          "logicSF");       //its name   

        new G4PVPlacement(0,		       //no rotation
                        G4ThreeVector(0,0,SFposBL),  //its position
                        logicSF,            //its logical volume
                        "physSF",    //its name
                        fLogicBLMother,               //its mother
                        false,                     //no boolean operat
                        0,                              //copy number  
                        true);                     // check overlap    
    } else {
        // construct the 28m setup

        //---------------------------------------------------------------
        // Geometry Parameters 
        //---------------------------------------------------------------

        //VacuumChamber
        G4double chamberOuterR = 700.0*mm;
        G4double chamberInnerR = 670.0*mm;
        G4double chamberHeight = 660.0*mm;
        G4double chamberThick = 30.0*mm;

        // APL
        G4double radiusAPL = 1.0*mm; 
        G4double lengthAPL = 40.0*mm;
        G4double APLtoWall = 534.0*mm-5.*mm-15*mm;

        //BeamLineTube
        G4double pipeLength = 1739.0*mm;
        G4double pipeInnerR = 18.5*mm;
        G4double pipeOuterR = 19.25*mm;

        // Exit window of vacuum chamber
        G4double windowThick = 0.3*mm;
        G4double winToSol = 500.0*mm; 

        // Copper Collimator
        G4double collimatorLength = 200*mm;
        G4double collimatorWidth = 100*mm;
        G4double collimatorRadius = 0.5*mm;
        G4double collToSol = 205.0*mm;

        //TurboICT
        // Just assume a circular shape here to make my life easier
        G4double TICTinnerRadius = 40.0*mm;
        G4double TICTouterY = 154*mm;
        G4double TICTLength = 32.0*mm;
        G4double TICT2Sol = 168.0*mm;

        // Lanex Screens and Table should be added to Solenoid logical volume
        // and be build when specified in config file 
        
        //.................................
        // Get the Materials
        //.................................
        G4Material* Steel = Materials::GetInstance()->GetMaterial("G4_STAINLESS-STEEL");
        G4Material* Vacuum = Materials::GetInstance()->GetMaterial("Galactic");
        G4Material* worldMat = Materials::GetInstance()->GetMaterial(fWorldMaterial);
        G4Material* Alu = Materials::GetInstance()->GetMaterial("G4_Al");
        G4Material* Copper = Materials::GetInstance()->GetMaterial("G4_Cu");
        G4Material* Argon = Materials::GetInstance()->GetMaterial("G4_Ar");
        G4Material* Lead = Materials::GetInstance()->GetMaterial("G4_Pb");
        
        //.................................
        // Virtual Mother Volume 
        //.................................
        fLengthBL = 2*chamberOuterR + pipeLength + windowThick + winToSol;
         G4Box* solidBLMother = new G4Box("solidBLMother",  //Name
                                            chamberOuterR,   // x size
                                            chamberOuterR,     // y size
                                            fLengthBL/2.); // z size
        // using the convention of putting an f in front of member variables
        fLogicBLMother = new G4LogicalVolume(solidBLMother,    //its solid
                                          worldMat,    //its material
                                          "logicBLMother");       //its name

        //make virtual mother volume invisible
        fLogicBLMother->SetVisAttributes(G4VisAttributes::GetInvisible());

        //.................................
        // Vacuum Chamber
        //.................................
        //----------Chamber Walls---------------------------------------------
        auto solidChamberWalls = new G4Tubs("solidChamberWalls", //name
                              0, // inner radius
                              chamberOuterR ,  // outer radius
                              chamberHeight/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicChamberWalls = new G4LogicalVolume(solidChamberWalls, //its solid
                                      Steel, // its material
                                      "logicChamberWalls" ); // its name

        G4RotationMatrix* chamberRotM = new G4RotationMatrix(0,-90.0*deg,0);
        G4double chamberZ = -fLengthBL/2.+chamberOuterR;
        G4ThreeVector chamberPosition = G4ThreeVector(0.,0.,chamberZ);
        G4Transform3D chamberTransform = G4Transform3D(*chamberRotM, chamberPosition);

        new G4PVPlacement(chamberTransform,             // rcombined rotation and translation
                            logicChamberWalls,            //its logical volume
                            "physChamberWalls",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //----------Chamber Vacuum---------------------------------------------
        auto solidChamberVac = new G4Tubs("solidChamberVac", //name
                              0, // inner radius
                              chamberInnerR ,  // outer radius
                              chamberHeight/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicChamberVac = new G4LogicalVolume(solidChamberVac, //its solid
                                      Vacuum, // its material
                                      "logicChamberVac" ); // its name

        new G4PVPlacement(0,             // no rotation
                            G4ThreeVector(0.,0.,0.),
                            logicChamberVac,            //its logical volume
                            "physChamberVac",                 //its name
                            logicChamberWalls,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //----------Chamber Hole---------------------------------------------
        auto solidChamberHole = new G4Tubs("solidChamberHole", //name
                              0, // inner radius
                              pipeInnerR ,  // outer radius
                              chamberThick/2.+1.5*mm, // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicChamberHole = new G4LogicalVolume(solidChamberHole, //its solid
                                      Vacuum, // its material
                                      "logicChamberHole" ); // its name

        G4RotationMatrix* chamberHoleRotM = new G4RotationMatrix(0,90*deg,0);
        G4ThreeVector chamberHolePosition = G4ThreeVector(0.,chamberOuterR-(chamberThick+3.0*mm)/2,0.);
        G4Transform3D chamberHoleTransform = G4Transform3D(*chamberHoleRotM, chamberHolePosition);

        new G4PVPlacement(chamberHoleTransform,             // rcombined rotation and translation
                            logicChamberHole,            //its logical volume
                            "physChamberHole",                 //its name
                            logicChamberWalls,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //----------Chamber Lid and bottom------------------------------------
        auto solidChamberLid = new G4Tubs("solidChamberLid", //name
                              0, // inner radius
                              chamberOuterR ,  // outer radius
                              chamberThick/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicChamberLid = new G4LogicalVolume(solidChamberLid, //its solid
                                      Steel, // its material
                                      "logicChamberLid" ); // its name

        G4ThreeVector chamberLidPosition = G4ThreeVector(0.,chamberHeight/2.+chamberThick/2.,chamberZ);
        G4Transform3D chamberLidTransform = G4Transform3D(*chamberRotM, chamberLidPosition);
        new G4PVPlacement(chamberLidTransform,             // rcombined rotation and translation
                            logicChamberLid,            //its logical volume
                            "physChamberLid",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        G4ThreeVector chamberBottomPosition = G4ThreeVector(0.,-chamberHeight/2.-chamberThick/2.,chamberZ);
        G4Transform3D chamberBottomTransform = G4Transform3D(*chamberRotM, chamberBottomPosition);
        new G4PVPlacement(chamberBottomTransform,             // combined rotation and translation
                            logicChamberLid,            //its logical volume
                            "physChamberBottom",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            1);                        //copy number
        //.................................
        // APL
        //.................................
        auto solidAPL = new G4Tubs("solidAPL", //name
                              0, // inner radius
                              radiusAPL ,  // outer radius
                              lengthAPL/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicAPL = new G4LogicalVolume(solidAPL, //its solid
                                      Argon, // its material
                                      "logicAPL" ); // its name

        G4RotationMatrix* APLRotM = new G4RotationMatrix(0,90*deg,0);
        G4ThreeVector APLPosition = G4ThreeVector(0.,chamberOuterR-APLtoWall,0.);
        G4Transform3D APLTransform = G4Transform3D(*APLRotM, APLPosition);

        new G4PVPlacement(APLTransform,    //its rotation and position
                            logicAPL,            //its logical volume
                            "physAPL",                 //its name
                            logicChamberVac,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number
                              
        G4VisAttributes * APLVis= new G4VisAttributes( G4Colour(204/255. ,204/255. ,255/255. ));
        APLVis->SetVisibility(true);
        APLVis->SetLineWidth(2);
        APLVis->SetForceSolid(true);
        logicAPL->SetVisAttributes(APLVis);
        //.................................
        // Beam Pipe 
        //.................................
        //---------------------------steel tube---------------------------------
        auto solidPipeTube = new G4Tubs("solidPipeTube", //name
                              0, // inner radius
                              pipeOuterR ,  // outer radius
                              pipeLength/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicPipeTube = new G4LogicalVolume(solidPipeTube, //its solid
                                      Steel, // its material
                                      "logicPipeTube" ); // its name

        G4double pipePos = -fLengthBL/2. + 2*chamberOuterR + pipeLength/2.;
        new G4PVPlacement(0,                   //no rotation
                            G4ThreeVector(0.,0.,pipePos),    //its position
                            logicPipeTube,            //its logical volume
                            "physPipeTube",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //---------------------------vacuum in pipe----------------------------
        auto solidPipeVac = new G4Tubs("solidPipeVac", //name
                              0, // inner radius
                              pipeInnerR,  // outer radius
                              pipeLength/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicPipeVac = new G4LogicalVolume(solidPipeVac, //its solid
                                      Vacuum, // its material
                                      "logicPipeVac" ); // its name

        new G4PVPlacement(0,                   //no rotation
                            G4ThreeVector(0.,0.,0.),    //its position
                            logicPipeVac,            //its logical volume
                            "physPipeVac",                 //its name
                            logicPipeTube,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number
        
        G4VisAttributes * pipeVacVis= new G4VisAttributes( G4Colour(127/255. ,233/255. ,255/255. ));
        pipeVacVis->SetVisibility(true);
        pipeVacVis->SetLineWidth(2);
        pipeVacVis->SetForceSolid(true);
        logicPipeVac->SetVisAttributes(pipeVacVis);
        //---------------------------exit window----------------------------
        auto solidWindow = new G4Tubs("solidWindow", //name
                              0, // inner radius
                              pipeOuterR,  // outer radius
                              windowThick/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicWindow = new G4LogicalVolume(solidWindow, //its solid
                                      Alu, // its material
                                      "logicWindow" ); // its name

        G4double windowPos= fLengthBL/2 - winToSol - windowThick/2.;
        new G4PVPlacement(0,                   //no rotation
                            G4ThreeVector(0.,0.,windowPos),    //its position
                            logicWindow,            //its logical volume
                            "physWindow",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        G4VisAttributes * windowVis= new G4VisAttributes( G4Colour(252/255. ,252/255. ,12/255. ));
        windowVis->SetVisibility(true);
        windowVis->SetLineWidth(2);
        windowVis->SetForceSolid(true);
        logicWindow->SetVisAttributes(windowVis);

        //.................................
        // Copper Collimator
        //.................................
        //---------------------------copper brick-------------------------
        auto solidCollimator = new G4Box("solidCollimator",  //Name
                                collimatorWidth/2.,   // x size
                                collimatorWidth/2.,     // y size
                                collimatorLength/2.); // z size

        auto logicCollimator = new G4LogicalVolume(solidCollimator, //its solid
                                      Copper, // its material
                                      "logicCollimator" ); // its name

        G4double CollPos = fLengthBL/2. - collToSol - collimatorLength/2.;
        new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,CollPos),    //its position
                            logicCollimator,            //its logical volume
                            "physCollimator",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //---------------------------collimator hole------------------------
        auto solidCollHole = new G4Tubs("solidCollHole", //name
                              0, // inner radius
                              collimatorRadius,  // outer radius
                              collimatorLength/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicCollHole = new G4LogicalVolume(solidCollHole, //its solid
                                      worldMat, // its material
                                      "logicCollimatorHole" ); // its name

        new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0.,0.),    //its position
                            logicCollHole,            //its logical volume
                            "physCollimatorHole",                 //its name
                            logicCollimator,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        G4VisAttributes * holeVis= new G4VisAttributes( G4Colour(192/255. ,192/255. ,192/255. ));
        holeVis->SetVisibility(true);
        holeVis->SetLineWidth(2);
        holeVis->SetForceSolid(true);
        logicCollHole->SetVisAttributes(holeVis);

        //.................................
        // Lead Bricks
        //.................................
        G4double brickHeight = collimatorWidth*3/4;
        auto solidLeadBricks = new G4Box("solidLeadBricks",  //Name
                                collimatorWidth,   // half size in x 
                                brickHeight/2,     // half size in y size
                                collimatorLength/2.); // half size in z 

        auto logicLeadBricks = new G4LogicalVolume(solidLeadBricks, //its solid
                                      Lead, // its material
                                      "logicLeadBricks" ); // its name

        new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,-collimatorWidth/2-brickHeight/2.,CollPos),    //its position
                            logicLeadBricks,            //its logical volume
                            "physLeadBricks",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //.................................
        // Turbo ICT
        //.................................
        //---------------------------outer volume-------------------------
        auto solidTICT = new G4Box("solidTICT",  //Name
                                TICTouterY/2.,   // x size
                                TICTouterY/2.,     // y size
                                TICTLength/2.); // z size

        auto logicTICT = new G4LogicalVolume(solidTICT, //its solid
                                      Steel, // its material
                                      "logicTICT" ); // its name

        G4double TICTpos = fLengthBL/2. - TICT2Sol - TICTLength/2.;
        new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0., TICTpos),    //its position
                            logicTICT,            //its logical volume
                            "physTICT",                 //its name
                            fLogicBLMother,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        //---------------------------TICT Hole----------------------------
        auto solidTICTHole = new G4Tubs("solidTICTHole", //name
                              0, // inner radius
                              TICTinnerRadius,  // outer radius
                              TICTLength/2., // half length in z
                              0.0*deg,  // starting angle
                              360.0*deg ); // total angle

        auto logicTICTHole = new G4LogicalVolume(solidTICTHole, //its solid
                                      worldMat, // its material
                                      "logicTICTHole" ); // its name

        new G4PVPlacement(0,                   //no rotation
                    G4ThreeVector(0.,0., 0.),    //its position
                            logicTICTHole,            //its logical volume
                            "physTICTHole",                 //its name
                            logicTICT,               //its mother
                            false,                     //no boolean operat
                            0);                        //copy number

        logicTICTHole->SetVisAttributes(holeVis);

    } // end if fBeamLineState
    // always return the logical beamline mother 
    return fLogicBLMother;
}