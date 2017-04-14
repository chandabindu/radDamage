#ifndef mscPrimaryGeneratorAction_h
#define mscPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class mscPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  mscPrimaryGeneratorAction();    
  virtual ~mscPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);
  void SetGunEnergy(G4double val){gunEnergy=val;}
  
private:
  G4ParticleGun*  fParticleGun; // G4 particle gun
  G4double gunEnergy;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


