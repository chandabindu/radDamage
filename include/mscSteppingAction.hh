#ifndef mscSteppingAction_hh
#define mscSteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TH3I.h"
#include "TTree.h"
#include "TGraph.h"


class mscSteppingAction : public G4UserSteppingAction
{
public:
  mscSteppingAction(G4int*);
  ~mscSteppingAction();
  
  void UserSteppingAction(const G4Step*);
  void InitVar();
  void InitOutput();
    
private:
  G4int *evNr;
  TFile *fout;
  TTree *tout;

  G4int currentEv;
  
  //tree variables
  G4int eventNr;
  G4int material; //detector 0, tgt 1
  G4int pType;    
  G4int trackID;
  G4int parentID;
  
  G4int detID;
  
  G4double  postE; // total energy
  G4double  postKE;// kinetic energy
  
  G4double  postPosX; //position
  G4double  postPosY;
  G4double  postPosZ;
  G4double  postMomX; //momentum
  G4double  postMomY;
  G4double  postMomZ;  

};

#endif
