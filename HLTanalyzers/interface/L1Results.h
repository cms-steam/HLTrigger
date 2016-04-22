#ifndef L1RESULTS_H
#define L1RESULTS_H

// C++
#include <vector>
#include <map>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TNamed.h"
#include "TROOT.h"
#include "TChain.h"

// CMSSW
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Common/interface/Provenance.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/Handle.h"

// L1 Stage2
#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1TUtmAlgorithm.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"

namespace edm {
  class ConsumesCollector;
  class ParameterSet;
}

typedef std::vector<std::string> MyStrings;

/** \class L1Results
  *  
  * $Date: April 2016
  * $Revision: 
  * \author N. Daci - VUB
  */
class L1Results {
public:
  //L1Results();

  template <typename T>
    L1Results(edm::ParameterSet const& pset,
	      edm::ConsumesCollector&& iC,
	      T& module);
  
  template <typename T>
    L1Results(edm::ParameterSet const& pset,
	      edm::ConsumesCollector& iC,
	      T& module);  
  
  void setup(const edm::ParameterSet& pSet, TTree* tree);
  //void beginRun(const edm::Run& , const edm::EventSetup& );
  void beginRun(const edm::Run& , edm::EventSetup const & );

  /** Analyze the Data */
  void analyze(const edm::Handle<GlobalAlgBlkBxCollection> & l1results,
	       edm::EventSetup const& eventSetup,
	       edm::Event const& iEvent,
	       TTree* tree);

private:

  L1Results();

  // Tree variables
  /* float *hltppt, *hltpeta; */
  int L1EvtCnt; //, nhltpart;
  int *trigflag; 
  int *trigPrescl;

  // utilities
  std::vector<std::string> dummyBranches_;
  std::string processName_;

  // input variables
  edm::ESHandle<L1TUtmTriggerMenu> l1GtMenu_;
  //std::map<std::string, L1TUtmAlgorithm> const & algorithmMap_;
  const std::map<std::string, L1TUtmAlgorithm>* algorithmMap_;
  bool _Debug;
};

template <typename T>
L1Results::L1Results(edm::ParameterSet const& pset,
		     edm::ConsumesCollector&& iC,
		     T& module) :
L1Results(pset, iC, module) {
}

/* 
template <typename T>
L1Results::L1Results(edm::ParameterSet const& pset,
		     edm::ConsumesCollector& iC,
		     T& module) :
L1Results() {
    hltPrescaleProvider_.reset(new HLTPrescaleProvider(pset, iC, module));
}
*/

#endif
