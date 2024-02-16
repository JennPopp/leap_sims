#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include <vector>
#include <string>
#include "AnaConfigManager.hh"

class EventAction : public G4UserEventAction {
public:
    EventAction(AnaConfigManager& anaConfigManager); // Constructor
    virtual ~EventAction(); // Destructor

    // Overridden methods from G4UserEventAction
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;

private:
    AnaConfigManager& fAnaConfigManager;
    const std::string fOutputMode;
    const std::vector<TreeInfo> fTreesInfo;
    
};

#endif // EVENTACTION_HH
