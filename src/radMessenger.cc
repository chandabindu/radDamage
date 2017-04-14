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

    gunPosXCmd = new G4UIcmdWithADoubleAndUnit("/rad/gun/setGunPosX",this);
    gunPosXCmd->SetParameterName("gunPosX", false);

    gunPosYCmd = new G4UIcmdWithADoubleAndUnit("/rad/gun/setGunPosY",this);
    gunPosYCmd->SetParameterName("gunPosY", false);

    gunPosZCmd = new G4UIcmdWithADoubleAndUnit("/rad/gun/setGunPosZ",this);
    gunPosZCmd->SetParameterName("gunPosZ", false);

    tgtMatCmd = new G4UIcmdWithAString("/rad/det/setTargetMaterial",this);
    tgtMatCmd->SetParameterName("tgtMat", false);

    updateGeoCmd = new G4UIcmdWithoutParameter("/rad/det/updateGeometry",this);    
}

radMessenger::~radMessenger(){
  if(nrDetCmd) delete nrDetCmd;
  if(gunEnergyCmd) delete gunEnergyCmd;
  if(gunPosXCmd) delete gunPosXCmd;
  if(gunPosYCmd) delete gunPosYCmd;
  if(gunPosZCmd) delete gunPosZCmd;
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
  }else if( cmd == gunPosXCmd ){
    G4double val = gunPosXCmd->GetNewDoubleValue(newValue);
    fPriGen->SetGunPosX( val );
  }else if( cmd == gunPosYCmd ){
    G4double val = gunPosYCmd->GetNewDoubleValue(newValue);
    fPriGen->SetGunPosY( val );
  }else if( cmd == gunPosZCmd ){
    G4double val = gunPosZCmd->GetNewDoubleValue(newValue);
    fPriGen->SetGunPosZ( val );
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
