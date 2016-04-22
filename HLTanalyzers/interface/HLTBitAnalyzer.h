#include <iostream>

#include "HLTrigger/HLTanalyzers/interface/EventHeader.h"
#include "HLTrigger/HLTanalyzers/interface/HLTResults.h"
#include "HLTrigger/HLTanalyzers/interface/L1Results.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/Registry.h"

#include "DataFormats/Common/interface/Handle.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"  
#include "HLTrigger/HLTanalyzers/interface/HLTMCtruth.h" 
#include "HLTrigger/HLTanalyzers/interface/RECOVertex.h" 

/** \class HLTBitAnalyzer
  *  
  * $Date: November 2006
  * $Revision: 
  * \author P. Bargassa - Rice U.
  */

class HLTBitAnalyzer : public edm::EDAnalyzer {
public:
  explicit HLTBitAnalyzer(edm::ParameterSet const& conf);
  virtual void analyze(edm::Event const& e, edm::EventSetup const& iSetup);
  virtual void endJob();
  virtual void beginRun(edm::Run const&, edm::EventSetup const&);

  //  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions); 

  // Analysis tree to be filled
  TTree *HltTree;
  TTree *L1Tree;

private:
  // variables persistent across events should be declared here.
  //
  ///Default analyses

  EventHeader evt_header_;
  HLTResults  hlt_analysis_;
  L1Results   l1_analysis_;

  HLTMCtruth  mct_analysis_;
  RECOVertex  vrt_analysisOffline0_;

  edm::InputTag hltresults_,genEventInfo_;
  edm::InputTag l1results_;

  edm::InputTag mctruth_,simhits_; 
  edm::InputTag VertexTagOffline0_;
  edm::InputTag pileupInfo_;

  edm::EDGetTokenT<edm::TriggerResults>                  hltresultsToken_;
  edm::EDGetTokenT<GenEventInfoProduct>                  genEventInfoToken_;
  edm::EDGetTokenT<GlobalAlgBlkBxCollection>             ugt_token_;

  edm::EDGetTokenT<reco::CandidateView>                     mctruthToken_;
  edm::EDGetTokenT<std::vector<SimTrack> >                  simtracksToken_;
  edm::EDGetTokenT<std::vector<SimVertex> >                 simverticesToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> >         pileupInfoToken_;
  edm::EDGetTokenT<reco::VertexCollection>                  VertexTagOffline0Token_;

  // L1 menu
  //edm::ESHandle<L1TUtmTriggerMenu>      l1menu_;

  int errCnt;
  static int errMax() { return 5; }

  std::string _HistName; // Name of histogram file
  double _EtaMin,_EtaMax;
  TFile* m_file; // pointer to Histogram file
  bool _UseTFileService;
  bool _isData;

  double ptHat, weight;

};
