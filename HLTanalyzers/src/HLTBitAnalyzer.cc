// File: HLTBitAnalyzer.cc
// Description:  Example of Analysis driver originally from Jeremy Mans, 
// Date:  13-October-2006

#include <boost/foreach.hpp>

#include "HLTrigger/HLTanalyzers/interface/HLTBitAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

typedef const edm::InputTag* MissingCollectionInfo;
  
template <class T>
static inline
bool getCollection(const edm::Event & event, std::vector<MissingCollectionInfo> & missing, edm::Handle<T> & handle, const edm::InputTag & name, const edm::EDGetTokenT<T> token) 
{
  event.getByToken(token, handle);
  bool valid = handle.isValid();
  if (not valid) {
    missing.push_back(&name);
    handle.clear();

  }
  return valid;
}

// Boiler-plate constructor definition of an analyzer module:
HLTBitAnalyzer::HLTBitAnalyzer(edm::ParameterSet const& conf)  :
  hlt_analysis_(conf, consumesCollector(), *this) {

  // If your module takes parameters, here is where you would define
  // their names and types, and access them to initialize internal
  // variables. Example as follows:
  std::cout << " Beginning HLTBitAnalyzer Analysis " << std::endl;

  mctruth_           = conf.getParameter<edm::InputTag> ("mctruth");
  genEventInfo_      = conf.getParameter<edm::InputTag> ("genEventInfo");
  simhits_           = conf.getParameter<edm::InputTag> ("simhits");

  hltresults_  = conf.getParameter<edm::InputTag> ("hltresults");
  l1results_   = conf.getParameter<edm::InputTag> ("l1results");

  //L1 objects
  l1muons_  = conf.getParameter<edm::InputTag> ("l1muons");
  l1egamma_ = conf.getParameter<edm::InputTag> ("l1egamma");
  l1jets_   = conf.getParameter<edm::InputTag> ("l1jets");
  l1taus_   = conf.getParameter<edm::InputTag> ("l1taus");
  l1etsums_ = conf.getParameter<edm::InputTag> ("l1etsums");

  pileupInfo_ = edm::InputTag("addPileupInfo");

  hltresultsToken_        = consumes<edm::TriggerResults>(hltresults_);
  genEventInfoToken_      = consumes<GenEventInfoProduct>(genEventInfo_);  
  l1resultsToken_         = consumes<GlobalAlgBlkBxCollection>(l1results_);
  l1muonsToken_           = consumes<l1t::MuonBxCollection>(l1muons_);
  l1egammaToken_          = consumes<l1t::EGammaBxCollection>(l1egamma_);
  l1jetsToken_            = consumes<l1t::JetBxCollection>(l1jets_);
  l1tausToken_            = consumes<l1t::TauBxCollection>(l1taus_);
  l1etsumsToken_          = consumes<l1t::EtSumBxCollection>(l1etsums_);
  mctruthToken_           = consumes<reco::CandidateView>(mctruth_);
  simtracksToken_         = consumes<std::vector<SimTrack> >(simhits_);
  simverticesToken_       = consumes<std::vector<SimVertex> >(simhits_);
  pileupInfoToken_        = consumes<std::vector<PileupSummaryInfo> >(pileupInfo_);
  
  _UseTFileService = conf.getUntrackedParameter<bool>("UseTFileService",false);
	
  m_file = 0;   // set to null
  errCnt = 0;
  errMax = 5;

  // read run parameters with a default value
  edm::ParameterSet runParameters = conf.getParameter<edm::ParameterSet>("RunParameters");
  _HistName = runParameters.getUntrackedParameter<std::string>("HistogramFile", "test.root");
  _isData   = runParameters.getParameter<bool>("isData");
  _Debug    = runParameters.getParameter<bool>("Debug");

  // open the tree file and initialize the tree
  if(_UseTFileService){
    edm::Service<TFileService> fs;
    HltTree = fs->make<TTree>("HltTree", "");
  }else{
    m_file = new TFile(_HistName.c_str(), "RECREATE");
    if (m_file) m_file->cd();
    HltTree = new TTree("HltTree", "");
  }

  // Setup the different analysis
  hlt_analysis_.setup(conf, HltTree);
  if (!_isData) { mct_analysis_.setup(conf, HltTree); }
  evt_header_.setup(consumesCollector(), HltTree);
}

// Boiler-plate "analyze" method declaration for an analyzer module.
void HLTBitAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {

  edm::Handle<edm::TriggerResults>      hltresults;

  edm::Handle<GlobalAlgBlkBxCollection> l1results;
  edm::Handle<l1t::MuonBxCollection>    l1muons;
  edm::Handle<l1t::EGammaBxCollection>  l1egamma;
  edm::Handle<l1t::JetBxCollection>     l1jets;
  edm::Handle<l1t::TauBxCollection>     l1taus;
  edm::Handle<l1t::EtSumBxCollection>   l1etsums;

  edm::Handle<reco::CandidateView>               mctruth;
  edm::Handle<GenEventInfoProduct>               genEventInfo;
  edm::Handle<std::vector<SimTrack> >            simTracks;
  edm::Handle<std::vector<SimVertex> >           simVertices;
  edm::Handle<std::vector< PileupSummaryInfo > > pupInfo; 

  // extract the collections from the event, check their validity and log which are missing
  std::vector<MissingCollectionInfo> missing;

  getCollection( iEvent, missing, hltresults,   hltresults_,   hltresultsToken_);
  getCollection( iEvent, missing, l1results,    l1results_,    l1resultsToken_);
  getCollection( iEvent, missing, l1muons,      l1muons_,      l1muonsToken_);
  getCollection( iEvent, missing, l1egamma,     l1egamma_,     l1egammaToken_);
  getCollection( iEvent, missing, l1jets,       l1jets_,       l1jetsToken_);
  getCollection( iEvent, missing, l1taus,       l1taus_,       l1tausToken_);
  getCollection( iEvent, missing, l1etsums,     l1etsums_,     l1etsumsToken_);

  if(!_isData){
    getCollection( iEvent, missing, mctruth,      mctruth_,      mctruthToken_);
    getCollection( iEvent, missing, simTracks,    simhits_,      simtracksToken_);
    getCollection( iEvent, missing, simVertices,  simhits_,      simverticesToken_);
    getCollection( iEvent, missing, genEventInfo, genEventInfo_, genEventInfoToken_);
    getCollection( iEvent, missing, pupInfo,      pileupInfo_,   pileupInfoToken_);
  }

  double ptHat = -1.;
  double weight = -1.;
  if (!_isData) {
    ptHat=-1.;
    if (genEventInfo.isValid()) {ptHat=genEventInfo->qScale();}
    
    weight = genEventInfo->weight();
  }
  // print missing collections
  if (not missing.empty() and errCnt < errMax ) {
    errCnt++;
    std::stringstream out;       
    out <<  "OpenHLT analyser - missing collections:";
    BOOST_FOREACH(const MissingCollectionInfo & entry, missing)
      out << "\n\t" << entry->encode();
    edm::LogPrint("OpenHLT") << out.str() << std::endl; 
    if(errCnt == errMax)
      edm::LogWarning("OpenHLT") << "Maximum error count reached -- No more messages will be printed.";
  }

  // run the analysis, passing required event fragments
  if(_Debug) std::cout << "Launch HLTInfo analysis" << std::endl;
  hlt_analysis_.analyze(hltresults,
			l1results,
			l1muons,
			l1egamma,
			l1jets,
			l1taus,
			l1etsums,
			iSetup,
			iEvent,
			HltTree);

  evt_header_.analyze(iEvent, HltTree);

  if (!_isData) mct_analysis_.analyze(mctruth,
				      ptHat,
				      weight,
				      simTracks,
				      simVertices,
				      pupInfo,
				      HltTree);

  if(_Debug) std::cout << " Ending Event Analysis" << std::endl;
  // After analysis, fill the variables tree
  if (m_file) m_file->cd();

  HltTree->Fill();
}

// ------------ method called when starting to processes a run  ------------
void 
HLTBitAnalyzer::beginRun(edm::Run const& run, edm::EventSetup const& es)
{
  hlt_analysis_.beginRun(run, es);
}

// "endJob" is an inherited method that you may implement to do post-EOF processing and produce final output.
void HLTBitAnalyzer::endJob() {
	
  if(!_UseTFileService){
    if (m_file) m_file->cd();
		
    HltTree->Write();
    delete HltTree;
    HltTree = 0;
		
    if (m_file) {         // if there was a tree file...
      m_file->Write();    // write out the branches
      delete m_file;      // close and delete the file
      m_file = 0;         // set to zero to clean up
    }
  }
}
