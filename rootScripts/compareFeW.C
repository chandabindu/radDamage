void compareFeW(){
  TFile *fi=TFile::Open("o_psAna_iron_242e5Ev.root","READ");
  TFile *ft=TFile::Open("o_psAna_tungsten_242e5Ev.root","READ");

  gStyle->SetOptStat("neMRoui");


  TCanvas *c1=new TCanvas("c1","c1");
  TH1D *ihE_n=(TH1D*)fi->Get("hE_n");
  TH1D *thE_n=(TH1D*)ft->Get("hE_n");
  ihE_n->SetLineColor(2);
  ihE_n->DrawCopy();
  thE_n->DrawCopy("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  gPad->SetLogy(1);
  
  TCanvas *c2=new TCanvas("c2","c2");
  TH1D *ihE_e=(TH1D*)fi->Get("hE_ep");
  TH1D *thE_e=(TH1D*)ft->Get("hE_ep");
  ihE_e->SetLineColor(2);
  ihE_e->DrawCopy();
  thE_e->DrawCopy("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  gPad->SetLogy(1);

  // TH1D *ihNEIL_n=(TH1D*)fi->Get("hNEIL_n");
  // TH1D *thNEIL_n=(TH1D*)ft->Get("hNEIL_n");


  TCanvas *c3=new TCanvas("c3","c3");
  TH1D *ihMREM_n=(TH1D*)fi->Get("hMREM_n");
  TH1D *thMREM_n=(TH1D*)ft->Get("hMREM_n");
  ihMREM_n->SetLineColor(2);
  ihMREM_n->DrawCopy();
  thMREM_n->DrawCopy("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  gPad->SetLogy(1);
  
  TCanvas *c4=new TCanvas("c4","c4");
  TH1D *iahMREM_n=(TH1D*)fi->Get("hAvgMREM_n");
  TH1D *tahMREM_n=(TH1D*)ft->Get("hAvgMREM_n");
  iahMREM_n->SetLineColor(2);
  iahMREM_n->GetYaxis()->SetRangeUser(0,1000);
  iahMREM_n->DrawCopy();
  tahMREM_n->DrawCopy("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  //gPad->SetLogy(1);

  TCanvas *c5=new TCanvas("c5","c5");
  TH1D *iahMREM_e=(TH1D*)fi->Get("hAvgMREM_ep");
  TH1D *tahMREM_e=(TH1D*)ft->Get("hAvgMREM_ep");
  iahMREM_e->SetLineColor(2);
  iahMREM_e->DrawCopy();
  tahMREM_e->DrawCopy("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  gPad->SetLogy(1);

}
