//
// Original Author:  Boris Mangano (ETHZ) [mangano]
//         Created:  Sun Aug 25 11:05:49 CEST 2013
// $Id$
//
//

using namespace std;
// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//#include "DataFormats/VertexReco/interface/Vertex.h"
//#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

 
#include "TH2D.h"

//
// class declaration
//

class TobTecTagProducer : public edm::EDProducer {
public:
  explicit TobTecTagProducer(const edm::ParameterSet&);
  ~TobTecTagProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  virtual void beginRun(edm::Run&, edm::EventSetup const&);
  virtual void endRun(edm::Run&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

  // ----------member data ---------------------------
  edm::InputTag trackCollection_;
  int nPhiBins;
  int nEtaBins;
  double etaMin;
  double etaMax;
  double etaOffset;
  //double phiMin;
  //double phiMax;

};


// constructors and destructor
//
TobTecTagProducer::TobTecTagProducer(const edm::ParameterSet& iConfig)
{
  trackCollection_ = iConfig.getParameter< edm::InputTag > ("trackCollection");
  nPhiBins = iConfig.getParameter<int> ("nPhiBins");
  nEtaBins = iConfig.getParameter<int> ("nEtaBins");
  etaMin   = iConfig.getParameter<double> ("etaMin"); 
  etaMax   = iConfig.getParameter<double> ("etaMax"); 
  etaOffset   = iConfig.getParameter<double> ("etaOffset"); 
  //phiMin   = iConfig.getParameter<double> ("phiMin"); 
  //phiMax   = iConfig.getParameter<double> ("phiMax"); 

  //register your products
  produces<double>(); 
  
}


TobTecTagProducer::~TobTecTagProducer()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
TobTecTagProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  using namespace edm;
  using namespace reco;
  
  std::auto_ptr<double> pOut(new double(-1));



  Handle<reco::TrackCollection> tracks;
  iEvent.getByLabel(trackCollection_,tracks);

  //Handle<reco::VertexCollection> vtxs;
  //iEvent.getByLabel("offlinePrimaryVerticesWithBS",vtxs);
  //int nvtx = vtxs->size();


  // ==================
  // loop track collection and fill histograms
  double maxRatio(-1);


  TH2D numH("numH","numerator histo",nPhiBins,-M_PI,M_PI,nEtaBins,etaMin,etaMax);
  TH2D denH("denH","denominator histo",nPhiBins,-M_PI,M_PI,nEtaBins,etaMin,etaMax);

  TH2D numH2("numH2","numerator histo2",nPhiBins,-M_PI,M_PI,nEtaBins/2,0.+etaOffset,etaMax+etaOffset);
  TH2D denH2("denH2","denominator histo2",nPhiBins,-M_PI,M_PI,nEtaBins/2,0.+etaOffset,etaMax+etaOffset);

  TH2D numH3("numH3","numerator histo3",nPhiBins,-M_PI,M_PI,nEtaBins/2,-(etaMax+etaOffset),0.0-etaOffset);
  TH2D denH3("denH3","denominator histo3",nPhiBins,-M_PI,M_PI,nEtaBins/2,-(etaMax+etaOffset),0.0-etaOffset);


  for(std::vector<Track>::const_iterator tk=tracks->begin(); tk!=tracks->end(); ++tk){
    if(tk->algo()==10) {
      numH.Fill(tk->phi(),tk->eta());
      numH2.Fill(tk->phi(),tk->eta());
      numH3.Fill(tk->phi(),tk->eta());
    }

    if(tk->algo()==4) {
      denH.Fill(tk->phi(),tk->eta());
      denH2.Fill(tk->phi(),tk->eta());
      denH3.Fill(tk->phi(),tk->eta());
    }

  }

  
  // --- here I avoid bins with zero content in denH, before I do the ratio
  for(int binX=1; binX<=denH.GetNbinsX();binX++){
    for(int binY=1; binY<=denH.GetNbinsY();binY++){
      if(denH.GetBinContent(binX,binY)==0) denH.SetBinContent(binX,binY,1.0);      
    }
  }

  for(int binX=1; binX<=denH2.GetNbinsX();binX++){
    for(int binY=1; binY<=denH2.GetNbinsY();binY++){
      if(denH2.GetBinContent(binX,binY)==0) denH2.SetBinContent(binX,binY,1.0);      
    }
  }

  for(int binX=1; binX<=denH3.GetNbinsX();binX++){
    for(int binY=1; binY<=denH3.GetNbinsY();binY++){
      if(denH3.GetBinContent(binX,binY)==0) denH3.SetBinContent(binX,binY,1.0);      
    }
  }
  // ----
  
  numH.Divide(&denH);
  numH2.Divide(&denH2);
  numH3.Divide(&denH3);

  double maxRatio1 = numH.GetBinContent(numH.GetMaximumBin());
  double maxRatio2 = numH2.GetBinContent(numH2.GetMaximumBin());
  double maxRatio3 = numH3.GetBinContent(numH3.GetMaximumBin());

  maxRatio = max(maxRatio1,maxRatio2);
  maxRatio = max(maxRatio,maxRatio3);

  *pOut = maxRatio;
  iEvent.put(pOut);
 
}

// ------------ method called once each job just before starting event loop  ------------
void 
TobTecTagProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TobTecTagProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
TobTecTagProducer::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
TobTecTagProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
TobTecTagProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TobTecTagProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TobTecTagProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TobTecTagProducer);
