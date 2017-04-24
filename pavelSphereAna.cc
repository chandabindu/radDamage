#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <fstream>

#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"

#include "radDamage.hh"
#include "G4ThreeVector.hh"

using namespace std;

G4int evNr, mat, pType, tID, pID, detID;
G4double preE, preKE, prePosX, prePosY, prePosZ, preMomX;
G4double preMomY, preMomZ, neilVal, mremVal, normCosAng;
TFile *fout;
const int nHist=6;
//[6]== electron/positron, neutron, pion, gamma, proton, other
TH1D *hE[nHist],*hNEIL[nHist],*hMREM[nHist];
const int nAverage=5000;
TH1D *hAvgE[nHist],*hAvgNEIL[nHist],*hAvgMREM[nHist];
double avgE[nHist]={0,0,0,0,0,0};
double avgN[nHist]={0,0,0,0,0,0};
double avgM[nHist]={0,0,0,0,0,0};

radDamage radDmg;
void InitOutput();
void WriteOutput();
void Init(TTree *t);

long currentEv(0),prevEv(0),processedEv(0);
void processOne(string fnm);

int main(int argc,char** argv) {
  
  if(argc != 2){
    cout<<"Usage: build/pavelSphereAna [file with list of rootfiles]"<<endl;
    cout<<" for example: build/pavelSphereAna tungsten.lst"<<endl;
    return 1;
  }
  string inFile=argv[1];
  InitOutput();

  ifstream ifile(inFile.c_str());  
  string data;
  while(ifile>>data){
    processOne(data);
  }
  cout<<"Total of "<<processedEv<<endl;
  WriteOutput();
  
  return 0; 
}
 
void processOne(string fnm){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  Init(t);

  const G4double pi=acos(-1);
  long nEntries=t->GetEntries();
  for(long i=0;i<nEntries;i++){
    t->GetEntry(i);

    currentEv += evNr - prevEv;
    prevEv = evNr;
    
    if( currentEv > nAverage ){
      for(int j=0;j<nHist;j++){
	hAvgE[j]   ->Fill(avgE[j]/nAverage);
	hAvgNEIL[j]->Fill(avgN[j]/nAverage);
	hAvgMREM[j]->Fill(avgM[j]/nAverage);
	avgE[j]=0;
	avgN[j]=0;
	avgM[j]=0;
      }
      currentEv=currentEv - nAverage;
    }

    if(mat!=1) continue;
    G4ThreeVector mom(preMomX,preMomY,preMomZ);
    G4ThreeVector norm(prePosX,prePosY,prePosZ);
    G4double theta = mom.unit().angle(norm.unit());
    if( abs(theta/(pi/2) -1 )<0.01 ) {
      G4cout<<__LINE__<<"\t"<<__PRETTY_FUNCTION__
	    <<"\tProblem with theta "<<theta
	    <<"\n\t\t"<<mom
	    <<"\n\t\t"<<norm<<G4endl;
      theta=pi/2*1.01;	
    }
    G4double val;
    if(abs(pType)==11){
      hE[0]->Fill(preKE);
      avgE[0]+=preKE;

      val = radDmg.getNEIL(3,preKE)/abs(cos(theta));
      hNEIL[0]->Fill(val);
      avgN[0]+=val;

      val = radDmg.getMREM(2,preKE,theta);
      hMREM[0]->Fill(val);
      avgM[0]+=val;
    }else if(pType==2112){//neutrons
      hE[1]->Fill(preKE);
      avgE[1]+=preKE;

      val = radDmg.getNEIL(0,preKE)/abs(cos(theta));
      hNEIL[1]->Fill(val);
      avgN[1]+=val;

      val = radDmg.getMREM(0,preKE,theta);
      hMREM[1]->Fill(val);
      avgM[1]+=val;
    }else if(pType==2212){//protons
      hE[4]->Fill(preKE);
      avgE[4]+=preKE;

      val = radDmg.getNEIL(1,preKE)/abs(cos(theta));
      hNEIL[4]->Fill(val);
      avgN[4]+=val;
    }else if(abs(pType)==22){//photons
      hE[3]->Fill(preKE);
      avgE[3]+=preKE;

      val = radDmg.getMREM(2,preKE,theta);
      hMREM[3]->Fill(val);
      avgM[3]+=val;
    }else if(pType == 211 || pType == -211 || pType == 111){//pions
      hE[2]->Fill(preKE);
      avgE[2]+=preKE;

      val = radDmg.getNEIL(2,preKE)/abs(cos(theta));
      hNEIL[2]->Fill(val);
      avgN[2]+=val;
    }else {
      hE[5] -> Fill(preKE);
      avgE[5] +=preKE;
    }
  }

  processedEv += ceil(prevEv/1000.)*1000;
  // cout<<processedEv<<" "<<prevEv<<endl;
  // cin.ignore();
  prevEv = 0;
  fin->Close();
  delete fin;
}
void Init(TTree *t){      
  t->SetBranchAddress("evNr",&evNr);
  t->SetBranchAddress("material",&mat);
  t->SetBranchAddress("pType",&pType);
  t->SetBranchAddress("trackID",&tID);
  t->SetBranchAddress("parentID",&pID);
  t->SetBranchAddress("detID",&detID);  
  t->SetBranchAddress("preE",&preE);
  t->SetBranchAddress("preKE",&preKE);  
  t->SetBranchAddress("prePosX", &prePosX);
  t->SetBranchAddress("prePosY", &prePosY);
  t->SetBranchAddress("prePosZ", &prePosZ);
  t->SetBranchAddress("preMomX", &preMomX);
  t->SetBranchAddress("preMomY", &preMomY);
  t->SetBranchAddress("preMomZ", &preMomZ);
  t->SetBranchAddress("neilVal", &neilVal);
  t->SetBranchAddress("mremVal", &mremVal);
  t->SetBranchAddress("normCosAng", &normCosAng);
}

void InitOutput(){
  fout=new TFile("o_psAna.root","RECREATE");
  string pNm[nHist]={"ep","n","pi","g","pr","o"};
  int nBin=400;
  int elBin[nHist]={ 0 ,  0, 0, 0,  0, 0};
  int ehBin[nHist]={50 ,700,100,10,700,10};
  int nhBin[nHist]={ 1 , 50,100,10,100,10};
  int mhBin[nHist]={10000,10000,10,10, 10,10};
  for(int i=0;i<nHist;i++){
    hE[i]=new TH1D(Form("hE_%s",pNm[i].c_str()),";Energy [MeV]",
		   nBin,elBin[i],ehBin[i]);
    hNEIL[i]=new TH1D(Form("hNEIL_%s",pNm[i].c_str()),";1MeV neutron equivalent",
		      nBin,elBin[i],nhBin[i]);
    hMREM[i]=new TH1D(Form("hMREM_%s",pNm[i].c_str()),";dose [mrem]",
		      nBin,elBin[i],mhBin[i]);

    hAvgE[i]=new TH1D(Form("hAvgE_%s",pNm[i].c_str()),
		      Form("Average over %d events;Energy [MeV]",nAverage),
		      nBin,elBin[i],ehBin[i]/1000);
    hAvgNEIL[i]=new TH1D(Form("hAvgNEIL_%s",pNm[i].c_str()),
			 Form("Average over %d events;1MeV neutron equivalent",nAverage),
			 nBin,elBin[i],nhBin[i]/1000);
    int divide=1000;
    if(i==1)
      divide=100;
    hAvgMREM[i]=new TH1D(Form("hAvgMREM_%s",pNm[i].c_str()),
			 Form("Average over %d events;dose [mrem]",nAverage),
			 nBin,elBin[i],mhBin[i]/divide);
  }
}

void WriteOutput(){
  fout->cd();
  for(int i=0;i<nHist;i++){
    hE[i]->Write();
    hNEIL[i]->Write();
    hMREM[i]->Write();
    hAvgE[i]->Write();
    hAvgNEIL[i]->Write();
    hAvgMREM[i]->Write();    
  }
  fout->Close();
  delete fout;
}
