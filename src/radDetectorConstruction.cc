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

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
 
#include <stdio.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

radDetectorConstruction::radDetectorConstruction()
 : G4VUserDetectorConstruction(),
   nrDet(40),
   targetMaterial("copper"),
   fCheckOverlaps(true)
{  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

radDetectorConstruction::~radDetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* radDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();

  if(nrDet>=50){
    G4cerr << __PRETTY_FUNCTION__
	   << " number of detector units for the radial direction is too large! Quitting!"<<G4endl;
    exit(2);
  }
  // Get materials
  G4Material* vacuumMaterial=G4Material::GetMaterial("Galactic");
  G4Material* tgtMaterial;
  if( targetMaterial == "tungsten" )
    tgtMaterial = G4Material::GetMaterial("G4_W");
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
  G4double worldSizeXYZ = 25 * m;  
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
  G4double tgtR = 5 * cm, tgtLen=15*cm;
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

  ////// Radial detectors
  G4double detectorThickness = 0.01 * mm;
  G4double spaceBetweenDet = 30*cm;
  G4VSolid *detOutS[50];
  G4LogicalVolume *detOutL[50];
  G4Colour  red(255/255.,0/255.,0/255.);
  G4VisAttributes* detVisAtt = new G4VisAttributes(red);
  detVisAtt->SetVisibility(true);
  for(G4int i=0;i<nrDet;i++){
    G4double innerR = tgtR + spaceBetweenDet*i;
    std::string solidNm = "detOutS_"+std::to_string(i);
    detOutS[i] = new G4Tubs(solidNm.c_str(),
			    innerR,innerR+detectorThickness,tgtLen/2.,
			    0*deg,360*deg);
    std::string logicNm = "detOutL_"+std::to_string(i);
    detOutL[i] = new G4LogicalVolume(detOutS[i],vacuumMaterial,logicNm.c_str());
    detOutL[i]->SetVisAttributes(detVisAtt);
    std::string physNm = "detOut_"+std::to_string(i);
    new G4PVPlacement(0,G4ThreeVector(),detOutL[i],physNm.c_str(),
		      worldLV,false,0,fCheckOverlaps);
  }

  ////// Upstream/Downstream detectors
  G4double spaceBetweenVertDet = 30*cm;
  G4double vertDetR = 30*cm;
  G4VSolid *detUS_S[50];
  G4LogicalVolume *detUS_L[50];
  G4RotationMatrix *rot1=new G4RotationMatrix(90*degree,0,0);
  for(G4int i=0;i<nrDet;i++){
    G4double zpos = - tgtLen/2. - spaceBetweenVertDet*(i+1);
    std::string solidNm = "detUS_S_"+std::to_string(i);
    detUS_S[i] = new G4Tubs(solidNm.c_str(),
			    0,vertDetR,detectorThickness,
			    0*deg,360*deg);
    
    std::string logicNm = "detUS_L_"+std::to_string(i);
    detUS_L[i] = new G4LogicalVolume(detUS_S[i],vacuumMaterial,logicNm.c_str());
    detUS_L[i]->SetVisAttributes(detVisAtt);
    std::string physNm = "detUpStream_"+std::to_string(i);
    new G4PVPlacement(rot1,G4ThreeVector(0,0,zpos),
		      detUS_L[i],physNm.c_str(),
		      worldLV,false,0,fCheckOverlaps);

    physNm = "detDnStream_"+std::to_string(i);
    new G4PVPlacement(rot1,G4ThreeVector(0,0,-zpos),
		      detUS_L[i],physNm.c_str(),
		      worldLV,false,0,fCheckOverlaps);
  }

  
  
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void radDetectorConstruction::DefineMaterials()
{ 

  G4NistManager* nistManager = G4NistManager::Instance();
  // G4Material* matPb = nistManager->FindOrBuildMaterial("G4_Pb");
  // G4Material* matW  = nistManager->FindOrBuildMaterial("G4_W");
  // G4Material* matCu = nistManager->FindOrBuildMaterial("G4_Cu");
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_W");
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

