// File: HLTBitAnalyzer.cc
// Description:  Example of Analysis driver originally from Jeremy Mans, 
// Date:  13-October-2006

#include <boost/foreach.hpp>

#include "HLTrigger/HLTanalyzers/interface/HLTBitAnalyzer.h"
#include "HLTMessages.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

typedef std::pair<const char *, const edm::InputTag *> MissingCollectionInfo;
  
template <class T>
static inline
bool getCollection(const edm::Event & event, std::vector<MissingCollectionInfo> & missing, edm::Handle<T> & handle, const edm::InputTag & name, const edm::EDGetTokenT<T> token, const char * description) 
{
  event.getByToken(token, handle);
  bool valid = handle.isValid();
  if (not valid) {
    missing.push_back( std::make_pair(description, & name) );
    handle.clear();
    //	std::cout << "not valid "<< description << " " << name << std::endl;
  }
  return valid;
}

// Boiler-plate constructor definition of an analyzer module:
//HLTBitAnalyzer::HLTBitAnalyzer(edm::ParameterSet const& conf) {
HLTBitAnalyzer::HLTBitAnalyzer(edm::ParameterSet const& conf)
{

  hlt_analysis_(conf, consumesCollector(), *this);
  l1_analysis_ (conf, consumesCollector(), *this);

  // If your module takes parameters, here is where you would define
  // their names and types, and access them to initialize internal
  // variables. Example as follows:
  std::cout << " Beginning HLTBitAnalyzer Analysis " << std::endl;

  // InputTags //

  // reco/sim informations
  mctruth_            = conf.getParameter<edm::InputTag> ("mctruth");
  genEventInfo_       = conf.getParameter<edm::InputTag> ("genEventInfo");
  VertexTagOffline0_  = conf.getParameter<edm::InputTag> ("OfflinePrimaryVertices0");
  simhits_            = conf.getParameter<edm::InputTag> ("simhits");
  pileupInfo_         = edm::InputTag("addPileupInfo");

  // HLT
  hltresults_       = conf.getParameter<edm::InputTag> ("hltresults");

  // L1 Stage2 Menu
  l1results_        = conf.getParameter<edm::InputTag> ("l1results");

  // Tokens //

  // reco/sim informations
  genEventInfoToken_ = consumes<GenEventInfoProduct>(genEventInfo_);
  mctruthToken_ = consumes<reco::CandidateView>(mctruth_);
  VertexTagOffline0Token_ = consumes<reco::VertexCollection>(VertexTagOffline0_);
  simtracksToken_ = consumes<std::vector<SimTrack> >(simhits_);
  simverticesToken_ = consumes<std::vector<SimVertex> >(simhits_);
  pileupInfoToken_ = consumes<std::vector<PileupSummaryInfo> >(pileupInfo_);

  // HLT
  hltresultsToken_  = consumes<edm::TriggerResults>(hltresults_);

  // L1 Stage2 Menu
  ugt_token_        = consumes<GlobalAlgBlkBxCollection>(l1results_);

  // TFile service
  _UseTFileService = conf.getUntrackedParameter<bool>("UseTFileService",false);
	
  m_file = 0;   // set to null
  errCnt = 0;

  // read run parameters with a default value
  edm::ParameterSet runParameters = conf.getParameter<edm::ParameterSet>("RunParameters");
  _HistName = runParameters.getUntrackedParameter<std::string>("HistogramFile", "test.root");
  _isData = runParameters.getUntrackedParameter<bool>("isData",true);

  // open the tree file and initialize the tree
  if(_UseTFileService){
    edm::Service<TFileService> fs;
    HltTree = fs->make<TTree>("HltTree", "");
    L1Tree  = fs->make<TTree>("L1Tree" , "");
  }else{
    m_file = new TFile(_HistName.c_str(), "RECREATE");
    if (m_file)
      m_file->cd();
    HltTree = new TTree("HltTree", "");
    L1Tree  = new TTree("L1Tree" , "");
  }

  // Setup the different analysis
  hlt_analysis_.setup(conf, HltTree);
  l1_analysis_ .setup(conf, L1Tree );
  if (!_isData) { mct_analysis_.setup(conf, HltTree); }
  vrt_analysisOffline0_.setup(conf, HltTree, "Offline0");
  evt_header_.setup(consumesCollector(), HltTree);
}

// Boiler-plate "analyze" method declaration for an analyzer module.
void HLTBitAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {

  // reco/sim informations
  edm::Handle<reco::CandidateView>                  mctruth;
  edm::Handle<GenEventInfoProduct>                  genEventInfo;
  edm::Handle<std::vector<SimTrack> >               simTracks;
  edm::Handle<std::vector<SimVertex> >              simVertices;
  edm::Handle<reco::VertexCollection>               recoVertexsOffline0; 
  edm::Handle<std::vector< PileupSummaryInfo > >    pupInfo; 

  // HLT
  edm::Handle<edm::TriggerResults>                  hltresults;

  // L1 Stage2 Menu
  edm::Handle<GlobalAlgBlkBxCollection>             l1results;

  // extract the collections from the event, check their validity and log which are missing
  std::vector<MissingCollectionInfo> missing;

  // reco/sim informations
  getCollection( iEvent, missing, mctruth,         mctruth_,      mctruthToken_,         kMctruth );
  getCollection( iEvent, missing, simTracks,       simhits_,      simtracksToken_,       kSimhit );
  getCollection( iEvent, missing, simVertices,     simhits_,      simverticesToken_,     kSimhit );
  getCollection( iEvent, missing, genEventInfo,    genEventInfo_, genEventInfoToken_,    kGenEventInfo );
  getCollection( iEvent, missing, pupInfo,         pileupInfo_,   pileupInfoToken_,      kPileupInfo );
  getCollection( iEvent, missing, recoVertexsOffline0,            VertexTagOffline0_,    VertexTagOffline0Token_,     kRecoVerticesOffline0 );
  //
  getCollection( iEvent, missing, hltresults,      hltresults_,   hltresultsToken_     , kHltresults );
  getCollection( iEvent, missing, l1results ,      l1results_ ,   ugt_token_           , kL1GtRR     );

  if (!_isData) {
    ptHat=-1.;
    if (genEventInfo.isValid()) {ptHat=genEventInfo->qScale();}
    
    weight = genEventInfo->weight();
  }

  // print missing collections
  if (not missing.empty() and (errCnt < errMax())) {
    errCnt++;
    std::stringstream out;       
    out <<  "OpenHLT analyser - missing collections:";
    BOOST_FOREACH(const MissingCollectionInfo & entry, missing)
      out << "\n\t" << entry.first << ": " << entry.second->encode();
    edm::LogPrint("OpenHLT") << out.str() << std::endl; 
    if (errCnt == errMax())
      edm::LogWarning("OpenHLT") << "Maximum error count reached -- No more messages will be printed.";
  }

  // run the analysis, passing required event fragments
  l1_analysis_.analyze(
    l1results,
    iSetup,
    iEvent,
    L1Tree);

  hlt_analysis_.analyze(
    hltresults,
    iSetup,
    iEvent,
    HltTree);

  evt_header_.analyze(iEvent, HltTree);

  if (!_isData) {
    mct_analysis_.analyze(
			  mctruth,
			  ptHat,
			  weight,
			  simTracks,
			  simVertices,
			  pupInfo,
			  HltTree);
  }
  vrt_analysisOffline0_.analyze(
  				recoVertexsOffline0,
  				HltTree);

  // std::cout << " Ending Event Analysis" << std::endl;
  // After analysis, fill the variables tree
  if (m_file)
    m_file->cd();
  HltTree->Fill();
}

// ------------ method called when starting to processes a run  ------------
void 
HLTBitAnalyzer::beginRun(edm::Run const& run, edm::EventSetup const& es)
{
   hlt_analysis_.beginRun(run, es);
   l1_analysis_ .beginRun(run, es);
}

// "endJob" is an inherited method that you may implement to do post-EOF processing and produce final output.
void HLTBitAnalyzer::endJob() {
	
  if(!_UseTFileService){
    if (m_file)
      m_file->cd();
    
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
