//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(
                      const DetectorConstruction* detectorConstruction,
                      EventAction* eventAction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // get volume of the current step
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  if ( volume == fDetector->GetVacStep1PV() ) {
       // get analysis manager
       auto analysisManager = G4AnalysisManager::Instance();

       // fill ntuple id=0
       analysisManager->FillNtupleIColumn(0,0, aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());

       analysisManager->FillNtupleDColumn(0,1, aStep->GetPostStepPoint()->GetKineticEnergy()/MeV);

       analysisManager->FillNtupleDColumn(0,2, aStep->GetPostStepPoint()->GetPosition().x());
       analysisManager->FillNtupleDColumn(0,3, aStep->GetPostStepPoint()->GetPosition().y());
       analysisManager->FillNtupleDColumn(0,4, aStep->GetPostStepPoint()->GetPosition().z());

       analysisManager->FillNtupleDColumn(0,5, aStep->GetTrack()->GetVertexPosition().x());
       analysisManager->FillNtupleDColumn(0,6, aStep->GetTrack()->GetVertexPosition().y());
       analysisManager->FillNtupleDColumn(0,7, aStep->GetTrack()->GetVertexPosition().z());

       analysisManager->FillNtupleDColumn(0,8, aStep->GetPostStepPoint()->GetMomentumDirection().x());
       analysisManager->FillNtupleDColumn(0,9, aStep->GetPostStepPoint()->GetMomentumDirection().y());
       analysisManager->FillNtupleDColumn(0,10, aStep->GetPostStepPoint()->GetMomentumDirection().z());

       analysisManager->FillNtupleDColumn(0,11, aStep->GetTrack()->GetPolarization().x());
       analysisManager->FillNtupleDColumn(0,12, aStep->GetTrack()->GetPolarization().y());
       analysisManager->FillNtupleDColumn(0,13, aStep->GetTrack()->GetPolarization().z());

       analysisManager->FillNtupleDColumn(0,14, aStep->GetTrack()->GetTrackID());
       analysisManager->FillNtupleDColumn(0,15, aStep->GetTrack()->GetParentID());
       analysisManager->FillNtupleDColumn(0,16, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

       analysisManager->AddNtupleRow(0);

  //G4cout<< " This part of the code you are currently testing is executed"  << G4endl;
     }

     if ( volume == fDetector->GetVacStep2PV() ) {

          auto analysisManager = G4AnalysisManager::Instance();
         //~//~//~//
         analysisManager->FillNtupleIColumn(1,0, aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());

         analysisManager->FillNtupleDColumn(1,1, aStep->GetPostStepPoint()->GetKineticEnergy()/MeV);

         analysisManager->FillNtupleDColumn(1,2, aStep->GetPostStepPoint()->GetPosition().x());
         analysisManager->FillNtupleDColumn(1,3, aStep->GetPostStepPoint()->GetPosition().y());
         analysisManager->FillNtupleDColumn(1,4, aStep->GetPostStepPoint()->GetPosition().z());

         analysisManager->FillNtupleDColumn(1,5, aStep->GetTrack()->GetVertexPosition().x());
         analysisManager->FillNtupleDColumn(1,6, aStep->GetTrack()->GetVertexPosition().y());
         analysisManager->FillNtupleDColumn(1,7, aStep->GetTrack()->GetVertexPosition().z());

         analysisManager->FillNtupleDColumn(1,8, aStep->GetPostStepPoint()->GetMomentumDirection().x());
         analysisManager->FillNtupleDColumn(1,9, aStep->GetPostStepPoint()->GetMomentumDirection().y());
         analysisManager->FillNtupleDColumn(1,10, aStep->GetPostStepPoint()->GetMomentumDirection().z());

         analysisManager->FillNtupleDColumn(1,11, aStep->GetTrack()->GetPolarization().x());
         analysisManager->FillNtupleDColumn(1,12, aStep->GetTrack()->GetPolarization().y());
         analysisManager->FillNtupleDColumn(1,13, aStep->GetTrack()->GetPolarization().z());

         analysisManager->FillNtupleDColumn(1,14, aStep->GetTrack()->GetTrackID());
         analysisManager->FillNtupleDColumn(1,15, aStep->GetTrack()->GetParentID());
         analysisManager->FillNtupleDColumn(1,16, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());

         analysisManager->AddNtupleRow(1);


    //G4cout<< " This part of the code you are currently testing is executed"  << G4endl;
       }

// Collect energy and track length step by step
 /*


  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( volume == fDetConstruction->GetAbsorberPV() ) {
    fEventAction->AddAbs(edep,stepLength);
  }

  if ( volume == fDetConstruction->GetGapPV() ) {
    fEventAction->AddGap(edep,stepLength);
  }*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......