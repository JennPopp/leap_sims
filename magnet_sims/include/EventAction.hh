/// \brief Definition of the EventAction class
//

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
   ~EventAction();

  public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    
    void AddEnergy      (G4double edep)   {fEnergyDeposit  += edep;};
    void AddTrakLenCharg(G4double length) {fTrakLenCharged += length;};
    void AddTrakLenNeutr(G4double length) {fTrakLenNeutral += length;};
    
    void CountStepsCharg ()               {fNbStepsCharged++ ;};
    void CountStepsNeutr ()               {fNbStepsNeutral++ ;};
    
    void SetTransmitFlag (G4int flag) 
                           {if (flag > fTransmitFlag) fTransmitFlag = flag;};
    void SetReflectFlag  (G4int flag) 
                           {if (flag > fReflectFlag)   fReflectFlag = flag;};
                           
    void AddKinEnegyCharged (const G4double ek) { fKinetikEnergyCharged.push_back(ek); };                 
    void AddKinEnegyGamma (const G4double ek) { fKinetikEnergyGamma.push_back(ek); };

  private:
    G4double fEnergyDeposit;
    G4double fTrakLenCharged, fTrakLenNeutral;
    G4int    fNbStepsCharged, fNbStepsNeutral;
    G4int    fTransmitFlag,   fReflectFlag; 
    
    std::vector<G4double> fKinetikEnergyGamma;
    std::vector<G4double> fKinetikEnergyCharged;

 };

#endif

    
