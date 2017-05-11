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
Int_t    VERBOSE = 1; // full verbosity = 3
Bool_t   TEST    = false; // to test with NTEST entries per process
Int_t    NTEST   = 1000000;
Double_t CONVERT = 1e-36;

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

Int_t rate(UInt_t minPU=54, UInt_t maxPU=58, Double_t lumi=2.0e34, TString input="All")
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
  // processes.push_back("QCD15");  // actually not needed for rate studies
  processes.push_back("QCD30");     
  processes.push_back("QCD50");     
  processes.push_back("QCD80");     
  processes.push_back("QCD120");    
  processes.push_back("QCD170");    
  processes.push_back("QCD300");    
  processes.push_back("QCD470");    
  //
  processes.push_back("QCDEM15");    
  processes.push_back("QCDEM20");    
  processes.push_back("QCDEM30");    
  processes.push_back("QCDEM50");    
  processes.push_back("QCDEM80");    
  processes.push_back("QCDEM120");   
  //
  processes.push_back("QCDMu15");    
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
  subdir["DYToLL"].push_back("/DYToLL_M_1_TuneCUETP8M1_13TeV_pythia8/DYToLL/170428_144342/0000/");
  subdir["WJets" ].push_back("/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/WJets/170428_144318/0000/");
  subdir["QCD15"   ].push_back("C/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/QCD15/170502_141753/0000/");
  subdir["QCD15"   ].push_back("C/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/QCD15/170502_141753/0001/");
  subdir["QCD15"   ].push_back("C/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/QCD15/170502_141753/0002/");
  subdir["QCD30"   ].push_back("B/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30/170428_144919/0000/");
  subdir["QCD30"   ].push_back("B/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30/170428_144919/0001/");
  subdir["QCD30"   ].push_back("B/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/QCD30/170428_144919/0002/");
  subdir["QCD50"   ].push_back("B/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/QCD50/170428_144852/0000/");
  subdir["QCD50"   ].push_back("B/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/QCD50/170428_144852/0001/");
  subdir["QCD50"   ].push_back("B/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/QCD50/170428_144852/0002/");
  subdir["QCD80"   ].push_back("B/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/QCD80/170428_144824/0000/");
  subdir["QCD120"  ].push_back("/QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8/QCD120/170428_144518/0000/");
  subdir["QCD170"  ].push_back("/QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/QCD170/170428_144454/0000/");
  subdir["QCD300"  ].push_back("/QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8/QCD300/170428_144431/0000/");
  subdir["QCD470"  ].push_back("/QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8/QCD470/170428_144406/0000/");
  subdir["QCDMu15" ].push_back("B/QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu15/170428_145222/0000/");
  subdir["QCDMu20" ].push_back("B/QCD_Pt-20to30_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu20/170428_145153/0000/");
  subdir["QCDMu30" ].push_back("C/QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu30/170502_141815/0000/");
  subdir["QCDMu50" ].push_back("B/QCD_Pt-50to80_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu50/170428_145059/0000/");
  subdir["QCDMu80" ].push_back("B/QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu80/170428_145032/0000/");
  subdir["QCDMu120"].push_back("B/QCD_Pt-120to170_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/QCDMu120/170428_145009/0000/");
  subdir["QCDEM15" ].push_back("C/QCD_Pt-15to20_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM15/170502_141928/0000/");
  subdir["QCDEM20" ].push_back("B/QCD_Pt-20to30_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM20/170428_145450/0000/");
  subdir["QCDEM30" ].push_back("C/QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM30/170502_141901/0000/");
  subdir["QCDEM50" ].push_back("B/QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM50/170428_145402/0000/");
  subdir["QCDEM80" ].push_back("B/QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM80/170428_145337/0000/");
  subdir["QCDEM120"].push_back("B/QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8/QCDEM120/170428_145305/0000/");

  // Define paths to root files
  TString pathEOS="/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/83X_tests/HLTRates_1p25e34_2017apr28";

  // Make the chains
  TChain* chTemp;
  TString filePath;
  vector<TString> vTemp;
  //
  for(UInt_t iP=0; iP<processes.size(); iP++) { 

    vTemp  = subdir[processes[iP]];
    chTemp = new TChain("HltTree");

    if(VERBOSE>0) cout << "-- Filling chain for process: " << processes[iP] << endl;

    // Loop over subdirectories for process iP and add all root files
    for(UInt_t iS=0; iS<vTemp.size(); iS++) {

      filePath = pathEOS+vTemp[iS]+"/hltbits_*.root";

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

  // returns xsec * filter efficiency

  if(     process=="QCD15")     return 1.706E+09 ;
  else if(process=="QCD30")     return 1.518E+08 ;
  else if(process=="QCD50")     return 1.972E+07 ;
  else if(process=="QCD80")     return 2.734E+06 ;
  else if(process=="QCD120")    return 4.820E+05 ; 
  else if(process=="QCD170")    return 1.139E+05 ; 
  else if(process=="QCD300")    return 8.157E+03 ;
  else if(process=="QCD470")    return 6.467E+02 ; 
  //
  else if(process=="QCDEM15")    return 1.270E+09 * 1.099E-03 ;
  else if(process=="QCDEM20")    return 5.607E+08 * 1.038E-02 ;
  else if(process=="QCDEM30")    return 1.392E+08 * 5.073E-02 ;
  else if(process=="QCDEM50")    return 1.930E+07 * 1.103E-01 ;
  else if(process=="QCDEM80")    return 2.805E+06 * 1.771E-01 ;
  else if(process=="QCDEM120")   return 4.846E+05 * 1.521E-01 ;
  //
  else if(process=="QCDMu15")    return 1.273E+09 * 2.340E-03 ;
  else if(process=="QCDMu20")    return 5.621E+08 * 6.704E-03 ;
  else if(process=="QCDMu30")    return 1.392E+08 * 1.131E-02 ;
  else if(process=="QCDMu50")    return 1.947E+07 * 2.214E-02 ;
  else if(process=="QCDMu80")    return 2.739E+06 * 4.086E-02 ;
  else if(process=="QCDMu120")   return 4.754E+05 * 5.033E-02 ;
  //
  else if(process=="WJets")      return 50700 ;
  else if(process=="DYToLL")     return 1.887E+04 ; 

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
