//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "ConfigReader.hh"
#include "PhysicsList.hh"
#include "PhysListEmPolarized.hh"
#include "PhysListOptical.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmParameters.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(const ConfigReader& config)
: G4VModularPhysicsList(),fConfig(config),
  fEmPhysicsList(0), fOptPhysicsList(0)
{
  fPolStatus = config.GetConfigValue("PhysicsList", "polarizationStatus");
  fOptStatus = config.GetConfigValue("PhysicsList", "opticalStatus");
  G4EmParameters::Instance();

  SetVerboseLevel(1);
  if (fPolStatus=="On"){
    fEmPhysicsList = new PhysListEmPolarized();
  }else{
    fEmPhysicsList = new G4EmStandardPhysics();
  }
  
  fOptPhysicsList= new PhysListOptical();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
  delete fEmPhysicsList;
  delete fOptPhysicsList;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4EmParameters.hh"

void PhysicsList::ConstructProcess()
{
  // Transportation
  //
  AddTransportation();

  // Electromagnetic physics list
  //
  fEmPhysicsList->ConstructProcess();

  // Optical processes just active if Calorimeter is in use (have to check if this is necessary)
  if(fOptStatus == "On"){
  fOptPhysicsList->ConstructProcess();
  }

  // step limitation (as a full process)
  //
  AddStepMax();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ProcessManager.hh"
#include "StepMax.hh"

void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  StepMax* stepMaxProcess = new StepMax();

  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while ((*particleIterator)()){
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (stepMaxProcess->IsApplicable(*particle) && pmanager)
          pmanager ->AddDiscreteProcess(stepMaxProcess);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
