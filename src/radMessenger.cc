#include "radMessenger.hh"

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "radDetectorConstruction.hh"
#include "radEventAction.hh"
#include "radPrimaryGeneratorAction.hh"
#include "radSteppingAction.hh"

#include "G4UImanager.hh"
#include "G4RunManager.hh"

#include <iostream>

radMessenger::radMessenger(){
    /*  Initialize all the things it talks to to NULL */

    fDetCon       = NULL;
    fEvAct        = NULL;
    fPriGen       = NULL;
    fStepAct      = NULL;

    nrDetCmd = new G4UIcmdWithAnInteger("/rad/det/setNrDetectors",this);
    nrDetCmd->SetGuidance("Set number of detectors");
    nrDetCmd->SetParameterName("nrDet", false);

    gunEnergyCmd = new G4UIcmdWithADoubleAndUnit("/rad/gun/setGunEnergy",this);
    gunEnergyCmd->SetParameterName("gunEnergy", false);

    tgtMatCmd = new G4UIcmdWithAString("/rad/det/setTargetMaterial",this);
    tgtMatCmd->SetParameterName("tgtMat", false);

    updateGeoCmd = new G4UIcmdWithoutParameter("/rad/det/updateGeometry",this);    
}

radMessenger::~radMessenger(){
  if(nrDetCmd) delete nrDetCmd;
  if(gunEnergyCmd) delete gunEnergyCmd;
  if(tgtMatCmd) delete tgtMatCmd;
  if(updateGeoCmd) delete updateGeoCmd;
}


void radMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue){

  if( cmd == nrDetCmd ){
    G4int val = nrDetCmd->GetNewIntValue(newValue);
    fDetCon -> SetNrDetectors( val );
  }else if( cmd == gunEnergyCmd ){
    G4double val = gunEnergyCmd->GetNewDoubleValue(newValue);
    fPriGen->SetGunEnergy( val );
  }else if( cmd == tgtMatCmd ){
    fDetCon->SetTargetMaterial( newValue );
  }else if( cmd == updateGeoCmd ){
    fDetCon->UpdateGeometry();
  }else{
    G4cerr << __PRETTY_FUNCTION__
	   <<" what command are you talking about? "<<cmd<<" "<<newValue<<G4endl;
    exit(3);
  }

}
