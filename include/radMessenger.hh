#ifndef radMessenger_HH
#define radMessenger_HH

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4VModularPhysicsList.hh"

class radDetectorConstruction;
class radEventAction;
class radPrimaryGeneratorAction;
class radSteppingAction;

class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class radMessenger : public G4UImessenger {
public:
  radMessenger();
  ~radMessenger();
  
  void SetPriGen ( radPrimaryGeneratorAction *pg ){ fPriGen  = pg; }
  void SetDetCon ( radDetectorConstruction *dc )  { fDetCon  = dc; }
  void SetEvAct  ( radEventAction *ev )           { fEvAct   = ev; }
  void SetStepAct( radSteppingAction *st )        { fStepAct = st; }
  
  void SetNewValue(G4UIcommand* cmd, G4String newValue);
  
private:
  radDetectorConstruction *fDetCon;
  radEventAction *fEvAct;
  radPrimaryGeneratorAction *fPriGen;
  radSteppingAction *fStepAct;

  G4UIcmdWithAnInteger *nrDetCmd;
  G4UIcmdWithAString *tgtMatCmd;
  G4UIcmdWithADoubleAndUnit *gunEnergyCmd;
  
};

#endif//radMessenger_HH























