#ifndef radEventAction_h
#define radEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4GenericMessenger;

class radEventAction : public G4UserEventAction
{
public:
  radEventAction(G4int *evN);
  virtual ~radEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
                     
  // set methods
  void SetPrintModulo(G4int value);
    
private:
  
  // data members                   
  G4GenericMessenger*  fMessenger;
  G4int  fPrintModulo;
  G4int *evNr;
};

// inline functions

inline void radEventAction::SetPrintModulo(G4int value) {
  fPrintModulo = value;
}
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
