#include "mscSteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include <fstream>

mscSteppingAction::mscSteppingAction(G4int *evN)
{
  //eventID pointer from the mscEventAction.cc file
  evNr=evN;
  InitOutput();
}

void  mscSteppingAction::InitOutput(){

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
  
  tout->Branch("postPosX", &postPosX, "postPosX/D");
  tout->Branch("postPosY", &postPosY, "postPosY/D");
  tout->Branch("postPosZ", &postPosZ, "postPosZ/D");
  tout->Branch("postMomX", &postMomX, "postMomX/D");
  tout->Branch("postMomY", &postMomY, "postMomY/D");
  tout->Branch("postMomZ", &postMomZ, "postMomZ/D");
  
}


mscSteppingAction::~mscSteppingAction()
{
  /*Write out root file*/
  fout->cd();
  tout->Write();
  fout->Close();
}

void mscSteppingAction::UserSteppingAction(const G4Step* theStep)
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
  
    postE  =  thePostPoint->GetTotalEnergy();
    postKE = thePostPoint->GetKineticEnergy();

    postPosX  =  thePostPoint->GetPosition().getX();
    postPosY  =  thePostPoint->GetPosition().getY();
    postPosZ  =  thePostPoint->GetPosition().getZ();
    postMomX  =  thePostPoint->GetMomentum().getX();
    postMomY  =  thePostPoint->GetMomentum().getY();
    postMomZ  =  thePostPoint->GetMomentum().getZ();
    
  }

  if(fillTree){
    tout->Fill();
  }
}

void mscSteppingAction::InitVar(){
  eventNr = -999;
  material = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;
  detID = -999;
  
  postE  = -999;
  postKE = -999;

  postPosX  = -999;
  postPosY  = -999;
  postPosZ  = -999;
  postMomX  = -999;
  postMomY  = -999;
  postMomZ  = -999;
}



