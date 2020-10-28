//
/// \brief Definition of the Run class
//

#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "globals.hh"

class DetectorConstruction;
class G4ParticleDefinition;
class HistoManager;


class Run : public G4Run
{
 public:
   Run(DetectorConstruction*);
  ~Run();

 public:
    
   void SetPrimary(G4ParticleDefinition* particle, G4double energy);

   void AddEnergy (G4double edep)
              {fEnergyDeposit += edep; fEnergyDeposit2 += edep*edep;};

   void AddTrakLenCharg (G4double length)
              {fTrakLenCharged += length; fTrakLenCharged2 += length*length;};

   void AddTrakLenNeutr (G4double length)
              {fTrakLenNeutral += length; fTrakLenNeutral2 += length*length;};

   void AddMscProjTheta (G4double theta)
              {if (std::abs(theta) <= fMscThetaCentral) { fMscEntryCentral++;
                 fMscProjecTheta += theta;  fMscProjecTheta2 += theta*theta;}
              };

   void CountStepsCharg (G4int nSteps)
              {fNbStepsCharged += nSteps; fNbStepsCharged2 += nSteps*nSteps;};

   void CountStepsNeutr (G4int nSteps)
              {fNbStepsNeutral += nSteps; fNbStepsNeutral2 += nSteps*nSteps;};

   void CountParticles (G4ParticleDefinition* part)
              {     if (part == G4Gamma::Gamma())       fNbGamma++ ;
               else if (part == G4Electron::Electron()) fNbElect++ ;
               else if (part == G4Positron::Positron()) fNbPosit++ ; };

   void CountTransmit (G4int flag)
              {     if (flag == 1)  fTransmit[0]++;
               else if (flag == 2) {fTransmit[0]++; fTransmit[1]++; }};

   void CountReflect (G4int flag)
              {     if (flag == 1)  fReflect[0]++;
               else if (flag == 2) {fReflect[0]++; fReflect[1]++; }};
    
   void AddEnergyLeak (G4double eleak, G4int index)
            {fEnergyLeak[index] += eleak; fEnergyLeak2[index] += eleak*eleak;};
            
   G4double ComputeMscHighland();
   
   virtual void Merge(const G4Run*);
   
   void EndOfRun(); 
   
   void SetHistoManager (HistoManager *hmng);
   G4double GetTreeCutX(void) const { return fTreeCutX; }
   G4double GetTreeCutY(void) const { return fTreeCutY; }
   G4ParticleDefinition *GetTreeParticle(void) const { return fTreeParticle; }

 private:
    DetectorConstruction*  fDetector;
    G4ParticleDefinition*  fParticle;
    G4double  fEkin;
                           
    G4double fEnergyDeposit,  fEnergyDeposit2;
    G4double fTrakLenCharged, fTrakLenCharged2;
    G4double fTrakLenNeutral, fTrakLenNeutral2;
    G4double fNbStepsCharged, fNbStepsCharged2;
    G4double fNbStepsNeutral, fNbStepsNeutral2;
    G4double fMscProjecTheta, fMscProjecTheta2;
    G4double fMscThetaCentral;
    
    G4int    fNbGamma, fNbElect, fNbPosit;
    G4int    fTransmit[2],   fReflect[2];
    G4int    fMscEntryCentral;
    
    G4double fEnergyLeak[2],  fEnergyLeak2[2];
    
    HistoManager *fHistoManager;
    G4double  fTreeCutX, fTreeCutY;
    G4ParticleDefinition  *fTreeParticle;
    
};


#endif

