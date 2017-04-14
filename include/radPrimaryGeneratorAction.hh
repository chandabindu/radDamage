#ifndef radPrimaryGeneratorAction_h
#define radPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class radPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  radPrimaryGeneratorAction();    
  virtual ~radPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);

  void SetGunEnergy(G4double val){gunEnergy=val;}

  void SetGunPosX(G4double val){gunPosX=val;}
  void SetGunPosY(G4double val){gunPosY=val;}
  void SetGunPosZ(G4double val){gunPosZ=val;}

private:

  G4ParticleGun*  fParticleGun; // G4 particle gun
  G4double gunEnergy;
  G4double gunPosX;
  G4double gunPosY;
  G4double gunPosZ;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


