//
/// \brief Implementation of the EventAction class
//

#include "EventAction.hh"

#include "Run.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"



EventAction::EventAction()
:G4UserEventAction(),
 fEnergyDeposit(0.),
 fTrakLenCharged(0.), fTrakLenNeutral(0.),
 fNbStepsCharged(0), fNbStepsNeutral(0),
 fTransmitFlag(0), fReflectFlag(0)
{ }



EventAction::~EventAction()
{ }



void EventAction::BeginOfEventAction(const G4Event* )
{
 // initialisation per event
 fEnergyDeposit  = 0.;
 fTrakLenCharged = fTrakLenNeutral = 0.; 
 fNbStepsCharged = fNbStepsNeutral = 0;
 fTransmitFlag   = fReflectFlag    = 0;    
 
 fKinetikEnergyGamma.clear();
 fKinetikEnergyCharged.clear();
}



void EventAction::EndOfEventAction(const G4Event*)
{
  Run* run = static_cast<Run*>(
             G4RunManager::GetRunManager()->GetNonConstCurrentRun());
              
 run->AddEnergy(fEnergyDeposit);
 run->AddTrakLenCharg(fTrakLenCharged);
 run->AddTrakLenNeutr(fTrakLenNeutral);

 run->CountStepsCharg(fNbStepsCharged);
 run->CountStepsNeutr(fNbStepsNeutral);

 run->CountTransmit (fTransmitFlag);
 run->CountReflect  (fReflectFlag);

 G4AnalysisManager::Instance()->FillH1(1,fEnergyDeposit);

 G4double npart = static_cast<G4double>(fKinetikEnergyCharged.size() * fKinetikEnergyGamma.size());
 for (std::vector<G4double>::const_iterator itre = fKinetikEnergyCharged.begin(); itre != fKinetikEnergyCharged.end(); ++itre) {
   for (std::vector<G4double>::const_iterator itrg = fKinetikEnergyGamma.begin(); itrg != fKinetikEnergyGamma.end(); ++itrg) {
     G4AnalysisManager::Instance()->FillH2(1, *itre, *itrg, 1.0/npart);
   }
 }
}



