#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <functional>
#include <stdlib.h>
#include <string.h>

#include "HLTrigger/HLTanalyzers/interface/L1Results.h"
#include "FWCore/Common/interface/TriggerNames.h"

L1Results::L1Results() {

  //set parameter defaults 
  _Debug=false;
}

void L1Results::beginRun(const edm::Run& run, edm::EventSetup const & evSetup){ 
  evSetup.get<L1TUtmTriggerMenuRcd>().get(l1GtMenu_) ;
  //algorithmMap_ = l1GtMenu_->getAlgorithmMap();
  algorithmMap_ = &(l1GtMenu_->getAlgorithmMap());
}

/*  Setup the analysis to put the branch-variables into the tree. */
void L1Results::setup(const edm::ParameterSet& pSet, TTree* HltTree) {

  processName_ = pSet.getParameter<std::string>("L1ProcessName") ;

  edm::ParameterSet myL1Params = pSet.getParameter<edm::ParameterSet>("L1Parameters") ;
  std::vector<std::string> parameterNames = myL1Params.getParameterNames() ;
  
  for ( std::vector<std::string>::iterator iParam = parameterNames.begin();
        iParam != parameterNames.end(); iParam++ ){
    if ( (*iParam) == "Debug" ) _Debug =  myL1Params.getParameter<bool>( *iParam );
  }

  dummyBranches_ = pSet.getUntrackedParameter<std::vector<std::string> >("dummyBranches",std::vector<std::string>(0));

  L1EvtCnt = 0;
  const int kMaxTrigFlag = 10000;
  trigflag = new int[kMaxTrigFlag];
  trigPrescl = new int[kMaxTrigFlag];

}

/* **Analyze the event** */
void L1Results::analyze(const edm::Handle<GlobalAlgBlkBxCollection>  & l1results,
			edm::EventSetup const& eventSetup,
			edm::Event const& iEvent,
			TTree* HltTree) {

  //   std::cout << " Beginning L1Results " << std::endl;

  /////////// Analyzing L1 Trigger Results //////////

  if (l1results.isValid()) {

    if(_Debug) std::cout << "%L1Results -- l1results isValid!" << std::endl;

    /*
    // int ntrigs = l1results->size();
    // if (ntrigs==0){std::cout << "%L1Results -- No trigger name given in TriggerResults of the input " << std::endl;}

    // edm::TriggerNames const& triggerNames = iEvent.triggerNames(*hltresults);

    // 1st event : Book as many branches as trigger paths provided in the input...
    if (L1EvtCnt==0){
      for (int itrig = 0; itrig != ntrigs; ++itrig) {
        TString trigName = triggerNames.triggerName(itrig);
        HltTree->Branch(trigName,trigflag+itrig,trigName+"/I");
        HltTree->Branch(trigName+"_Prescl",trigPrescl+itrig,trigName+"_Prescl/I");
      }

      int itdum = ntrigs;
      for (unsigned int idum = 0; idum < dummyBranches_.size(); ++idum) {
	TString trigName(dummyBranches_[idum].data());
	bool addThisBranch = 1;
	for (int itrig = 0; itrig != ntrigs; ++itrig) {
	  TString realTrigName = triggerNames.triggerName(itrig);
	  if(trigName == realTrigName) addThisBranch = 0;
	}
	if(addThisBranch){
	  HltTree->Branch(trigName,trigflag+itdum,trigName+"/I");
	  HltTree->Branch(trigName+"_Prescl",trigPrescl+itdum,trigName+"_Prescl/I");
	  trigflag[itdum] = 0;
	  trigPrescl[itdum] = 0;
	  ++itdum;
	}
      }

      L1EvtCnt++;
    }

    for (int itrig = 0; itrig != ntrigs; ++itrig){

      std::string trigName=triggerNames.triggerName(itrig);
      bool accept = hltresults->accept(itrig);

      trigPrescl[itrig] = hltPrescaleProvider_->prescaleValue(iEvent, eventSetup, trigName);

      if (accept){trigflag[itrig] = 1;}
      else {trigflag[itrig] = 0;}

      if (_Debug){
        if (_Debug) std::cout << "%L1Results --  Number of HLT Triggers: " << ntrigs << std::endl;
        std::cout << "%L1Results --  HLTTrigger(" << itrig << "): " << trigName << " = " << accept << std::endl;
      }
    }

    */

  }
  else { if (_Debug) std::cout << "%L1Results -- No Trigger Result" << std::endl;}

  if (_Debug) std::cout << "%L1Results -- Done with routine" << std::endl;
}
