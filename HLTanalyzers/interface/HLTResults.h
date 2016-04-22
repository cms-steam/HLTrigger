#ifndef HLTRESULTS_H
#define HLTRESULTS_H

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TNamed.h"
#include <vector>
#include <map>
#include "TROOT.h"
#include "TChain.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Common/interface/Provenance.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "HLTrigger/HLTanalyzers/interface/JetUtil.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

namespace edm {
  class ConsumesCollector;
  class ParameterSet;
}

typedef std::vector<std::string> MyStrings;

/** \class HLTResults
  *  
  * $Date: November 2006
  * $Revision: 
  * \author P. Bargassa - Rice U.
  */
class HLTResults {
public:
  //HLTResults();

  template <typename T>
    HLTResults(edm::ParameterSet const& pset,
	    edm::ConsumesCollector&& iC,
	    T& module);
  
  template <typename T>
    HLTResults(edm::ParameterSet const& pset,
	    edm::ConsumesCollector& iC,
	    T& module);  
  
  void setup(const edm::ParameterSet& pSet, TTree* tree);
  void beginRun(const edm::Run& , const edm::EventSetup& );

  /** Analyze the Data */
  void analyze(const edm::Handle<edm::TriggerResults>                 & hltresults,
	       edm::EventSetup const& eventSetup,
	       edm::Event const& iEvent,
	       TTree* tree);

private:

  HLTResults();

  // Tree variables
  int HltEvtCnt;
  int *trigflag, *trigPrescl;

  // utilities
  std::vector<std::string> dummyBranches_;
  std::unique_ptr<HLTPrescaleProvider> hltPrescaleProvider_;
  std::string processName_;

  // input variables
  bool _Debug;
};

template <typename T>
HLTResults::HLTResults(edm::ParameterSet const& pset,
		       edm::ConsumesCollector&& iC,
		       T& module) :
HLTResults(pset, iC, module) {
}

template <typename T>
HLTResults::HLTResults(edm::ParameterSet const& pset,
		       edm::ConsumesCollector& iC,
		       T& module) :
HLTResults() {
  hltPrescaleProvider_.reset(new HLTPrescaleProvider(pset, iC, module));
}

#endif
