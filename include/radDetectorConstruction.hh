#ifndef radDetectorConstruction_h
#define radDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4Sphere;
class G4VPhysicalVolume;
class G4UniformMagField;
class G4GenericMessenger;

class radDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  radDetectorConstruction();
  virtual ~radDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();

  void SetNrDetectors( G4int val ){nrDet = val;}
  void SetTargetMaterial(std::string val){targetMaterial = val;}
  void UpdateGeometry();

private:  
  void DefineMaterials();  
  
  G4int nrDet;
  std::string targetMaterial;
  G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

