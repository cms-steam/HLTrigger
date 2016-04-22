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

#include "HLTrigger/HLTanalyzers/interface/HLTResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

// HLT related
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

HLTResults::HLTResults() {

  //set parameter defaults 
  _Debug=false;
}

void HLTResults::beginRun(const edm::Run& run, const edm::EventSetup& c){ 


  bool changed(true);
  if (hltPrescaleProvider_->init(run,c,processName_,changed)) {
    // if init returns TRUE, initialisation has succeeded!
    if (changed) {
      // The HLT config has actually changed wrt the previous Run, hence rebook your
      // histograms or do anything else dependent on the revised HLT config
      std::cout << "Initalizing HLTConfigProvider"  << std::endl;
    }
  } else {
    // if init returns FALSE, initialisation has NOT succeeded, which indicates a problem
    // with the file and/or code and needs to be investigated!
    std::cout << " HLT config extraction failure with process name " << processName_ << std::endl;
    // In this case, all access methods will return empty values!
  }

}

/*  Setup the analysis to put the branch-variables into the tree. */
void HLTResults::setup(const edm::ParameterSet& pSet, TTree* HltTree) {

  processName_ = pSet.getParameter<std::string>("HLTProcessName") ;

  edm::ParameterSet myHltParams = pSet.getParameter<edm::ParameterSet>("RunParameters") ;
  std::vector<std::string> parameterNames = myHltParams.getParameterNames() ;
  
  for ( std::vector<std::string>::iterator iParam = parameterNames.begin();
        iParam != parameterNames.end(); iParam++ ){
    if ( (*iParam) == "Debug" ) _Debug =  myHltParams.getParameter<bool>( *iParam );
  }

  dummyBranches_ = pSet.getUntrackedParameter<std::vector<std::string> >("dummyBranches",std::vector<std::string>(0));

  HltEvtCnt = 0;
  const int kMaxTrigFlag = 10000;
  trigflag = new int[kMaxTrigFlag];
  trigPrescl = new int[kMaxTrigFlag];

}

/* **Analyze the event** */
void HLTResults::analyze(const edm::Handle<edm::TriggerResults>                 & hltresults,
			 edm::EventSetup const& eventSetup,
			 edm::Event const& iEvent,
			 TTree* HltTree) {

  //   std::cout << " Beginning HLTResults " << std::endl;

  /////////// Analyzing HLT Trigger Results (TriggerResults) //////////
  if (hltresults.isValid()) {
    int ntrigs = hltresults->size();
    if (ntrigs==0){std::cout << "%HLTResults -- No trigger name given in TriggerResults of the input " << std::endl;}

    edm::TriggerNames const& triggerNames = iEvent.triggerNames(*hltresults);

    // 1st event : Book as many branches as trigger paths provided in the input...
    if (HltEvtCnt==0){
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

      HltEvtCnt++;
    }
    // ...Fill the corresponding accepts in branch-variables

    //std::cout << "Number of prescale sets: " << hltConfig_.prescaleSize() << std::endl;
    //std::cout << "Number of HLT paths: " << hltConfig_.size() << std::endl;
    //int presclSet = hltConfig_.prescaleSet(iEvent, eventSetup);
    //std::cout<<"\tPrescale set number: "<< presclSet <<std::endl; 

    for (int itrig = 0; itrig != ntrigs; ++itrig){

      std::string trigName=triggerNames.triggerName(itrig);
      bool accept = hltresults->accept(itrig);

      trigPrescl[itrig] = hltPrescaleProvider_->prescaleValue(iEvent, eventSetup, trigName);


      if (accept){trigflag[itrig] = 1;}
      else {trigflag[itrig] = 0;}

      if (_Debug){
        if (_Debug) std::cout << "%HLTResults --  Number of HLT Triggers: " << ntrigs << std::endl;
        std::cout << "%HLTResults --  HLTTrigger(" << itrig << "): " << trigName << " = " << accept << std::endl;
      }
    }
  }
  else { if (_Debug) std::cout << "%HLTResults -- No Trigger Result" << std::endl;}

  if (_Debug) std::cout << "%HLTResults -- Done with routine" << std::endl;
}
