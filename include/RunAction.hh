// MyRunAction.hh
#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include <memory> // For std::unique_ptr

class G4Run;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run* run) override;
    virtual void EndOfRunAction(const G4Run* run) override;

    const std::map<std::string, int>& GetNtupleNameToIdMap() const { return ntupleNameToIdMap; }

private:
    void SetUpTtrees();
    std::map<std::string, int> ntupleNameToIdMap;
};

#endif