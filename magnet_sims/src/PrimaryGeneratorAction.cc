//
/// \brief Implementation of the PrimaryGeneratorAction class
//


#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"
#include "PrimarySpectra.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* DC)
:G4VUserPrimaryGeneratorAction(),
 fParticleGun(0),fDetector(DC),fBeamType(beamGauss),
 fx0(0.0), fy0(0.0), fz0(0.0), fsigmax(0.5*um), fsigmay(0.5*um), fsigmaz(24.0*um),
 femittancex(0.0), femittancey(0.0), fbetax(0.0), fbetay(0.0), fbeamfocus(8.1*m), fSpectra(0),
 fGunMessenger(0)

{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
  SetDefaultKinematic();

  //create a messenger for this class
  fGunMessenger = new PrimaryGeneratorMessenger(this);
}



PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;
  delete fSpectra;
}


void PrimaryGeneratorAction::SetDefaultKinematic()
{
  // default particle kinematic
  // defined by the beam parameters: emittancex, emittancey, fsigmax, fsigmay, fsigmaz and drift distance to IP fz0.
  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(60.0*MeV);

  fz0 = -20.0*cm;

  fsigmax = 0.5*um;
  fsigmay = 0.5*um;

  fsigmaz = 24.0*um;

  G4double lf = fParticleGun->GetParticleEnergy() / particle->GetPDGMass();

  femittancex = 1.4e-3 * mm / lf;
  femittancey = 1.4e-3 * mm / lf;

  fbetax = fsigmax*fsigmax/femittancex;
  fbetay = fsigmay*fsigmay/femittancey;

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticlePosition(G4ThreeVector(fx0, fy0, fz0));
}


void PrimaryGeneratorAction::SetBeamType(G4String val)
{
  if (val == "gaussian") {
    fBeamType = beamGauss;
  } else if (val == "mono") {
    fBeamType = beamMono;
  } else {
    std::cout << "PrimaryGeneratorAction::SetBeamType: <" << val << ">"
           << " is not defined"
           << std::endl;
    return;
  }
  std::cout << "PrimaryGeneratorAction: Set Beam Type to : " << val << std::endl;
}



void PrimaryGeneratorAction::SetSigmaX(const G4double sigma)
{
  fsigmax = sigma;
  fbetax = fsigmax*fsigmax/femittancex;
  std::cout << "PrimaryGeneratorAction: Set beam sigmaX at IP to : " << G4BestUnit(fsigmax, "Length") << std::endl;
//G4cout does not print anything...   G4cout << "Set beam sigmaX at IP to : " << G4BestUnit(fsigmax, "Length") << G4endl;
}



void PrimaryGeneratorAction::SetSigmaY(const G4double sigma)
{
  fsigmay = sigma;
  fbetay = fsigmay*fsigmay/femittancey;
  std::cout << "PrimaryGeneratorAction: Set beam sigmaY at IP to : " << G4BestUnit(fsigmay, "Length") << std::endl;
//   G4cout << "Set beam sigmaY at IP to : " << G4BestUnit(fsigmay, "Length") << G4endl;
}



void PrimaryGeneratorAction::SetBeamFocus(const G4double zfocus)
{
  fbeamfocus = zfocus;
  std::cout << "PrimaryGeneratorAction: Set beam focus to : " << G4BestUnit(fbeamfocus, "Length") << std::endl;
}



void PrimaryGeneratorAction::SetSpectraFile(G4String val)
{
  if (fSpectra) delete fSpectra;
  fSpectra = new PrimarySpectra();
//  fSpectra->SetVerbose(1);
  int ndata = fSpectra->LoadData(val);
  fSpectra->SetScale(GeV);
  std::cout << "PrimaryGeneratorAction: Primary spectra with " << ndata << " data points was loaded from the file " << val << std::endl;
}



void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // this function is called at the begining of event
  if (fSpectra) fParticleGun->SetParticleEnergy(fSpectra->GetRandom());

  if (fBeamType == beamGauss) {
    GenerateGaussian(anEvent);
  } else if (fBeamType == beamMono) {
    GenerateMono(anEvent);
  }
}



void PrimaryGeneratorAction::GenerateGaussian(G4Event* anEvent)
{
  // it generate particle according to defined emittance drifting from fz0 towards an IP
  // where the beam size is fsigmax, fsigmay, fsigmaz.
  G4double zshift = -20.0*cm;
  if (zshift > fDetector->GetzstartAbs() - 100.0*fsigmaz) {
    zshift = 0.5 * (fDetector->GetzstartAbs() - 0.5 * fDetector->GetWorldSizeZ());
    if (zshift > fDetector->GetzstartAbs() - 100.0*fsigmaz) {
      G4String msgstr("Error setting initial position!\n");
      G4Exception("PrimaryGeneratorAction::", "GenerateGaussian(Event)", FatalException, msgstr.c_str());
    }
  }

  fz0 = zshift;  // in SetDefaultKinematic fDetector is before update from the config file.
//   double zfoucus = 0.5 * fDetector->GetWorldSizeZ();
  double zfoucus = fbeamfocus;

  G4double z0 = G4RandGauss::shoot(fz0, fsigmaz);
  z0 -= zfoucus;  // This is needed to have correct drift distance for x, y distribution.

  G4double sigmax = fsigmax * sqrt(1.0 + pow(z0/fbetax, 2.0));
  G4double x0 = G4RandGauss::shoot(fx0, sigmax);
  G4double meandx = x0*z0 / (z0*z0 + fbetax*fbetax);
  G4double sigmadx = sqrt( femittancex*fbetax / (z0*z0 + fbetax*fbetax) );
  G4double dx0 = G4RandGauss::shoot(meandx, sigmadx);

  G4double sigmay = fsigmay * sqrt(1.0 + pow(z0/fbetay, 2.0));
  G4double y0 = G4RandGauss::shoot(fy0, sigmay);
  G4double meandy = y0*z0 / (z0*z0 + fbetay*fbetay);
  G4double sigmady = sqrt( femittancey*fbetay / (z0*z0 + fbetay*fbetay) );
  G4double dy0 = G4RandGauss::shoot(meandy, sigmady);

  G4double mass = fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4double E = fParticleGun->GetParticleEnergy();

  G4double pz = sqrt( (E*E - mass*mass)/ (dx0*dx0 + dy0*dy0 + 1.0) );

  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0 + zfoucus ));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dx0*pz, dy0*pz, pz));
  fParticleGun->GeneratePrimaryVertex(anEvent);
}



void PrimaryGeneratorAction::GenerateMono(G4Event* anEvent)
{
  G4double zshift = -20.0*cm;
  if (zshift > fDetector->GetzstartAbs()) {
    zshift = 0.5 * (fDetector->GetzstartAbs() - 0.5 * fDetector->GetWorldSizeZ());
  }
  fz0 = zshift;  // in SetDefaultKinematic fDetector is before update from the config file.
  fx0 = fy0 = 0.0;

  G4double mass = fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4double E = fParticleGun->GetParticleEnergy();
  G4double pz = sqrt(E*E - mass*mass);
  Polx0=0.0; Poly0=0.0; Polz0=1.0;

  fParticleGun->SetParticlePosition(G4ThreeVector(fx0, fy0, fz0));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, pz));
  fParticleGun->SetParticlePolarization(G4ThreeVector(Polx0, Poly0, Polz0));
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
