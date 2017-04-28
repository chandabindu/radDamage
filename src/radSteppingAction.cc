#include "radSteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include <fstream>

radSteppingAction::radSteppingAction(G4int *evN)
{
  //eventID pointer from the radEventAction.cc file
  evNr=evN;
  InitOutput();
}

void  radSteppingAction::InitOutput(){

  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_radTree.root","RECREATE");

  tout=new TTree("t","Stepping action event tree");
    
  tout->Branch("evNr",&eventNr,"evNr/I");
  tout->Branch("material",&material,"material/I");
  tout->Branch("pType",&pType,"pType/I");
  tout->Branch("trackID",&trackID,"trackID/I");
  tout->Branch("parentID",&parentID,"parentID/I");
  tout->Branch("detID",&detID,"detID/I");
  
  tout->Branch("preE",&preE,"preE/D");
  tout->Branch("preKE",&preKE,"preKE/D");
  
  tout->Branch("prePosX", &prePosX, "prePosX/D");
  tout->Branch("prePosY", &prePosY, "prePosY/D");
  tout->Branch("prePosZ", &prePosZ, "prePosZ/D");
  tout->Branch("preMomX", &preMomX, "preMomX/D");
  tout->Branch("preMomY", &preMomY, "preMomY/D");
  tout->Branch("preMomZ", &preMomZ, "preMomZ/D");

  tout->Branch("neilVal", &neilVal, "neilVal/D");
  tout->Branch("mremVal", &mremVal, "mremVal/D");
  tout->Branch("normCosAng", &normCosAng, "normCosAng/D");
  
}


radSteppingAction::~radSteppingAction()
{
  /*Write out root file*/
  fout->cd();
  tout->Write();
  fout->Close();
}


void radSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track*              theTrack     = theStep->GetTrack();
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  G4StepPoint*          thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*          thePostPoint = theStep->GetPostStepPoint();
  G4String              particleName = theTrack->GetDefinition()->GetParticleName();

  //get material
  G4Material* theMaterial = theTrack->GetMaterial();
  G4ThreeVector _polarization=theTrack->GetPolarization();
  G4String _pn=thePostPoint->GetProcessDefinedStep()->GetProcessName();

  InitVar();  
  
  eventNr=*evNr;
  if(currentEv!=eventNr){
    currentEv=eventNr;
  }

  int fillTree=0;
  
  if(theMaterial){    
    if(theMaterial->GetName().compare("Galactic")==0){
      material=1;
      fillTree=1;
    }else if(theMaterial->GetName().compare("G4_W")==0 ||
	    theMaterial->GetName().compare("G4_Cu")==0 ||
	    theMaterial->GetName().compare("G4_Pb")==0){
      material=0;
      fillTree=0;
    }else fillTree=0;

    std::string volNm = thePrePoint->GetTouchableHandle()->GetVolume()->GetName();
    if(volNm.compare(0,6,"detOut")==0){
      detID = 200 + std::atoi(volNm.substr(7).c_str());
    }else if(volNm.compare(0,11,"detUpStream")==0){
      detID = 100 + std::atoi(volNm.substr(12).c_str());
    }else if(volNm.compare(0,11,"detDnStream")==0){
      detID = 300 + std::atoi(volNm.substr(12).c_str());
    }
    
    pType = particleType->GetPDGEncoding();    
    trackID = theStep->GetTrack()->GetTrackID();
    parentID = theStep->GetTrack()->GetParentID();
  
    preE  =  thePrePoint->GetTotalEnergy();
    preKE = thePostPoint->GetKineticEnergy();

    prePosX  =  thePrePoint->GetPosition().getX();
    prePosY  =  thePrePoint->GetPosition().getY();
    prePosZ  =  thePrePoint->GetPosition().getZ();
    preMomX  =  thePrePoint->GetMomentum().getX();
    preMomY  =  thePrePoint->GetMomentum().getY();
    preMomZ  =  thePrePoint->GetMomentum().getZ();

    G4ThreeVector mom(preMomX,preMomY,preMomZ);
    G4bool valid(false);
    G4double theta(0);
    if(material==1){
      G4ThreeVector norm = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetGlobalExitNormal(thePostPoint->GetPosition(),&valid);      
      theta = norm.angle(mom.unit());
    }
    if(valid)
      normCosAng = cos(theta);
    
    neilVal = dmgCalc.getNEIL(pType,preKE,theta);
    mremVal = dmgCalc.getMREM(pType,preKE,theta);
  }

  if(fillTree){
    // G4cout<<" currentEv "<<eventNr
    // 	  <<"  "<<theMaterial->GetName()<<" "<<particleType->GetPDGEncoding()
    // 	  <<" "<<thePrePoint->GetTouchableHandle()->GetVolume()->GetName()
    // 	  <<" "<<material<<G4endl;    
    // G4cout<<detID<<G4endl;
    tout->Fill();
  }
  
}

void radSteppingAction::InitVar(){
  eventNr = -999;
  material = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;
  detID = -999;
  
  preE  = -999;
  preKE = -999;

  prePosX  = -999;
  prePosY  = -999;
  prePosZ  = -999;
  preMomX  = -999;
  preMomY  = -999;
  preMomZ  = -999;

  neilVal  = -999;
  mremVal  = -999;

  normCosAng = -999;
}



