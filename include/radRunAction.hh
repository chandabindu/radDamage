#ifndef radRunAction_h
#define radRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class radRunAction : public G4UserRunAction
{
  public:
    radRunAction();
    virtual ~radRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

