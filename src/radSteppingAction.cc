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
  
  tout->Branch("postE",&postE,"postE/D");
  tout->Branch("postKE",&postKE,"postKE/D");
  
  tout->Branch("prePosX", &prePosX, "prePosX/D");
  tout->Branch("prePosY", &prePosY, "prePosY/D");
  tout->Branch("prePosZ", &prePosZ, "prePosZ/D");
  tout->Branch("preMomX", &preMomX, "preMomX/D");
  tout->Branch("preMomY", &preMomY, "preMomY/D");
  tout->Branch("preMomZ", &preMomZ, "preMomZ/D");

  tout->Branch("neilVal", &neilVal, "neilVal/D");
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
  G4ThreeVector polarization=theTrack->GetPolarization();
  G4String processNm = thePostPoint->GetProcessDefinedStep()->GetProcessName();

  InitVar();  
  
  eventNr = *evNr;
  if(currentEv!=eventNr){
    currentEv=eventNr;
  }

  int fillTree=0;
  
  if(theMaterial){    
    if(theMaterial->GetName().compare("Galactic")==0){
      fillTree=1;
      material=1;
    } else if(theMaterial->GetName().compare("G4_W")==0 ||
	    theMaterial->GetName().compare("G4_Cu")==0 ||
	    theMaterial->GetName().compare("matFe")==0 ||
	    theMaterial->GetName().compare("matW")==0 ||
	    theMaterial->GetName().compare("G4_Pb")==0){
      material=0;
      fillTree=0;
    }else fillTree=0;

    std::string volNm = thePrePoint->GetTouchableHandle()->GetVolume()->GetName();
    if(volNm.compare("radialDetector")==0)
      detID = 0;
    
    pType = particleType->GetPDGEncoding();
    if( abs(pType) != 11 && abs(pType) != 211 && pType != 111 && pType != 2112 && pType != 2212 )
      fillTree=0;
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

    int nDmg(-1);
    if(pType == 11 || pType == -11) nDmg=3;
    else if(pType == 2112) nDmg=0;
    else if(pType == 2212) nDmg=1;
    else if(pType == 211 || pType == -211 || pType == 111) nDmg=2; 

    if(nDmg!=-1)
      neilVal = neutronEquiv.getNEIL(nDmg,preE);

    G4ThreeVector globalMomentum = thePrePoint->GetMomentum();
    G4ThreeVector localMomentum = thePrePoint->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(globalMomentum);
    normCosAng = cos(localMomentum.getTheta());
  }

  if(fillTree){
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

  normCosAng = -999;
}



