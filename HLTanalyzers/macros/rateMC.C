#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <utility>
//
#include "TChain.h"
#include "TString.h"
#include "TCut.h"
#include "TEntryList.h"
#include "TDirectory.h"
#include "TMath.h"
//
using namespace std;
int    VERBOSE = 1; // full verbosity = 3
bool   TEST    = false; // to test with NTEST entries per process
int    NTEST   = 1000000;
double CONVERT = 1e-36;

///////////////////////
// Declare functions //
///////////////////////

Double_t GetXSec(TString process);
Int_t    DefineChains(vector<TString> &processes, vector<TChain*> &chains);
TCut     DefineCut(TString process, UInt_t minPU, UInt_t maxPU);
Int_t    DefineCuts(vector<TString> processes, vector<TCut> &cuts, UInt_t minPU, UInt_t maxPU);
Int_t    DefinePaths(vector<TString> &paths);


///////////////////
// Main function //
///////////////////

Int_t rate(UInt_t minPU=38, UInt_t maxPU=42, Double_t lumi=1.25e34, TString input="All")
{

  // Define the chains
  vector<TChain*> chains;
  vector<TString> processes;
  DefineChains(processes, chains);

  // Define the cuts
  vector<TCut> cuts;
  DefineCuts(processes, cuts, minPU, maxPU);

  // Define paths to study
  vector<TString> paths;
  DefinePaths(paths);

  // Check vector sizes
  const UInt_t nP    = paths    .size();
  const UInt_t nCh   = chains   .size();
  const UInt_t nProc = processes.size();
  const UInt_t nCuts = cuts     .size();
  if( nCh!=nProc || nCh!=nCuts || nProc!=nCuts ) {
    cout << "ERROR: sizes do not match!" << endl;
    return -1;
  }

  // Check if the chains get defined correctly
  cout << "- Defined " << nProc << " processes." << endl;
  if(VERBOSE>1) {
    for(UInt_t iP=0; iP<nProc; iP++) {
      cout << processes[iP]     << endl
	   << "   => Entries: " << chains[iP]->GetEntries() << endl
	   << "   => Cut: "     << cuts[iP] << endl;
    }
    cout << endl << endl;
  }

  // Print-out paths
  if(VERBOSE>0) {
    cout << "- Paths to be studied:" << endl;
    for(UInt_t iP=0; iP<paths.size(); iP++) {
      cout << "-- " << paths[iP] << endl;
    }
    cout << endl << endl;
  }

  // Initialize counters
  Long64_t den[nCh];
  Long64_t num[nCh][nP];
  for(UInt_t iCh=0; iCh<nCh; iCh++) {
    den[iCh]=0;
    for(UInt_t iP=0; iP<nP; iP++) {
      num[iCh][iP]=0;
    }
  }

  // Count entries
  TEntryList* skim;
  TString     tskim;
  TCut        thePath;
  //
  cout << "- Start looping over chains to count events" << endl;
  //
  for(UInt_t iCh=0; iCh<nCh; iCh++) {

    // look only at the requested process
    // unless "All" is specified for parameter "input"
    if(input!="All" && input!=processes[iCh]) continue; 

    cout << "-- process: " << processes[iCh] << endl;

    // Skim the chain with denominator events only
    tskim    = "skim_"+processes[iCh];
    //
    if(TEST) {
      den[iCh] = chains[iCh]->Draw(">>+"+tskim, cuts[iCh], "entrylist", NTEST); // test with NTEST entries
    }
    else {
      den[iCh] = chains[iCh]->Draw(">>+"+tskim, cuts[iCh], "entrylist");
    }
    //
    skim     = (TEntryList*)gDirectory->Get(tskim);
    //
    if(skim) { 
      chains[iCh]->SetEntryList(skim); 
      if(VERBOSE>2) cout << "-- skim size: " << skim->GetN() << " entries ; den[iCh]=" << den[iCh] << endl;
    }

    // Get one numerator per path
    for(UInt_t iP=0; iP<nP; iP++) {
      thePath = TCut(paths[iP]);
      num[iCh][iP] = chains[iCh]->GetEntries( thePath );
    }

  } // end loop over chains

  // Initialize rates
  Double_t totalRate[nP];
  Double_t totalErr[nP];
  Double_t rate[nCh][nP];
  Double_t rateErr[nCh][nP];
  Double_t fraction, xsec;
  //
  fraction = xsec = 0;
  for(UInt_t iP=0; iP<nP; iP++) {
    totalRate[iP]=0;
    totalErr[iP] =0;
    for(UInt_t iCh=0; iCh<nCh; iCh++) {
      rate   [iCh][iP]=0;
      rateErr[iCh][iP]=0;
    }
  }

  // Evaluate rates
  for(UInt_t iP=0; iP<nP; iP++) {
    for(UInt_t iCh=0; iCh<nCh; iCh++) {
      xsec          = GetXSec(processes[iCh]);
      fraction      = den[iCh]!=0 ? num[iCh][iP] / ((Double_t)den[iCh]) : 0;
      //
      rate   [iCh][iP] = xsec * CONVERT * lumi * fraction;
      rateErr[iCh][iP] = den[iCh]!=0 ? xsec * CONVERT * lumi * TMath::Sqrt( num[iCh][iP] ) / ((Double_t)den[iCh]) : 0;
      //
      totalRate[iP]+= rate[iCh][iP];
      totalErr[iP] += TMath::Power(rateErr[iCh][iP], 2);
    }
    totalErr[iP] = TMath::Sqrt( totalErr[iP] );
  }

  // Print-out rates
  cout << endl << "=== RATES ===" << endl;
  for(UInt_t iP=0; iP<nP; iP++) {
    cout << "- " << paths[iP] << " : " << totalRate[iP] << " +/- " << totalErr[iP] << endl;
  }
  cout << "=============" << endl;

  return 0;
}


// Define list of trigger paths to be studied //
Int_t DefinePaths(vector<TString> &paths)
{

  paths.push_back("HLT_DiCentralPFJet170_CFMax0p1_v5");
  paths.push_back("HLT_DiCentralPFJet330_CFMax0p5_v5");
  paths.push_back("HLT_DiCentralPFJet430_v5");

  return 0;
}

// Define TChains and add input root files //
Int_t DefineChains(vector<TString> &processes, vector<TChain*> &chains)
{

  if(VERBOSE>0) cout << "- Launch DefineChains()" << endl;

  // Define processes
  // processes.push_back("QCD15");  // ntuples exist but this process is useless for rates
  processes.push_back("QCD30");     
  processes.push_back("QCD30ext");  
  processes.push_back("QCD50");     
  processes.push_back("QCD50ext");  
  processes.push_back("QCD80");     
  processes.push_back("QCD80ext");  
  processes.push_back("QCD120");    
  processes.push_back("QCD170");    
  processes.push_back("QCD300");    
  processes.push_back("QCD470");    
  //
  processes.push_back("QCDEM15");    
  processes.push_back("QCDEM15ext"); 
  processes.push_back("QCDEM20");    
  processes.push_back("QCDEM30");    
  processes.push_back("QCDEM30ext"); 
  processes.push_back("QCDEM50");    
  processes.push_back("QCDEM80");    
  processes.push_back("QCDEM120");   
  //
  processes.push_back("QCDMu15");    
  processes.push_back("QCDMu15ext"); 
  processes.push_back("QCDMu20");    
  processes.push_back("QCDMu30");    
  processes.push_back("QCDMu50");    
  processes.push_back("QCDMu80");    
  processes.push_back("QCDMu120");   
  //
  processes.push_back("WJets");      
  processes.push_back("DYToLL");     

  // List subdirectories for each process
  map< TString , vector<TString> > subdir;
  //
  subdir["DYToLL"].push_back("DYToLL_M_1_TuneCUETP8M1_13TeV_pythia8/DYToLL/170224_102942/0000/"); 
  subdir["WJets" ].push_back("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/WJets/170224_102918/0000/"); 
  //
  subdir["QCD15"   ].push_back("QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/QCD15/170224_103523/0000/");  
  subdir["QCD15"   ].push_back("QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/QCD15/170224_103523/0001/");  
  subdir["QCD30"   ].push_back("QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30/170224_103458/0000/");  
  subdir["QCD30ext"].push_back("QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30ext/170224_103343/0000/");
  subdir["QCD30ext"].push_back("QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30ext/170224_103343/0001/");
  subdir["QCD50"   ].push_back("QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/QCD50/170224_103316/0000/");  
  subdir["QCD50ext"].push_back("QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/QCD50ext/170224_103247/0000/");
  subdir["QCD80"   ].push_back("QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/QCD80/170224_103219/0000/"); 
  subdir["QCD80ext"].push_back("QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/QCD80ext/170224_103153/0000/");
  subdir["QCD120"  ].push_back("QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8/QCD120/170224_103129/0000/");
  subdir["QCD170"  ].push_back("QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/QCD170/170224_103102/0000/");
  subdir["QCD300"  ].push_back("QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8/QCD300/170224_103033/0000/");
  subdir["QCD470"  ].push_back("QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8/QCD470/170224_103007/0000/");
  //
  subdir["QCDMu15"   ].push_back("QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu15/170224_103820/0000/"); 
  subdir["QCDMu15ext"].push_back("QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu15ext/170224_103751/0000/"); 
  subdir["QCDMu20"   ].push_back("QCD_Pt-20to30_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu20/170224_103727/0000/"); 
  subdir["QCDMu30"   ].push_back("QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu30/170224_103701/0000/"); 
  subdir["QCDMu50"   ].push_back("QCD_Pt-50to80_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu50/170224_103638/0000/"); 
  subdir["QCDMu80"   ].push_back("QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu80/170224_103614/0000/");
  subdir["QCDMu120"  ].push_back("QCD_Pt-120to170_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu120/170224_103548/0000/");
  //
  subdir["QCDEM15"   ].push_back("QCD_Pt-15to20_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM15/170224_104144/0000/"); 
  subdir["QCDEM15ext"].push_back("QCD_Pt-15to20_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM15ext/170224_104119/0000/"); 
  subdir["QCDEM20"   ].push_back("QCD_Pt-20to30_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM20/170224_104055/0000/"); 
  subdir["QCDEM30"   ].push_back("QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM30/170224_104029/0000/"); 
  subdir["QCDEM30ext"].push_back("QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM30ext/170224_104004/0000/");
  subdir["QCDEM50"   ].push_back("QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM50/170224_103936/0000/"); 
  subdir["QCDEM80"   ].push_back("QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM80/170224_103911/0000/");
  subdir["QCDEM120"  ].push_back("QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM120/170227_093827/0000/");

  // Define paths to root files
  TString pathEOS1="/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/Summer16_FlatPU28to62/HLTRates_v4p2_V2_1p35e34_MC_2017feb24A/";
  TString pathEOS2="/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/Summer16_FlatPU28to62/HLTRates_v4p2_V2_1p35e34_MC_2017feb24C/";

  // Make the chains
  TChain* chTemp;
  TString filePath;
  TString pathEOS;
  vector<TString> vTemp;
  //
  for(UInt_t iP=0; iP<processes.size(); iP++) { 

    vTemp  = subdir[processes[iP]];
    chTemp = new TChain("HltTree");

    // QCDEM120 is the only one in pathEOS2 for the set of ntuples being processed here
    if(processes[iP]=="QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8") {
      pathEOS = pathEOS2;
    }
    else {
      pathEOS = pathEOS1;
    }

    if(VERBOSE>0) cout << "-- Filling chain for process: " << processes[iP] << endl;

    // Loop over subdirectories for process iP and add all root files
    for(UInt_t iS=0; iS<vTemp.size(); iS++) {

      filePath = pathEOS+"/"+vTemp[iS]+"/hltbits_*.root";

      chTemp->Add(filePath);
      if(VERBOSE>2) cout << "--- added: " << filePath << endl;
    }

    if(VERBOSE>2) cout << "-- entries: " << chTemp->GetEntries() << endl;

    chains.push_back(chTemp);
  }

  return 0;
}

// Get the cross section for each process //
Double_t GetXSec(TString process)
{

  // - generic case: returns xsec * McM efficiency
  // - particular case: for processes that have an extended dataset,
  //   add a third factor to reweight wrt #generated events

  // When processing NTEST events for all processes, 
  // the third factor is 0.5 as the same #events is used 
  // in both versions of the dataset
  if(TEST) {
    if(     process=="QCDEM15")    return 1279000000.0 *0.0018 *0.5;
    else if(process=="QCDEM15ext") return 1279000000.0 *0.0018 *0.5;
    else if(process=="QCDEM30")    return 136000000.0  *0.073  *0.5;
    else if(process=="QCDEM30ext") return 136000000.0  *0.073  *0.5;
    else if(process=="QCDMu15")    return 1279000000.0 *0.003  *0.5;
    else if(process=="QCDMu15ext") return 1279000000.0 *0.003  *0.5;
  }

  // Generic case (process all events)
  if(     process=="QCD15")     return 1837410000;
  else if(process=="QCD30")     return 140932000.*0.2512; 
  else if(process=="QCD30ext")  return 140932000.*0.7488;
  else if(process=="QCD50")     return 19204300. *0.2513; 
  else if(process=="QCD50ext")  return 19204300. *0.7487;
  else if(process=="QCD80")     return 2762530.  *0.6839; 
  else if(process=="QCD80ext")  return 2762530   *0.3161;
  else if(process=="QCD120")    return 471100; 
  else if(process=="QCD170")    return 117276; 
  else if(process=="QCD300")    return 7823; 
  else if(process=="QCD470")    return 648.2; 
  //
  else if(process=="QCDEM15")    return 1279000000.0 *0.0018 *0.6969;
  else if(process=="QCDEM15ext") return 1279000000.0 *0.0018 *0.3031;
  else if(process=="QCDEM20")    return 557600000.0  *0.0096;
  else if(process=="QCDEM30")    return 136000000.0  *0.073  *0.4073;
  else if(process=="QCDEM30ext") return 136000000.0  *0.073  *0.5927;
  else if(process=="QCDEM50")    return 19800000.0   *0.146;
  else if(process=="QCDEM80")    return 2800000.0    *0.125;
  else if(process=="QCDEM120")   return 477000.0     *0.132;
  //
  else if(process=="QCDMu15")    return 1279000000.0 *0.003  *0.4999;
  else if(process=="QCDMu15ext") return 1279000000.0 *0.003  *0.5001;
  else if(process=="QCDMu20")    return 557600000.0  *0.0053;
  else if(process=="QCDMu30")    return 136000000.0  *0.01182;
  else if(process=="QCDMu50")    return 19800000.0   *0.02276;
  else if(process=="QCDMu80")    return 2800000.0    *0.03844;
  else if(process=="QCDMu120")   return 477000.0     *0.05362;
  //
  else if(process=="WJets")      return 50700; 
  else if(process=="DYToLL")     return 20000; 

  else return -1;
}

// Define the cut to be used when counting denominator/numerator events //
TCut DefineCut(TString process, UInt_t minPU, UInt_t maxPU)
{

  // PU filter
  TCut cut_PU = "HLT_RemovePileUpDominatedEventsGen_v1";

  // GEN filter
  TCut cut_GEN = "";

  if(process.Contains("QCD")) {

    if(process.Contains("QCDMu")) {
      cut_GEN = "HLT_MuFilter_v1 && !(!HLT_BCToEFilter_v1 && HLT_EmFilter_v1)";
    }

    else if(process.Contains("QCDEM")) {
      cut_GEN = "!HLT_BCToEFilter_v1 && HLT_EmFilter_v1";
    }

    else if( !process.Contains("QCD170") && !process.Contains("QCD300") && !process.Contains("QCD470") ) {
      cut_GEN = "!HLT_MuFilter_v1 && !(!HLT_BCToEFilter_v1 && HLT_EmFilter_v1)";
    }

  }

  // PU range
  ostringstream oss_minPU("");
  oss_minPU << minPU;
  TString string_minPU = oss_minPU.str();
  //
  ostringstream oss_maxPU("");
  oss_maxPU << maxPU;
  TString string_maxPU = oss_maxPU.str();
  //
  TString string_RangePU = "NPUTrueBX0>"+string_minPU+" && NPUTrueBX0<"+string_maxPU;
  TCut cut_RangePU(string_RangePU);

  return cut_PU * cut_GEN * cut_RangePU;
}

// Launch DefineCut() for each process //
Int_t DefineCuts(vector<TString> processes, vector<TCut> &cuts, UInt_t minPU, UInt_t maxPU)
{

  TCut theCut;
  for(uint iP=0; iP<processes.size(); iP++) {
    theCut = DefineCut(processes[iP], minPU, maxPU);
    cuts.push_back(theCut);
  }
  
  return 0;
}
