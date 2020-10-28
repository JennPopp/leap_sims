//
/// \brief Implementation of the HistoManager class
//

#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "HistoMessenger.hh"


HistoManager::HistoManager()
  : fFileName("lxphoton_out_vg1"), fHMessanger(0),
    fTreeCutX(25.0*mm), fTreeCutY(25.0*mm),
    fTreeParticle("gamma")
{
  Book();
  fHMessanger = new HistoMessenger(this);
}



HistoManager::~HistoManager()
{
  delete G4AnalysisManager::Instance();
  delete fHMessanger;
}



void HistoManager::Book()
{
  // Create or get analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in HistoManager.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetFileName(fFileName);
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetActivation(true);    // enable inactivation of histograms

  // Define histograms start values
  const G4int kMaxHisto = 71;
  const G4String id[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                         "10","11","12","13","14","15","16","17","18","19",
                         "20","21","22","23","24","25","26","27","28","29",
                         "30","31","32","33","34","35","36","37","38","39",
                         "40","41","42","43","44","45","46","47","48","49",
                         "50","51","52", "53","54","55","56","57","58","59",
                         "60","61","62", "63","64","65","66","67","68","69",
                         "70"
                        };

  const G4String title[] =
                { "dummy",                                                //0
                  "energy deposit in absorber",                           //1
                  "energy of charged secondaries at creation",            //2
                  "energy of neutral secondaries at creation",            //3
                  "energy of charged at creation (log scale)",            //4
                  "energy of neutral at creation (log scale)",            //5
                  "x_vertex of charged secondaries (all)",                //6
                  "x_vertex of charged secondaries (not absorbed)",       //7
                  "dummy","dummy",                                        //8-9
                  "(transmit, charged) : kinetic energy at exit",         //10
                  "(transmit, charged) : ener fluence: dE(MeV)/dOmega",   //11
                  "(transmit, charged) : space angle: dN/dOmega",         //12
                  "(transmit, charged) : projected angle at exit",        //13
                  "(transmit, charged) : projected position at exit",     //14
                  "(transmit, charged) : radius at exit",                 //15
                  "energy of Auger e- at creation",                       //16
                  "energy of fluorescence gamma at creation",             //17
                  "energy of Auger e- at creation (log scale)",           //18
                  "energy of fluorescence gamma at creation (log scale)", //19
                  "(transmit, neutral) : kinetic energy at exit",         //20
                  "(transmit, neutral) : ener fluence: dE(MeV)/dOmega",   //21
                  "(transmit, neutral) : space angle: dN/dOmega",         //22
                  "(transmit, neutral) : projected angle at exit",        //23
                  "gamma: x-position at exit",                            //24
                  "gamma: radius at exit",                                //25
                  "dummy","dummy","dummy","dummy",       //26-29
                  "(reflect , charged) : kinetic energy at exit",         //30
                  "(reflect , charged) : ener fluence: dE(MeV)/dOmega",   //31
                  "(reflect , charged) : space angle: dN/dOmega",         //32
                  "(reflect , charged) : projected angle at exit",        //33
                  "dummy","dummy","dummy","dummy","dummy","dummy",       //34-39
                  "(reflect , neutral) : kinetic energy at exit",         //40
                  "(reflect , neutral) : ener fluence: dE(MeV)/dOmega",   //41
                  "(reflect , neutral) : space angle: dN/dOmega",         //42
                  "(reflect , neutral) : projected angle at exit",        //43
                  "energy of PIXE Auger e- at creation",                  //44
                  "energy of PIXE gamma at creation",                     //45
                  "energy of PIXE Auger e- at creation (log scale)",      //46
                  "energy of PIXE gamma at creation (log scale)",         //47
                  "dummy","dummy",                                        //48-49
                  "dummy",
                  "Z vertex position of charged produced after the magnet", //51
                  "Z vertex position of neutral produced after the magnet", //52

                  "X vertex position of primary particles", //53
                  "Y vertex position of primary particles", //54
                  "Z vertex position of primary particles", //55

                  "PX of primary particles", //56
                  "PY of primary particles", //57
                  "PZ of primary particles", //58

                  "X at focus of primary particles", //59
                  "Y at focus of primary particles", //60

                  "electron time at exit", //61
                  "gamma time at exit", //62

                  "dummy","dummy",
                  "electron spectrum",  //65
                  "photon spectrum",    //66
                  "positron spectrum",  //67

                  "electron x",  //68
                  "photon x",    //69
                  "positron x"  //70

                };

  // Default values (to be reset via /analysis/h1/set command)
  G4int nbins = 100;
  G4double vmin = 0.;
  G4double vmax = 100.;

  // Create all histograms as inactivated
  // as we have not yet set nbins, vmin, vmax
  for (G4int k=0; k<kMaxHisto; k++) {
    G4int ih = analysisManager->CreateH1("h"+id[k], title[k], nbins,vmin,vmax);
    analysisManager->SetH1Activation(ih, false);
  }

// ob
  const G4int kMaxHisto2D = 13;
  const G4String id2D[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"
                        };

  const G4String title2D[] =
                { "(transmit, charged) : projected position at exit vs energy",  //0
                  "(transmit, charged vs neutral): kinetic energy e-gamma at exit", //1
                  "(transmit, charged): x, y position at exit", //2
                  "(transmit, neutral): x, y position at exit", //3
                  "Z vertex position vs E of charged produced after the magnet", //4
                  "Z vertex position vs E of neutral produced after the magnet", //5
                  "electron polar anvgle vs E at exit",     // 6
                  "photons polar anvgle vs E at exit",      // 7
                  "photons polar anvgle vs E at exit, urad scale",      // 8
//                   "photons radial position vs momentum theta at exit"   //9
                  "photons position x, y at exit",   //9
                  "primary electron x, vs dx/dz",   //10
//                   "primary electron y, vs dy/dz"   //11
                  "electron x, vs dx/dz at exit",   //11
                  "positron polar anvgle vs E at exit"   //12
                };

  for (G4int k=0; k<kMaxHisto2D; k++) {
    G4int ih = analysisManager->CreateH2("h2d"+id2D[k], title2D[k], nbins,vmin,vmax, nbins,vmin,vmax);
    analysisManager->SetH2Activation(ih, false);
//     G4cout << "2D histogram id: " << ih << G4endl;


  }

  // Creating ntuple vacstep1 , id=0
  //
  analysisManager->CreateNtuple("bremssim1", "vacstep1");
  analysisManager->CreateNtupleIColumn("pdg");
  analysisManager->CreateNtupleIColumn("charge");
  analysisManager->CreateNtupleDColumn("E");
  analysisManager->CreateNtupleDColumn("x");
  analysisManager->CreateNtupleDColumn("y");
  analysisManager->CreateNtupleDColumn("z");
  analysisManager->CreateNtupleDColumn("startx");
  analysisManager->CreateNtupleDColumn("starty");
  analysisManager->CreateNtupleDColumn("startz");
  analysisManager->CreateNtupleDColumn("px");
  analysisManager->CreateNtupleDColumn("py");
  analysisManager->CreateNtupleDColumn("pz");
  analysisManager->CreateNtupleDColumn("Polx");
  analysisManager->CreateNtupleDColumn("Poly");
  analysisManager->CreateNtupleDColumn("Polz");
  analysisManager->CreateNtupleDColumn("TrackID");
  analysisManager->CreateNtupleDColumn("ParentID");
  analysisManager->CreateNtupleDColumn("EventID");
  analysisManager->FinishNtuple();

  // Creating ntuple vacstep2 , id=1
  //
  analysisManager->CreateNtuple("bremssim2", "vacstep2");
  analysisManager->CreateNtupleIColumn("pdg");
  analysisManager->CreateNtupleIColumn("charge");
  analysisManager->CreateNtupleDColumn("E");
  analysisManager->CreateNtupleDColumn("x");
  analysisManager->CreateNtupleDColumn("y");
  analysisManager->CreateNtupleDColumn("z");
  analysisManager->CreateNtupleDColumn("startx");
  analysisManager->CreateNtupleDColumn("starty");
  analysisManager->CreateNtupleDColumn("startz");
  analysisManager->CreateNtupleDColumn("px");
  analysisManager->CreateNtupleDColumn("py");
  analysisManager->CreateNtupleDColumn("pz");
  analysisManager->CreateNtupleDColumn("Polx");
  analysisManager->CreateNtupleDColumn("Poly");
  analysisManager->CreateNtupleDColumn("Polz");
  analysisManager->CreateNtupleDColumn("TrackID");
  analysisManager->CreateNtupleDColumn("ParentID");
  analysisManager->CreateNtupleDColumn("EventID");
  analysisManager->FinishNtuple();

}
