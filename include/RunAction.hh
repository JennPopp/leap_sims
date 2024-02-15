// MyRunAction.hh
#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include <memory> // For std::unique_ptr

#include "AnaConfigManager.hh"

class G4Run;

class RunAction : public G4UserRunAction {
public:
    RunAction(AnaConfigManager& anaConfigManager);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run* run) override;
    virtual void EndOfRunAction(const G4Run* run) override;

    const std::string& GetOutputMode() const { return fOutputMode; }
    const std::vector<TreeInfo>& GetTreesInfo() const { return fTreesInfo; }

private:
    AnaConfigManager& fAnaConfigManager;
    const std::string fOutputMode;
    const std::vector<TreeInfo> fTreesInfo;
    
};

#endif