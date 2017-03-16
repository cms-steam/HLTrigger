#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"  

#include "HLTrigger/HLTanalyzers/interface/EventHeader.h"
#include "HLTrigger/HLTanalyzers/interface/HLTInfo.h"
#include "HLTrigger/HLTanalyzers/interface/HLTMCtruth.h" 

/** \class HLTBitAnalyzer
  *  
  * $Date: November 2006
  * $Revision: 
  * \author P. Bargassa - Rice U.

  * $Date: April 2016 
  * $Revision:   
  * \author G. Karapostoli - ULB
  */

class HLTBitAnalyzer : public edm::EDAnalyzer {
public:
  explicit HLTBitAnalyzer(edm::ParameterSet const& conf);
  virtual void analyze(edm::Event const& e, edm::EventSetup const& iSetup);
  virtual void endJob();
  virtual void beginRun(edm::Run const&, edm::EventSetup const&);

private:

  EventHeader evt_header_;
  HLTInfo     hlt_analysis_;

  HLTMCtruth  mct_analysis_;

  edm::InputTag hltresults_;
  edm::InputTag genEventInfo_;
  edm::InputTag l1results_;
  edm::InputTag l1muons_;
  edm::InputTag l1egamma_;
  edm::InputTag l1jets_;
  edm::InputTag l1taus_;
  edm::InputTag l1etsums_;
  edm::InputTag mctruth_;
  edm::InputTag simhits_; 
  edm::InputTag pileupInfo_;

  edm::EDGetTokenT<edm::TriggerResults>             hltresultsToken_;
  edm::EDGetTokenT<GenEventInfoProduct>             genEventInfoToken_;
  edm::EDGetTokenT<GlobalAlgBlkBxCollection>        l1resultsToken_;    
  edm::EDGetTokenT<l1t::MuonBxCollection>           l1muonsToken_;    
  edm::EDGetTokenT<l1t::EGammaBxCollection>         l1egammaToken_;    
  edm::EDGetTokenT<l1t::JetBxCollection>            l1jetsToken_;    
  edm::EDGetTokenT<l1t::TauBxCollection>            l1tausToken_;    
  edm::EDGetTokenT<l1t::EtSumBxCollection>          l1etsumsToken_;    
  edm::EDGetTokenT<reco::CandidateView>             mctruthToken_;
  edm::EDGetTokenT<std::vector<SimTrack> >          simtracksToken_;
  edm::EDGetTokenT<std::vector<SimVertex> >         simverticesToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupInfoToken_;

  int errCnt;
  int errMax;

  std::string _HistName; // Name of histogram file
  TFile* m_file;         // pointer to Histogram file
  TTree *HltTree;        // Analysis tree to be filled
  bool _UseTFileService;
  bool _isData;
  bool _Debug;

};
