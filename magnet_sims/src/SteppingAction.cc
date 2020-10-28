//
/// \brief Implementation of the SteppingAction class
//



#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "G4VUserTrackInformation.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "PrimaryGeneratorAction.hh"


SteppingAction::SteppingAction(DetectorConstruction* DET,
                               EventAction* EA)
:G4UserSteppingAction(),fDetector(DET), fEventAction(EA)
{ }



SteppingAction::~SteppingAction()
{ }



void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
 if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()
     == fDetector->GetAbsorber() ){

 fEventAction->AddEnergy (aStep->GetTotalEnergyDeposit());

 G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
   if (charge != 0.) {
     fEventAction->AddTrakLenCharg(aStep->GetStepLength());
     fEventAction->CountStepsCharg();
   } else {
     fEventAction->AddTrakLenNeutr(aStep->GetStepLength());
     fEventAction->CountStepsNeutr();
   }
  }
 // get volume of the current step
   auto volume = aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
//G4cout<< " This part of the code you are currently testing is executed"  << G4endl;
//if the volume of the current step is in the vacstep->GetMethods
 if ( volume == fDetector->GetVacStep1PV() ) {
     // get analysis manager
     auto analysisManager = G4AnalysisManager::Instance();

     // fill ntuple id=0
     analysisManager->FillNtupleIColumn(0,0, aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
     analysisManager->FillNtupleIColumn(0,1, aStep->GetTrack()->GetParticleDefinition()->GetPDGCharge());

     analysisManager->FillNtupleDColumn(0,2, aStep->GetPostStepPoint()->GetKineticEnergy()/MeV);

     analysisManager->FillNtupleDColumn(0,3, aStep->GetPostStepPoint()->GetPosition().x());
     analysisManager->FillNtupleDColumn(0,4, aStep->GetPostStepPoint()->GetPosition().y());
     analysisManager->FillNtupleDColumn(0,5, aStep->GetPostStepPoint()->GetPosition().z());

     analysisManager->FillNtupleDColumn(0,6, aStep->GetTrack()->GetVertexPosition().x());
     analysisManager->FillNtupleDColumn(0,7, aStep->GetTrack()->GetVertexPosition().y());
     analysisManager->FillNtupleDColumn(0,8, aStep->GetTrack()->GetVertexPosition().z());

     analysisManager->FillNtupleDColumn(0,9, aStep->GetPostStepPoint()->GetMomentumDirection().x());
     analysisManager->FillNtupleDColumn(0,10, aStep->GetPostStepPoint()->GetMomentumDirection().y());
     analysisManager->FillNtupleDColumn(0,11, aStep->GetPostStepPoint()->GetMomentumDirection().z());

     analysisManager->FillNtupleDColumn(0,12, aStep->GetTrack()->GetPolarization().x());
     analysisManager->FillNtupleDColumn(0,13, aStep->GetTrack()->GetPolarization().y());
     analysisManager->FillNtupleDColumn(0,14, aStep->GetTrack()->GetPolarization().z());

     analysisManager->FillNtupleDColumn(0,15, aStep->GetTrack()->GetTrackID());
     analysisManager->FillNtupleDColumn(0,16, aStep->GetTrack()->GetParentID());
     analysisManager->FillNtupleDColumn(0,17, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

     analysisManager->AddNtupleRow(0);

//G4cout<< " This part of the code you are currently testing is executed"  << G4endl;
   }

   if ( volume == fDetector->GetVacStep2PV() ) {

        auto analysisManager = G4AnalysisManager::Instance();
       //~//~//~//
       analysisManager->FillNtupleIColumn(1,0, aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
       analysisManager->FillNtupleIColumn(1,1, aStep->GetTrack()->GetParticleDefinition()->GetPDGCharge());

       analysisManager->FillNtupleDColumn(1,2, aStep->GetPostStepPoint()->GetKineticEnergy()/MeV);

       analysisManager->FillNtupleDColumn(1,3, aStep->GetPostStepPoint()->GetPosition().x());
       analysisManager->FillNtupleDColumn(1,4, aStep->GetPostStepPoint()->GetPosition().y());
       analysisManager->FillNtupleDColumn(1,5, aStep->GetPostStepPoint()->GetPosition().z());

       analysisManager->FillNtupleDColumn(1,6, aStep->GetTrack()->GetVertexPosition().x());
       analysisManager->FillNtupleDColumn(1,7, aStep->GetTrack()->GetVertexPosition().y());
       analysisManager->FillNtupleDColumn(1,8, aStep->GetTrack()->GetVertexPosition().z());

       analysisManager->FillNtupleDColumn(1,9, aStep->GetPostStepPoint()->GetMomentumDirection().x());
       analysisManager->FillNtupleDColumn(1,10, aStep->GetPostStepPoint()->GetMomentumDirection().y());
       analysisManager->FillNtupleDColumn(1,11, aStep->GetPostStepPoint()->GetMomentumDirection().z());

       analysisManager->FillNtupleDColumn(1,12, aStep->GetTrack()->GetPolarization().x());
       analysisManager->FillNtupleDColumn(1,13, aStep->GetTrack()->GetPolarization().y());
       analysisManager->FillNtupleDColumn(1,14, aStep->GetTrack()->GetPolarization().z());

       analysisManager->FillNtupleDColumn(1,15, aStep->GetTrack()->GetTrackID());
       analysisManager->FillNtupleDColumn(1,16, aStep->GetTrack()->GetParentID());
       analysisManager->FillNtupleDColumn(1,17, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

       analysisManager->AddNtupleRow(1);


  //G4cout<< " This part of the code you are currently testing is executed"  << G4endl;
     }



}
