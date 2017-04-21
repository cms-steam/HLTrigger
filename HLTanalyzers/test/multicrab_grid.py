
name = 'HLTRates_testcleanup_2017mar24'
steam_dir = 'Summer16_FlatPU28to62'
running_options = ['isMC=True','doL1Prescales=True','L1PrescaleColumn=4']
runATCAF = False

dataset = {
   'SingleNu' : '/SingleNeutrino/RunIISummer16DR80-PUFlat0to70HcalNZSRAWAODSIM_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/RAWAODSIM',
   'DYToLL' : '/DYToLL_M_1_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v4/GEN-SIM-RAW',
   'WJets' : '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   
   'QCD15' : '/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCD30' : '/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCD30ext' : '/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/GEN-SIM-RAW',
   'QCD50' : '/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCD50ext' : '/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/GEN-SIM-RAW',
   'QCD80' : '/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/GEN-SIM-RAW',
   'QCD80ext' : '/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/GEN-SIM-RAW',
   'QCD120' : '/QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCD170' : '/QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCD300' : '/QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCD470' : '/QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   
   'QCDEM15' : '/QCD_Pt-15to20_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDEM15ext' : '/QCD_Pt-15to20_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/GEN-SIM-RAW',
   'QCDEM20' : '/QCD_Pt-20to30_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCDEM30' : '/QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDEM30ext' : '/QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/GEN-SIM-RAW',
   'QCDEM50' : '/QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/GEN-SIM-RAW',
   'QCDEM80' : '/QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDEM120' : '/QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   
   'QCDMu15' : '/QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDMu15ext' : '/QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/GEN-SIM-RAW',
   'QCDMu20' : '/QCD_Pt-20to30_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDMu30' : '/QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/GEN-SIM-RAW',
   'QCDMu50' : '/QCD_Pt-50to80_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCDMu80' : '/QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW',
   'QCDMu120' : '/QCD_Pt-120to170_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16DR80-FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW' 
   }


nevents = -1 
eventsPerJob = {
   'SingleNu':      20000,
   'QCDEM15':       20000,
   'QCDEM15ext':    10000,
   'QCDEM20':       20000,
   'QCDEM30':       20000,
   'QCDEM30ext':    20000,
   'QCDEM50':       20000,
   'QCDEM80':       5000,
   'QCDEM120':      1000,
   'QCDMu15':       20000,
   'QCDMu15ext':    20000,
   'QCDMu20':       20000,
   'QCDMu30':       20000,
   'QCDMu50':       10000,
   'QCDMu80':       10000,
   'QCDMu120':      2000,
   'QCD15':         20000,
   'QCD30':         20000,
   'QCD30ext':      20000,
   'QCD50':         20000,
   'QCD50ext':      20000,
   'QCD80':         20000,
   'QCD80ext':      10000,
   'QCD120':        10000,
   'QCD170':        2000,
   'QCD300':        2000,
   'QCD470':        2000,
   'DYToLL':        20000,
   'WJets':         10000
   }

listOfSamples = [
   'SingleNu',
   'QCDEM15',
   'QCDEM15ext',
   'QCDEM20',
   'QCDEM30',
   'QCDEM30ext',
   'QCDEM50',
   'QCDEM80',
   'QCDEM120',
   'QCDMu15',
   'QCDMu15ext',
   'QCDMu20',
   'QCDMu30',
   'QCDMu50',
   'QCDMu80',
   'QCDMu120',
   'QCD15',
   'QCD30',
   'QCD30ext',
   'QCD50',
   'QCD50ext',
   'QCD80',
   'QCD80ext',
   'QCD120',
   'QCD170',
   'QCD300',
   'QCD470',
   'DYToLL',
   'WJets'
   ]

if __name__ == '__main__':

   from CRABClient.UserUtilities import config
   config = config()

   from CRABAPI.RawCommand import crabCommand
   from multiprocessing import Process

   def submit(config):
       res = crabCommand('submit', config = config)

   config.General.workArea = 'crab_'+name
   config.General.transferLogs = True

   config.JobType.pluginName = 'Analysis'
   config.JobType.psetName = 'run_ntupleprod.py'
   config.JobType.maxMemoryMB = 2500
   config.JobType.numCores = 4
   config.JobType.inputFiles = ['/afs/cern.ch/user/s/sdonato/AFSwork/public/genJetPtHatPU/0.txt']
   config.JobType.outputFiles = ['hltbits.root']
   config.JobType.pyCfgParams = running_options

   config.Data.inputDBS = 'global'
   config.Data.splitting = 'EventAwareLumiBased'
   config.Data.publication = False
   config.Data.totalUnits = nevents
   config.Data.outLFNDirBase = '/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/' + steam_dir + name

   config.Site.storageSite = 'T2_CH_CERN'
   config.Site.blacklist = ['T2_BR_SPRACE', 'T2_US_Wisconsin', 'T1_RU_JINR', 'T2_RU_JINR', 'T2_EE_Estonia']
   if runATCAF :
      config.Site.whitelist = ['T3_CH_CERN_CAF']
      config.Site.ignoreGlobalBlacklist = True
      config.Data.ignoreLocality = True

   listOfSamples.reverse()
   for sample in listOfSamples:

      if sample == 'SingleNu':
         config.JobType.pyCfgParams.append('isZeroBias=True')

      config.General.requestName = sample
      config.Data.inputDataset = dataset[sample]
      config.Data.unitsPerJob = eventsPerJob[sample]
      config.Data.outputDatasetTag = sample
      #config.Data.lumiMask = mask
      p = Process(target=submit, args=(config,))
      p.start()
      p.join()

      if sample == 'SingleNu':
         config.JobType.pyCfgParams.remove('isZeroBias=True')
