#include "TGraphAsymmErrors.h"
#include "THStack.h"
#include "TH1F.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

//double lumi=9200.;
//double lumi=15000.;
double lumi=40000.;

template <typename ntupleType> class plot{

 public: 

  plot( double (*fillerFunc_)(ntupleType*) , TString label_="var"){
    fillerFunc = fillerFunc_;
    label = label_;
    xlabel = "" ; 
    nbins = 40; lower = 200.; upper = 1200.;
    binEdges = NULL;
    stack=new THStack(label+"_stack",label+"_stack");
    dataHist=NULL;
  };

  plot( double (*fillerFunc_)(ntupleType*) , 
	TString label_="var",TString xlabel_="var",
	int nbins_=20 , double lower_=200. , double upper_=2200.){
    fillerFunc = fillerFunc_;
    label = label_;
    xlabel =xlabel_ ; 
    nbins = nbins_; lower = lower_; upper = upper_;
    binEdges=NULL;
    stack=new THStack(label+"_stack",label+"_stack");
  };

  plot( double (*fillerFunc_)(ntupleType*) , 
	TString label_,TString xlabel_,
	int nbins_ , double* bins_){
    fillerFunc = fillerFunc_;
    label = label_;
    xlabel =xlabel_ ; 
    binEdges = bins_;
    nbins = nbins_; lower = binEdges[0]; upper = binEdges[nbins];
    stack=new THStack(label+"_stack",label+"_stack");
  };
  
  void setBinning(int nbins_ , double lower_ , double upper_ ){
    nbins = nbins_ ; lower = lower_ ; upper = upper_ ;
  };
  
  void addNtuple(ntupleType* ntuple_,TString tag="test"){ 
    //cout << "nbins: " << nbins << " lower: " << lower << " upper: " << upper << endl;
    tagMap[ntuple_] = tag;
    if( binEdges )
      histoMap[ntuple_] = new TH1F(label+"_"+tag,label+"_"+tag,nbins,binEdges);
    else
      histoMap[ntuple_] = new TH1F(label+"_"+tag,label+"_"+tag,nbins,lower,upper);
    histoMap[ntuple_]->Sumw2();
  };

  void addSignalNtuple(ntupleType* ntuple_,TString tag="test"){ 
    //cout << "nbins: " << nbins << " lower: " << lower << " upper: " << upper << endl;
    tagMap[ntuple_] = tag;
    if( binEdges )
      signalHistoMap[ntuple_] = new TH1F(label+"_"+tag,label+"_"+tag,nbins,binEdges);   
    else
      signalHistoMap[ntuple_] = new TH1F(label+"_"+tag,label+"_"+tag,nbins,lower,upper);

    signalHistoMap[ntuple_]->Sumw2();
  };
  
  void addDataNtuple(ntupleType* ntuple_,TString tag="test"){
    //cout << "nbins: " << nbins << " lower: " << lower << " upper: " << upper << endl;
    tagMap[ntuple_] = tag ;
    if( binEdges )
      dataHist = new TH1F(label+"_"+tag,label+"_"+tag,nbins,binEdges);
    else
      dataHist = new TH1F(label+"_"+tag,label+"_"+tag,nbins,lower,upper);
    dataHist->SetMarkerStyle(8);
  };
  
  int fill(ntupleType* ntuple ){
    if( histoMap.find(ntuple) != histoMap.end() ){
      return histoMap[ntuple]->Fill(fillerFunc(ntuple),ntuple->Weight*lumi);
    }else{
      cout << "plot::fill - ERROR: key not found: " << ntuple << endl;
      return 0;
    }
  };

  int fillSignal(ntupleType* ntuple ){
    if( signalHistoMap.find(ntuple) != signalHistoMap.end() ){
      return signalHistoMap[ntuple]->Fill(fillerFunc(ntuple),ntuple->Weight*lumi);
    }else{
      cout << "plot::fillSignal - ERROR: key not found, " << ntuple << endl;
      return 0;
    }
  };

  int fill(ntupleType* ntuple , float customWeight ){
    if( histoMap.find(ntuple) != histoMap.end() ){
      return histoMap[ntuple]->Fill(fillerFunc(ntuple),customWeight);
    }else{
      cout << "plot::fill - ERROR: key not found: " << ntuple << endl;
      return 0;
    }
  };

  int fillSignal(ntupleType* ntuple , float customWeight ){
    if( signalHistoMap.find(ntuple) != signalHistoMap.end() ){
      return signalHistoMap[ntuple]->Fill(fillerFunc(ntuple),customWeight);
    }else{
      cout << "plot::fillSignal - ERROR: key not found, " << ntuple << endl;
      return 0;
    }
  };

  int fillData(ntupleType* ntuple ){
    if( dataHist )
      return dataHist->Fill(fillerFunc(ntuple));
    else 
      return 0;
  };
  
  void setFillColor(ntupleType* ntuple , int color=1){
    if( histoMap.find(ntuple) != histoMap.end() ){
      histoMap[ntuple]->SetFillColor(color);
      histoMap[ntuple]->SetLineWidth(2);
    }else{
      cout << "plot::setFillColor - ERROR: couldn't find key, " << ntuple << endl;
    }
  };

  void setLineColor(ntupleType* ntuple , int color=1){
    if( signalHistoMap.find(ntuple) != signalHistoMap.end() ){
      signalHistoMap[ntuple]->SetLineColor(color);
      signalHistoMap[ntuple]->SetLineWidth(2);
    }else{
      cout << "plot::setFillColor - ERROR: couldn't find key, " << ntuple << endl;
    }
  };
  
  void buildStack(vector<ntupleType*> ntuples){
    if( stack ){
      for( int iSample = 0 ; iSample < ntuples.size() ; iSample++ ){
	stack->Add(histoMap[ntuples[iSample]]);
      }    
    }
  };

  void buildStack(){
    
    vector<ntupleType*> ntuples;
    for( typename map<ntupleType*,TH1F*>::iterator it = histoMap.begin() ;
	 it != histoMap.end() ; 
	 ++it){
      ntuples.push_back(it->first);
    }
    buildStack(ntuples);
  };

  void buildSum(){
    sum = NULL;
    for( typename map<ntupleType*,TH1F*>::iterator it = histoMap.begin() ;
	 it != histoMap.end() ; 
	 ++it){
      if( sum == NULL ){
	sum = new TH1F(*(it->second));
	sum->SetNameTitle(label+"_sum",label+"_sum");
      }else
	sum->Add(it->second);
    }
  };

  void Write(){

    for( typename map<ntupleType*,TH1F*>::iterator it = histoMap.begin() ;
	 it != histoMap.end() ; 
	 ++it ){
      if( it->second ) 
	it->second->Write();
    }

    for( typename map<ntupleType*,TH1F*>::iterator it = signalHistoMap.begin() ;
	 it != signalHistoMap.end() ; 
	 ++it ){
      if( it->second ) 
	it->second->Write();
    }
    if( dataHist )
      dataHist->Write();

  };

  void Draw(TCanvas* can,
	    vector<ntupleType*>ntuples,
	    vector<ntupleType*>signalNtuples,
	    TString dir = "./" ){

    if( ! can ) return ;
    
    TPad* topPad = new TPad("topPad","topPad",0.,0.4,.99,.99);
    TPad* botPad = new TPad("botPad","botPad",0.,0.01,.99,.39);
    topPad->Draw();
    botPad->Draw();
    topPad->cd();
    
    double max = 0 ;
    if( histoMap.size() ){
      buildStack(ntuples);
      stack->Draw("histo");
      cout << "xlabel: " << xlabel << endl;
      stack->GetXaxis()->SetTitle(xlabel);
      stack->GetXaxis()->SetNdivisions(505);
      stack->GetYaxis()->SetTitle("Events");
      buildSum();
      max = sum->GetMaximum(); 
    }
    
    for(int iSample = 0 ; iSample < signalNtuples.size() ; iSample++){
      TH1F* temp = signalHistoMap[signalNtuples[iSample]];
      if( temp ){
	//temp->Scale(sum->Integral()/temp->Integral());
	temp->Draw("histo,SAME");
	if( temp->GetMaximum() > max ) 
	  max = temp->GetMaximum();
      }
    }
    if( dataHist ){
      dataHist->Draw("e1,SAME");
      if( dataHist->GetMaximum() > max ) 
	max = dataHist->GetMaximum();
    }

    stack->SetMaximum(max);
    stack->SetMinimum(0.1);

    botPad->cd();
    TH1F* ratio = new TH1F(*sum);
    ratio->SetNameTitle(sum->GetName()+TString("ratio"),sum->GetTitle());
    ratio->Divide(dataHist);
    //TGraphAsymmErrors* ratio = new TGraphAsymmErrors(sum,dataHist,"pois");
    ratio->SetMarkerStyle(8);
    ratio->GetYaxis()->SetTitle("MC/Data");
    ratio->GetYaxis()->SetRangeUser(0,2);
    ratio->GetXaxis()->SetRangeUser(lower,upper);
    ratio->GetXaxis()->SetTitle(xlabel);
    ratio->Draw("e1");

    topPad->cd();
    gPad->SetLogy(false);
    can->SaveAs(dir+"/"+label+".png");
    gPad->SetLogy(true);
    can->SaveAs(dir+"/"+label+"_LogY.png");

    TCanvas* legCan = new TCanvas("legCan","legCan",500,500);
    TLegend* leg = new TLegend(0.1,.1,.9,.9);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    for(int iSample = 0 ; iSample < ntuples.size() ; iSample++){
      leg->AddEntry(histoMap[ntuples[iSample]],tagMap[ntuples[iSample]],"f");
    }
    for(int iSample = 0 ; iSample < signalNtuples.size() ; iSample++){
      leg->AddEntry(signalHistoMap[signalNtuples[iSample]],tagMap[signalNtuples[iSample]],"f");
    }
    if( dataHist ) 
      leg->AddEntry(dataHist,"data","p");
    leg->Draw();
    legCan->SaveAs(dir+"/legend.png");

  }

  TString label;
  TString xlabel;
  int nbins;
  double lower,upper;
  double* binEdges;
  map<ntupleType*,TH1F*> histoMap;
  map<ntupleType*,TString> tagMap;
  map<ntupleType*,TH1F*> signalHistoMap;
  TH1F* dataHist;
  double (*fillerFunc)(ntupleType*);
  THStack* stack;
  TH1F* sum; 
};
