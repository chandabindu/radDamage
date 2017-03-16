#include "mscMessenger.hh"

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "mscDetectorConstruction.hh"
#include "mscEventAction.hh"
#include "mscPrimaryGeneratorAction.hh"
#include "mscSteppingAction.hh"

#include "G4UImanager.hh"
#include "G4RunManager.hh"

#include <iostream>

mscMessenger::mscMessenger(){
    /*  Initialize all the things it talks to to NULL */

    fDetCon       = NULL;
    fEvAct        = NULL;
    fPriGen       = NULL;
    fStepAct      = NULL;

    nrDetCmd = new G4UIcmdWithAnInteger("/msc/det/setNrRadialDetectors",this);
    nrDetCmd->SetGuidance("Set number of detectors in radial direction");
    nrDetCmd->SetParameterName("nrDet", false);

    gunEnergyCmd = new G4UIcmdWithADoubleAndUnit("/msc/gun/setGunEnergy",this);
    gunEnergyCmd->SetParameterName("gunEnergy", false);

    tgtMatCmd = new G4UIcmdWithAString("/msc/det/setTargetMaterial",this);
    tgtMatCmd->SetParameterName("tgtMat", false);

    outNmCmd = new G4UIcmdWithAString("/msc/step/setOutFileName",this);
    outNmCmd->SetParameterName("outNm", false);

    tgtLenCmd = new G4UIcmdWithADoubleAndUnit("/msc/det/setTargetLength",this);
    tgtLenCmd->SetParameterName("tgtLength", false);

    updateGeoCmd = new G4UIcmdWithoutParameter("/msc/det/updateGeometry",this);
    updateGeoCmd->AvailableForStates(G4State_Idle);
}

mscMessenger::~mscMessenger(){
  if(nrDetCmd) delete nrDetCmd;
  if(gunEnergyCmd) delete gunEnergyCmd;
  if(tgtMatCmd) delete tgtMatCmd;
  if(outNmCmd) delete outNmCmd;
  if(tgtLenCmd) delete tgtLenCmd;
  if(updateGeoCmd) delete updateGeoCmd;
}


void mscMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue){

  if( cmd == nrDetCmd ){
    G4int val = nrDetCmd->GetNewIntValue(newValue);
    fDetCon -> SetNrDetectors( val );

  }else if( cmd == gunEnergyCmd ){
    G4double val = gunEnergyCmd->GetNewDoubleValue(newValue);
    fPriGen->SetGunEnergy( val );

  }else if( cmd == tgtMatCmd ){
    fDetCon->SetTargetMaterial( newValue );

  }else if( cmd == outNmCmd ){
    fStepAct->SetOutputFileName( newValue );
    fStepAct->InitOutput();

  }else if( cmd == updateGeoCmd ){
    fDetCon -> UpdateGeometry();

  }else if( cmd == tgtLenCmd ){
    G4double val = tgtLenCmd->GetNewDoubleValue(newValue);
    fDetCon -> SetTargetLength(val);

  }else{
    G4cerr << __PRETTY_FUNCTION__
	   <<" what command are you talking about? "<<cmd<<" "<<newValue<<G4endl;
    exit(3);
  }

}
