//
/// \brief Implementation of the PrimaryGeneratorMessenger class
//

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"



PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
                                             PrimaryGeneratorAction* Gun)
:G4UImessenger(),fAction(Gun),
 fGunDir(0),      
 fDefaultCmd(0),
 fBeamTypeCmd(0),
 fBeamSigmaXCmd(0),
 fBeamSigmaYCmd(0)
{
  fGunDir = new G4UIdirectory("/lxphoton/gun/");
  fGunDir->SetGuidance("gun control");
 
  fDefaultCmd = new G4UIcmdWithoutParameter("/lxphoton/gun/setDefault",this);
  fDefaultCmd->SetGuidance("set/reset the kinematic defined in PrimaryGenerator");
  fDefaultCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  fBeamTypeCmd = new G4UIcmdWithAString("/lxphoton/gun/beamType",this);
  fBeamTypeCmd->SetGuidance("Distribution of primary particles of the beam. Supported types: gaussian, mono");
  fBeamTypeCmd->SetParameterName("beamType",false);
  fBeamTypeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);   
 
  fSpectraFileCmd = new G4UIcmdWithAString("/lxphoton/gun/SpectraFile",this);
  fSpectraFileCmd->SetGuidance("File contaning tabulated PDF of primary particles of the beam.");
  fSpectraFileCmd->SetParameterName("SpectraFile",false);
  fSpectraFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);  

  fBeamSigmaXCmd = new G4UIcmdWithADoubleAndUnit("/lxphoton/gun/setSigmaX",this);
  fBeamSigmaXCmd->SetGuidance("Set sigma for beam distribution in X direction at IP");
  fBeamSigmaXCmd->SetParameterName("SigmaX",false);  
  fBeamSigmaXCmd->SetRange("SigmaX>0.");
  fBeamSigmaXCmd->SetUnitCategory("Length");  
  fBeamSigmaXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
 
  fBeamSigmaYCmd = new G4UIcmdWithADoubleAndUnit("/lxphoton/gun/setSigmaY",this);
  fBeamSigmaYCmd->SetGuidance("Set sigma for beam distribution in Y direction at IP");
  fBeamSigmaYCmd->SetParameterName("SigmaY",false);  
  fBeamSigmaYCmd->SetRange("SigmaY>0.");
  fBeamSigmaYCmd->SetUnitCategory("Length");  
  fBeamSigmaYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  fBeamFocusCmd = new G4UIcmdWithADoubleAndUnit("/lxphoton/gun/setFocus",this);
  fBeamFocusCmd->SetGuidance("Set distance between the target and beam focus (usually IP)");
  fBeamFocusCmd->SetParameterName("BeamFocus",false);  
  fBeamFocusCmd->SetUnitCategory("Length");  
  fBeamFocusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}



PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fDefaultCmd;
  delete fBeamTypeCmd;
  delete fSpectraFileCmd;
  delete fGunDir; 
  delete fBeamSigmaXCmd;
  delete fBeamSigmaYCmd;
  delete fBeamFocusCmd;
}



void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                               G4String newValue)
{ 
  if (command == fDefaultCmd)
    {fAction->SetDefaultKinematic();}
   
  if (command == fBeamTypeCmd)
   { fAction->SetBeamType(newValue);}       

  if (command == fSpectraFileCmd)
   { fAction->SetSpectraFile(newValue);}       

  if (command == fBeamSigmaXCmd)
   { fAction->SetSigmaX(fBeamSigmaXCmd->GetNewDoubleValue(newValue));}       

  if (command == fBeamSigmaYCmd)
   { fAction->SetSigmaY(fBeamSigmaYCmd->GetNewDoubleValue(newValue));}       
    
  if (command == fBeamFocusCmd)
   { fAction->SetBeamFocus(fBeamFocusCmd->GetNewDoubleValue(newValue));}       
}



