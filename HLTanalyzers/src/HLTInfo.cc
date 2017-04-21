//#include <iostream>
//#include <string>

#include "HLTrigger/HLTanalyzers/interface/HLTInfo.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"

#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"

HLTInfo::HLTInfo() {
  //set parameter defaults 
  _Debug=false;
  _isL1MuHighQual=true;
}

template <class T>
void HLTInfo::get_max_l1_info(const BXVector<T> obj_list, float &ptmax, float &etamax, float &phimax, bool isIsolated, bool isMuon){
  ptmax  = -999.;
  etamax = -999.;
  phimax = -999.;

  //std::cout << "Size of list = " << obj_list.size(0) << std::endl;

  if(obj_list.size(0) < 1) return;

  for(typename std::vector< T >::const_iterator obj = obj_list.begin(0); obj != obj_list.end(0); obj++){
    if(obj->pt() < ptmax) continue;
    if(isIsolated && !obj->hwIso()) continue;

    if(isMuon && obj->hwQual() < 8) continue;
    if(isMuon && _isL1MuHighQual && obj->hwQual() < 10) continue;
    //if(isMuon) cout << obj->hwQual() << endl;

    ptmax = obj->pt();
    etamax = obj->eta();
    phimax = obj->phi();
  }

}

void HLTInfo::beginRun(const edm::Run& run, const edm::EventSetup& c){ 

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
void HLTInfo::setup(const edm::ParameterSet& pSet, TTree* HltTree) {

  processName_ = pSet.getParameter<std::string>("HLTProcessName") ;

  _isL1MuHighQual = pSet.getParameter<bool>("isL1MuHighQual");

  edm::ParameterSet myHltParams = pSet.getParameter<edm::ParameterSet>("RunParameters") ;
  std::vector<std::string> parameterNames = myHltParams.getParameterNames() ;
  
  for ( std::vector<std::string>::iterator iParam = parameterNames.begin();
        iParam != parameterNames.end(); iParam++ ){
    if ( (*iParam) == "Debug" ) _Debug =  myHltParams.getParameter<bool>( *iParam );
  }

  dummyBranches_ = pSet.getUntrackedParameter<std::vector<std::string> >("dummyBranches",std::vector<std::string>(0));

  HltEvtCnt = 0;
  const int kMaxTrigFlag = 10000;
  trigflag = new bool[kMaxTrigFlag];
  trigPrescl = new int[kMaxTrigFlag];

  L1EvtCnt = 0;
  const int kMaxL1Flag = 10000;
  l1flag = new bool[kMaxL1Flag];
  l1Prescl = new int[kMaxL1Flag];

  l1mumax_pt     = -1.;
  l1mumax_eta    = -1.;
  l1mumax_phi    = -1.;
  l1egmax_pt     = -1.;
  l1egmax_eta    = -1.;
  l1egmax_phi    = -1.;
  l1isoegmax_pt  = -1.;
  l1isoegmax_eta = -1.;
  l1isoegmax_phi = -1.;
  l1jetmax_pt    = -1.;
  l1jetmax_eta   = -1.;
  l1jetmax_phi   = -1.;
  l1taumax_pt    = -1.;
  l1taumax_eta   = -1.;
  l1taumax_phi   = -1.;
  l1totalet_pt   = -1.;
  l1totalht_pt   = -1.;
  l1missinght_pt   = -1.;
  l1missinght_eta  = -1.;
  l1missinght_phi  = -1.;
 
  algoBitToName = new TString[512];
}

/* **Analyze the event** */
void HLTInfo::analyze(const edm::Handle<edm::TriggerResults>      & hltresults,
		      const edm::Handle<GlobalAlgBlkBxCollection> & l1results,
		      const edm::Handle<l1t::MuonBxCollection>    & l1muons,
		      const edm::Handle<l1t::EGammaBxCollection>  & l1egamma,
		      const edm::Handle<l1t::JetBxCollection>     & l1jets,
		      const edm::Handle<l1t::TauBxCollection>     & l1taus,
		      const edm::Handle<l1t::EtSumBxCollection>   & l1etsums,
		      edm::EventSetup const& eventSetup,
		      edm::Event const& iEvent,
                      TTree* HltTree) {

  if(_Debug) std::cout << " Beginning HLTInfo " << std::endl;

  /////////// Analyzing HLT Trigger Results (TriggerResults) //////////
  if (hltresults.isValid()) {
    int ntrigs = hltresults->size();
    if (ntrigs==0){std::cout << "%HLTInfo -- No trigger name given in TriggerResults of the input " << std::endl;}

    edm::TriggerNames const& triggerNames = iEvent.triggerNames(*hltresults);

    // 1st event : Book as many branches as trigger paths provided in the input...
    if (HltEvtCnt==0){
      for (int itrig = 0; itrig != ntrigs; ++itrig) {
        TString trigName = triggerNames.triggerName(itrig);
        HltTree->Branch(trigName,trigflag+itrig,trigName+"/B");
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
	  HltTree->Branch(trigName,trigflag+itdum,trigName+"/B");
	  HltTree->Branch(trigName+"_Prescl",trigPrescl+itdum,trigName+"_Prescl/I");
	  trigflag[itdum] = false;
	  trigPrescl[itdum] = 0;
	  ++itdum;
	}
      }

      HltEvtCnt++;
    }
    // ...Fill the corresponding accepts in branch-variables

    if(_Debug) std::cout << "Number of HLT paths: " << hltresults->size() << std::endl;

    for (int itrig = 0; itrig != ntrigs; ++itrig){

      std::string trigName=triggerNames.triggerName(itrig);
      bool accept = hltresults->accept(itrig);

      trigPrescl[itrig] = hltPrescaleProvider_->prescaleValue(iEvent, eventSetup, trigName);

      if (accept) trigflag[itrig] = true;
      else trigflag[itrig] = false;

      if (_Debug){
        if (_Debug) std::cout << "%HLTInfo --  Number of HLT Triggers: " << ntrigs << std::endl;
        std::cout << "%HLTInfo --  HLTTrigger(" << itrig << "): " << trigName << " = " << accept << std::endl;
      }
    }
  }
  else { if (_Debug) std::cout << "%HLTInfo -- No Trigger Result" << std::endl;}


  //==============L1 information=======================================

  // L1 Triggers from Menu
  L1GtUtils const& l1GtUtils = hltPrescaleProvider_->l1GtUtils();

  edm::ESHandle<L1TUtmTriggerMenu> menu;
  eventSetup.get<L1TUtmTriggerMenuRcd>().get(menu);

  int iErrorCode = -1;
  L1GtUtils::TriggerCategory trigCategory = L1GtUtils::AlgorithmTrigger;
  const int pfSetIndexAlgorithmTrigger = l1GtUtils.prescaleFactorSetIndex(iEvent, trigCategory, iErrorCode);
  if(_Debug){
    if (iErrorCode == 0) std::cout << "%Prescale set index: " << pfSetIndexAlgorithmTrigger  << std::endl;
    else std::cout << "%Could not extract Prescale set index from event record. Error code: " << iErrorCode << std::endl;
  }

  // 1st event : Book as many branches as trigger paths provided in the input...
  if (l1results.isValid()) {  
    
    int ntrigs = l1results->size();
    if(ntrigs==0) std::cout << "%L1Results -- No trigger name given in TriggerResults of the input " << std::endl;

    if(L1EvtCnt==0){

      // get the bit/name association         
      for (auto const & keyval: menu->getAlgorithmMap()) { 
	std::string const & trigName  = keyval.second.getName(); 
	unsigned int index = keyval.second.getIndex(); 
	if (_Debug) std::cerr << "bit: " << index << "\tname: " << trigName << std::endl;                                                              
	  
	int itrig = index;
 	algoBitToName[itrig] = TString( trigName );
	
	TString l1trigName= std::string (algoBitToName[itrig]); 
	std::string l1triggername = std::string (algoBitToName[itrig]); 

	HltTree->Branch(l1trigName,l1flag+itrig,l1trigName+"/B");
        HltTree->Branch(l1trigName+"_Prescl",l1Prescl+itrig,l1trigName+"_Prescl/I");
      } // end algo Map


      HltTree->Branch("L1muon_pt",&l1mumax_pt,"L1muon_pt/F");
      HltTree->Branch("L1muon_eta",&l1mumax_eta,"L1muon_eta/F");
      HltTree->Branch("L1muon_phi",&l1mumax_phi,"L1muon_phi/F");

      HltTree->Branch("L1egamma_pt",&l1egmax_pt,"L1egamma_pt/F");
      HltTree->Branch("L1egamma_eta",&l1egmax_eta,"L1egamma_eta/F");
      HltTree->Branch("L1egamma_phi",&l1egmax_phi,"L1egamma_phi/F");

      HltTree->Branch("L1isoegamma_pt",&l1isoegmax_pt,"L1isoegamma_pt/F");
      HltTree->Branch("L1isoegamma_eta",&l1isoegmax_eta,"L1isoegamma_eta/F");
      HltTree->Branch("L1isoegamma_phi",&l1isoegmax_phi,"L1isoegamma_phi/F");

      HltTree->Branch("L1jet_pt",&l1jetmax_pt,"L1jet_pt/F");
      HltTree->Branch("L1jet_eta",&l1jetmax_eta,"L1jet_eta/F");
      HltTree->Branch("L1jet_phi",&l1jetmax_phi,"L1jet_phi/F");

      HltTree->Branch("L1tau_pt",&l1taumax_pt,"L1tau_pt/F");
      HltTree->Branch("L1tau_eta",&l1taumax_eta,"L1tau_eta/F");
      HltTree->Branch("L1tau_phi",&l1taumax_phi,"L1tau_phi/F");

      HltTree->Branch("L1TotalEt_pt",&l1totalet_pt,"L1TotalEt_pt/F");
      HltTree->Branch("L1TotalHt_pt",&l1totalht_pt,"L1TotalHt_pt/F");

      HltTree->Branch("L1MissingHt_pt",&l1missinght_pt,"L1MissingHt_pt/F");
      HltTree->Branch("L1MissingHt_eta",&l1missinght_eta,"L1MissingHt_eta/F");
      HltTree->Branch("L1MissingHt_phi",&l1missinght_phi,"L1MissingHt_phi/F");

      L1EvtCnt++;     
    } // end l1evtCnt=0
  
    GlobalAlgBlk const &result = l1results->at(0,0);

    // get the individual decisions from the GlobalAlgBlk
    for (unsigned int itrig = 0; itrig < result.maxPhysicsTriggers; ++itrig) {
      //      std::cerr << "bit: " << itrig << "\tresult: " << results.getAlgoDecisionFinal(itrig) << std::endl;

      bool myflag = result.getAlgoDecisionFinal(itrig); 
      if (myflag) l1flag[itrig] = true;
      else l1flag[itrig] = false;

      std::string l1triggername= std::string (algoBitToName[itrig]);
      l1Prescl[itrig] = l1GtUtils.prescaleFactor(iEvent, 
						 l1triggername, 
						 iErrorCode);      

      if (_Debug) std::cout << "L1 TD: " << itrig << " " << algoBitToName[itrig] << " " 
			    << l1flag[itrig] << " " << l1Prescl[itrig] << std::endl;           
    }

    if(_Debug) std::cout << "Now find the ranking single L1 objects and save them" << std::endl;

    // Get highest pt object for each collection //
    if(l1muons.isValid()) get_max_l1_info(*l1muons,  l1mumax_pt,    l1mumax_eta,    l1mumax_phi, false, true);
    else if(_Debug)       std::cout <<  "ERROR: L1 Objects -- L1Muons not found!" << std::endl;

    if(l1egamma.isValid()) {
      get_max_l1_info(*l1egamma, l1egmax_pt,    l1egmax_eta,    l1egmax_phi);
      get_max_l1_info(*l1egamma, l1isoegmax_pt, l1isoegmax_eta, l1isoegmax_phi, true);
    }
    else if(_Debug) std::cout <<  "ERROR: L1 Objects -- L1EGammas not found!" << std::endl;
    
    if(l1jets.isValid()) get_max_l1_info(*l1jets,   l1jetmax_pt,   l1jetmax_eta,   l1jetmax_phi);
    else if(_Debug)      std::cout <<  "ERROR: L1 Objects -- L1Jets not found!" << std::endl;
    
    if(l1taus.isValid()) get_max_l1_info(*l1taus,   l1taumax_pt,   l1taumax_eta,   l1taumax_phi);
    else if(_Debug)      std::cout <<  "ERROR: L1 Objects --  L1Taus not found!" << std::endl;

    if(_Debug) std::cout << "Finished with the objects, now the sums " << std::endl;

    if(l1etsums.isValid()) {
      for(std::vector<l1t::EtSum>::const_iterator sum_obj = l1etsums->begin(0); sum_obj != l1etsums->end(); sum_obj++){
	if(sum_obj->getType() == l1t::EtSum::EtSumType::kTotalEt){
	  l1totalet_pt  = sum_obj->pt();
	}
	else if(sum_obj->getType() == l1t::EtSum::EtSumType::kTotalHt){
	  l1totalht_pt  = sum_obj->pt();
	}
	else if(sum_obj->getType() == l1t::EtSum::EtSumType::kMissingHt){
	  l1missinght_pt  = sum_obj->pt();
	  l1missinght_eta = sum_obj->eta();
	  l1missinght_phi = sum_obj->phi();
	}
      }
    }
    else if(_Debug)      std::cout <<  "ERROR: L1 Objects --  L1Sums not found!" << std::endl;

    //    L1EvtCnt++;
    if (_Debug) std::cout << "%L1Info -- Done with routine" << std::endl;                                                                         

  } // l1results.isValid
  else { if (_Debug) std::cout << "%L1Results -- No Trigger Result" << std::endl;}                                                                             
}
