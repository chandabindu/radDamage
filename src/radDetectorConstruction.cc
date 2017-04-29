#include "radDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4GenericMessenger.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UserLimits.hh"
#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include <stdio.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

radDetectorConstruction::radDetectorConstruction()
  : G4VUserDetectorConstruction(),
    targetMaterial("copper"),
    fCheckOverlaps(true)
{  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

radDetectorConstruction::~radDetectorConstruction(){ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void radDetectorConstruction::UpdateGeometry(){
  // clean-up previous geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();
  
  //define new one
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* radDetectorConstruction::Construct(){
  // Define materials 
  DefineMaterials();

  // Get materials
  G4Material* vacuumMaterial=G4Material::GetMaterial("Galactic");
  G4Material* tgtMaterial;
  if( targetMaterial == "calcium" )
    tgtMaterial = G4Material::GetMaterial("G4_Ca");
  else if( targetMaterial == "lead" )
    tgtMaterial = G4Material::GetMaterial("G4_Pb");
  else if( targetMaterial == "copper" )
    tgtMaterial = G4Material::GetMaterial("G4_Cu");
  else{
    G4cerr << __PRETTY_FUNCTION__
	   <<"No material for target with name: "<<targetMaterial<<G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }
  G4Material* airMaterial = G4Material::GetMaterial("Air");  
  if ( !tgtMaterial || !airMaterial ||
       !vacuumMaterial) {
    G4cerr << __PRETTY_FUNCTION__ << " Cannot retrieve materials already defined. " << G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }  

  // World
  G4double worldSizeXYZ = 5 * m;  
  G4VSolid* worldS 
    = new G4Box("World",           // its name
		worldSizeXYZ, worldSizeXYZ, worldSizeXYZ); // its size
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
			  worldS,           // its solid
			  airMaterial,  // its material
			  "World");         // its name                                   
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
			0,                // no rotation
			G4ThreeVector(),  // at (0,0,0)
			worldLV,          // its logical volume                         
			"World",          // its name
			0,                // its mother  volume
			false,            // no boolean operation
			0,                // copy number
			fCheckOverlaps);  // checking overlaps 

  //Target
  //default is copper
  G4double tgtR = 5 * cm, tgtLen=2*cm;
  if(targetMaterial == "lead"){
    tgtLen = 0.5*cm;
  }else if(targetMaterial == "calcium"){
    tgtLen = 0.6*cm;    
  }

  G4VSolid* tgtS 
    = new G4Tubs("targetS",           // its name
                 0, tgtR, tgtLen/2., //inner R, outer R, length
		 0*deg,360*deg);
  G4LogicalVolume* tgtL
    = new G4LogicalVolume(
			  tgtS,           // its solid
			  tgtMaterial,  // its material
			  "targetL");         // its name
  G4Colour  blue(0/255.,0/255.,255/255.);
  G4VisAttributes* tgtVisAtt = new G4VisAttributes(blue);
  tgtVisAtt->SetForceSolid(true);
  tgtVisAtt->SetVisibility(true);
  tgtL->SetVisAttributes(tgtVisAtt);
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(),  // at (0,0,0)
		    tgtL,             // its logical volume                         
		    "target",         // its name
		    worldLV,          // its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
  //
  //Detectors
  //
  G4double detectorThickness = 0.01 * mm;
  G4double radialDetRadius = 2*m;
  G4VSolid *radialDetS = new G4Sphere("radialDetS",
				      radialDetRadius - detectorThickness/2,
				      radialDetRadius + detectorThickness/2,
				      0*deg,360*deg, //phi Min, Max
				      0*deg,180*deg);//theta Min, Max
				      
  G4LogicalVolume* radialDetL
    = new G4LogicalVolume(
			  radialDetS,           // its solid
			  vacuumMaterial,  // its material
			  "targetL");         // its name
  G4Colour  red(255/255.,0/255.,0/255.);
  G4VisAttributes* detVisAtt = new G4VisAttributes(red);
  detVisAtt->SetForceSolid(true);
  detVisAtt->SetVisibility(true);
  radialDetL->SetVisAttributes(detVisAtt);
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(),  // at (0,0,0)
		    radialDetL,       // its logical volume                         
		    "radialDetector", // its name
		    worldLV,          // its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
  
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void radDetectorConstruction::DefineMaterials()
{ 

  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_Ca");
  nistManager->FindOrBuildMaterial("G4_Cu");

  // Air material: Air 18 degr.C and 58% humidity
  G4double fractionmass(0);
  G4Element* elH  = nistManager->FindOrBuildElement("H");
  G4Element* elN  = nistManager->FindOrBuildElement("N");
  G4Element* elO  = nistManager->FindOrBuildElement("O");
  G4Element* elAr = nistManager->FindOrBuildElement("Ar");
  G4Material  *matAir = new G4Material("Air",1.214*mg/cm3,4);
  matAir -> AddElement(elN,  fractionmass=0.7494);
  matAir -> AddElement(elO,  fractionmass=0.2369);
  matAir -> AddElement(elAr, fractionmass=0.0129);
  matAir -> AddElement(elH,  fractionmass=0.0008);

  // // Vacuum
  new G4Material("Galactic", 1., 1.01*g/mole, universe_mean_density,
		 kStateGas, 2.73*kelvin, 3.e-18*pascal);
    
  // Print materials
  G4cout << G4endl << G4endl << "~~~~~~~~~~~~~~~~~~~~~Material Printout~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
}

