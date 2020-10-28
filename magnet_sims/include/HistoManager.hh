//
/// \brief Definition of the HistoManager class
//

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"

#include "g4root.hh"
//#include "g4xml.hh"

class HistoMessenger;

class HistoManager
{
  public:
    HistoManager();
   ~HistoManager();
    
    void SetTreeCutX(const G4double cx) { fTreeCutX = cx; }
    void SetTreeCutY(const G4double cy) { fTreeCutY = cy; }
    void SetTreeParticle(const G4String pstr) {fTreeParticle = pstr; }

    G4double GetTreeCutX() const { return fTreeCutX; }
    G4double GetTreeCutY() const { return fTreeCutY; }
    G4String GetTreeParticle() const { return fTreeParticle;}
    
  private:
    void Book();
    
    G4String  fFileName;
    
    HistoMessenger *fHMessanger;
    G4double  fTreeCutX, fTreeCutY;
    G4String  fTreeParticle;
};



#endif

