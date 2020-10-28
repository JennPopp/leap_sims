//
/// \brief Implementation of the RunAction class
//

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include "Run.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4EmCalculator.hh"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <iomanip>



RunAction::RunAction(DetectorConstruction* det, PrimaryGeneratorAction* kin)
:G4UserRunAction(),fDetector(det), fPrimary(kin), fRun(0), fHistoManager(0)
{ 
  // Book predefined histograms
  fHistoManager = new HistoManager();
}



RunAction::~RunAction()
{ 
  delete fHistoManager;
}



G4Run* RunAction::GenerateRun()
{ 
  fRun = new Run(fDetector);
  fRun->SetHistoManager(fHistoManager);
  return fRun;
}



void RunAction::BeginOfRunAction(const G4Run*)
{
  // save Rndm status
  ////  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  if (isMaster) G4Random::showEngineStatus();
     
  // keep run condition
  if ( fPrimary ) { 
    G4ParticleDefinition* particle 
      = fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    fRun->SetPrimary(particle, energy);
  }
  
  //histograms
  //        
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->IsActive() ) {
    PrimaryGeneratorAction *gen = new PrimaryGeneratorAction(fDetector);
    if ( gen ) { 
//      G4double zmax = fabs(0.5 * fDetector->GetWorldSizeZ() - gen->GetZ0()); 
      G4double zmax = fabs(gen->GetBeamFocus() - gen->GetZ0()); 
   
      G4double beta = gen->GetBetaX();
      G4double range = 5.0*gen->GetSigmaX() * sqrt(1.0 + pow(zmax/beta, 2.0) ); 
      G4double drange = range*zmax/(zmax*zmax + beta*beta) + 5.0*sqrt((gen->GetEmittanceX()*beta)/(zmax*zmax + beta*beta));
      analysisManager->SetH2Activation(10, true);
      analysisManager->SetH2(10, 500, -range, range, 500, -drange, drange);  
      analysisManager->SetH1Activation(53, true);
      analysisManager->SetH1(53, 1000, -range, range);  
     
      G4cout << "x range : " << range << "   " << drange << G4endl;

      beta = gen->GetBetaY();
      range = 5.0*gen->GetSigmaY() * sqrt(1.0 + pow(zmax/beta, 2.0) ); 
      drange = range*zmax/(zmax*zmax + beta*beta) + 5.0*sqrt((gen->GetEmittanceY()*beta)/(zmax*zmax + beta*beta));
      analysisManager->SetH2Activation(11, true);
      analysisManager->SetH2(11, 500, -range, range, 500, -drange, drange);  
      analysisManager->SetH1Activation(54, true);
      analysisManager->SetH1(54, 1000, -range, range);  

      G4cout << "y range : " << range << "   " << drange << G4endl;
      
      range = zmax/c_light;
      drange = 10.0*gen->GetSigmaZ()/c_light;
      analysisManager->SetH1Activation(61, true);
      analysisManager->SetH1(61, 1000, range - drange, range + drange);  
      analysisManager->SetH1Activation(62, true);
      analysisManager->SetH1(62, 1000, range - drange, range+drange);  
      G4cout << "t range : " << range - drange << "   " << range + drange << G4endl;
    }
    delete gen;
    analysisManager->OpenFile();
  } 
}



void RunAction::EndOfRunAction(const G4Run*)
{  
  // print Run summary
  //
  if (isMaster) fRun->EndOfRun();    
      
  // save histograms
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();  
  if ( analysisManager->IsActive() ) {    
    analysisManager->Write();
    analysisManager->CloseFile();
  }  

  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();
}


