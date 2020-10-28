//
/// \brief Implementation of the StackingAction class
//

#include "StackingAction.hh"

#include "Run.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "StackingMessenger.hh"

#include "G4RunManager.hh"
#include "G4Track.hh"


StackingAction::StackingAction(EventAction* EA)
 : G4UserStackingAction(), fEventAction(EA),
   fKillSecondary(0),fStackMessenger(0),fPhotoGamma(-1),fComptGamma(-1),
   fPhotoAuger(-1),fComptAuger(-1),fPixeGamma(-1),fPixeAuger(-1),
   fIDdefined(false)
{
//test   fStackMessenger = new StackingMessenger(this);
}


StackingAction::~StackingAction()
{
  delete fStackMessenger;
}


G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  //keep primary particle
  if (aTrack->GetParentID() == 0) { return fUrgent; }

  if(!fIDdefined) {
    fIDdefined = true;
    fPhotoGamma = G4PhysicsModelCatalog::GetIndex("phot_fluo");
    fComptGamma = G4PhysicsModelCatalog::GetIndex("compt_fluo");
    fPhotoAuger = G4PhysicsModelCatalog::GetIndex("phot_auger");
    fComptAuger = G4PhysicsModelCatalog::GetIndex("compt_auger");
    fPixeGamma = G4PhysicsModelCatalog::GetIndex("gammaPIXE");
    fPixeAuger = G4PhysicsModelCatalog::GetIndex("e-PIXE");
  }
  G4int idx = aTrack->GetCreatorModelID();

  //count secondary particles
    
  Run* run = static_cast<Run*>(
             G4RunManager::GetRunManager()->GetNonConstCurrentRun()); 
  run->CountParticles(aTrack->GetDefinition());
  /*
  G4cout << "###StackingAction: new " 
         << aTrack->GetDefinition()->GetParticleName()
         << " E(MeV)= " << aTrack->GetKineticEnergy()
         << "  " << aTrack->GetMomentumDirection() << G4endl;
  */
  //
  //energy spectrum of secondaries
  //
  G4double energy = aTrack->GetKineticEnergy();
  G4double charge = aTrack->GetDefinition()->GetPDGCharge();

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  if (charge != 0.) {
    analysisManager->FillH1(2,energy);
    analysisManager->FillH1(4,energy);
    if(idx == fPhotoAuger || idx == fComptAuger) {
      analysisManager->FillH1(16,energy);
      analysisManager->FillH1(18,energy);
    } else if(idx == fPixeAuger) {
      analysisManager->FillH1(44,energy);
      analysisManager->FillH1(46,energy);
    }
  }

  if (aTrack->GetDefinition() == G4Gamma::Gamma()) {
    analysisManager->FillH1(3,energy);
    analysisManager->FillH1(5,energy);
    if(idx == fPhotoGamma || idx == fComptGamma) {
      analysisManager->FillH1(17,energy);
      analysisManager->FillH1(19,energy);
    } else if(idx == fPixeGamma) {
      analysisManager->FillH1(45,energy);
      analysisManager->FillH1(47,energy);
    }
  }  

  //stack or delete secondaries
  G4ClassificationOfNewTrack status = fUrgent;
  if (fKillSecondary) {
    if (fKillSecondary == 1) {
     fEventAction->AddEnergy(energy);
     status = fKill;
    }  
    if (aTrack->GetDefinition() == G4Gamma::Gamma()) {
      status = fKill;
    }
  }
    
  return status;
}


