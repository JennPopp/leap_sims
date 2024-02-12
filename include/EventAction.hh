// EventAction.hh
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "ConfigReader.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;

protected:
    ConfigReader fConfig;
    std::string fOutputMode;
    std::vector<TreeInfo> fTreesInfo;
};