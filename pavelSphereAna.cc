#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <fstream>

#include "TH1D.h"
#include "TH2D.h"
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
map <int,int> histNr;//<pType,histNr>
TH1D *hE[nHist],*hNEIL[nHist],*hMREM[nHist],*hElog[nHist];
const int nAverage=5000;
TH2D *hNEILneutron, *hMREMneutron;
TH1D *hAvgE[nHist],*hAvgNEIL[nHist],*hAvgMREM[nHist];
double avgE[nHist]={0,0,0,0,0,0};
double avgN[nHist]={0,0,0,0,0,0};
double avgM[nHist]={0,0,0,0,0,0};
int restrictAna(0);

radDamage radDmg;
void InitOutput();
void WriteOutput();
void Init(TTree *t);

long currentEv(0),prevEv(0),processedEv(0);
void processOne(string fnm);

int main(int argc,char** argv) {
  // radDamage calc;
  // cout<<calc.getMREM(2112,10,0)<<endl;
  // return 0;
  
  if(argc != 3){
    cout<<"Usage: build/pavelSphereAna [file with list of rootfiles] [0/1 -- restrict analysis region to 90 deg around z?]"<<endl;
    cout<<" for example: build/pavelSphereAna tungsten.lst 0"<<endl;
    return 1;
  }
  string inFile=argv[1];
  restrictAna = atoi(argv[2]);
  InitOutput();

  ifstream ifile(inFile.c_str());  
  string data;
  while(ifile>>data){
    processOne(data);
  }
  cout<<"Total of "<<processedEv<<endl;
  double gunEnergy = 11500;//[MeV]
  double Wfactor = gunEnergy * 1e-13*(1.60218);//[J/s]
  double areaDetector = 4 * acos(-1) * pow(300,2);//cm2
  if(restrictAna)
    areaDetector *= 0.1;
  double factorAll = (3600*3*1e4)/ ( areaDetector * Wfactor );
  cout<<"\tCompare to Pavel:\n"
      <<"\t\tNeutron: NEIL = "<< factorAll * hAvgNEIL[histNr[2112]]->GetMean()<<"\tpm\t"<< factorAll *  hAvgNEIL[histNr[2112]]->GetMeanError()<<"\t[NEIL/h/cm2]"<<endl
      <<"\t\tNeutron: MREM = "<< factorAll * hAvgMREM[histNr[2112]]->GetMean()<<"\tpm\t"<< factorAll * hAvgMREM[histNr[2112]]->GetMeanError()<<"\t[mrem/h/cm2]"<<endl;
  
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
    G4ThreeVector norm(prePosX,prePosY,prePosZ);

    if(restrictAna && abs(norm.getTheta()/(pi/2) -1) > 0.05) continue;
    
    G4ThreeVector mom(preMomX,preMomY,preMomZ);    
    G4double theta = mom.unit().angle(norm.unit());
    if( abs(theta/(pi/2) -1 )<0.01 ) {
      G4cout<<__LINE__<<"\t"<<__PRETTY_FUNCTION__
	    <<"\tProblem with theta "<<theta
	    <<"\n\t\t"<<mom
	    <<"\n\t\t"<<norm<<G4endl;
      theta=pi/2*1.01;	
    }

    int nrHist(-1);
    if( abs(pType) != 11 && abs(pType) != 211 && pType != 111 && pType != 2112 && pType != 2212 )
      nrHist=5;
    else
      nrHist=histNr[pType];

    hE[nrHist]->Fill(preKE);
    hElog[nrHist]->Fill(log10(preKE));
    avgE[nrHist] += preKE;

    double val(-1);
    val = radDmg.getNEIL(pType,preKE,theta);
    if(val!=-999){
      hNEIL[nrHist]->Fill(val);
      avgN[nrHist] += val;
      if(pType==2112)
	hNEILneutron->Fill(norm.getPhi()*180/pi,norm.getTheta()*180/pi,val);
    }

    val = radDmg.getMREM(pType,preKE,theta);
    if(val!=-999){
      hMREM[nrHist]->Fill(val);
      avgM[nrHist] += val;
      if(pType==2112){
	hMREMneutron->Fill(norm.getPhi()*180/pi,norm.getTheta()*180/pi,val);
      }
    }
  }

  processedEv += ceil(prevEv/1000.)*1000;
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
  histNr[11]=0;
  histNr[-11]=0;
  histNr[2112]=1;
  histNr[211]=2;
  histNr[-211]=2;
  histNr[111]=2;
  histNr[22]=3;
  histNr[2122]=4;

  fout=new TFile("o_psAna.root","RECREATE");
  string pNm[nHist]={"ep","n","pi","g","pr","o"};
  int nBin=400;
  int elBin[nHist]={ 0 ,  0, 0, 0,  0, 0};
  int ehBin[nHist]={50 ,700,100,10,700,10};
  int nhBin[nHist]={ 1 , 50,100,10,100,10};
  int mhBin[nHist]={10000,10000,10,10, 10,10};
  hNEILneutron = new TH2D("hNEILneutron","NEIL value;#phi [deg]; #theta [deg];",
			  360,-180,180,
			  180,0,180);
  hMREMneutron = new TH2D("hMREMneutron","MREM value;#phi [deg]; #theta [deg];",
			  360,-180,180,
			  180,0,180);
  for(int i=0;i<nHist;i++){
    hE[i]=new TH1D(Form("hE_%s",pNm[i].c_str()),";Energy [MeV]",
		   nBin,elBin[i],ehBin[i]);
    hElog[i]=new TH1D(Form("hElog_%s",pNm[i].c_str()),";log10(Energy/MeV)",
		      nBin,-7,3.5);
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
  hNEILneutron->Write();
  hMREMneutron->Write();
  for(int i=0;i<nHist;i++){
    hE[i]->Write();
    hElog[i]->Write();
    hNEIL[i]->Write();
    hMREM[i]->Write();
    hAvgE[i]->Write();
    hAvgNEIL[i]->Write();
    hAvgMREM[i]->Write();    
  }
  fout->Close();
  delete fout;
}
