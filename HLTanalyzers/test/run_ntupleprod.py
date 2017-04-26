import FWCore.ParameterSet.VarParsing as VarParsing
import sys

options = VarParsing.VarParsing()

options.register('isZeroBias', False, #default value
                 VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"")

options.register('isMC', True, #default value
                 VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"")

options.register('doL1Prescales', True, #default value
                 VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"")

options.register('L1PrescaleColumn', 4, #default value
                 VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"")

options.parseArguments()

print "Am I preparing to run on MC? -> ", options.isMC
print "Will I apply prescales to L1? -> ", options.doL1Prescales
if options.doL1Prescales :
    print "I will use the L1 prescale column n. ", options.L1PrescaleColumn

###Remove the options that will confuse hltGetConfiguration
restart = True
while(restart):
    for el_idx,el in enumerate(sys.argv):
        if ("isZeroBias" in el) or ("isMC" in el) or ("doL1Prescales" in el) or ("L1PrescaleColumn" in el):
            del sys.argv[el_idx]
            restart = True
            break
        restart = False

###Import the content of hltGetConfiguration
if options.isMC:
    from hlt_MC import *
else:
    from hlt_data import *

### STEAM Filters ###
if not options.isZeroBias and options.isMC :

    # PU Filters #

    # Define the EDFilters
    process.RemovePileUpDominatedEventsGen = cms.EDFilter("RemovePileUpDominatedEventsGen")

    process.RemovePileUpDominatedEventsGenV2 = cms.EDFilter(
        "RemovePileUpDominatedEventsGenV2",
        fileListFolder = cms.string("/afs/cern.ch/user/s/sdonato/AFSwork/public/genJetPtHadPU_RunIISummer15GS_ak4GenJetsNoNu"),
        genJets = cms.InputTag("ak4GenJetsNoNu"),
        pileupSummaryInfos = cms.InputTag("addPileupInfo"),
        )

    process.RemovePileUpDominatedEvents = cms.EDFilter(
        "RemovePileUpDominatedEvents",
        genjets = cms.InputTag("ak4GenJetsNoNu"),
        l1jets = cms.InputTag("hltGtStage2Digis","Jet"),
        l1taujets = cms.InputTag("hltGtStage2Digis","Tau"),
        l1muonjets = cms.InputTag("hltGtStage2Digis","Muon"),
        l1egs = cms.InputTag("hltGtStage2Digis","EGamma"),
        deltaR = cms.double(0.4),
        minGenjetPt = cms.double(10),
        )

    # Define the Paths
    process.HLT_RemovePileUpDominatedEventsGen_v1 = cms.Path( 
        process.HLTBeginSequence + process.RemovePileUpDominatedEventsGen + process.HLTEndSequence 
        )

    process.HLT_RemovePileUpDominatedEventsGenV2_v1 = cms.Path( 
        process.HLTBeginSequence + process.RemovePileUpDominatedEventsGenV2 + process.HLTEndSequence 
        )

    process.HLT_RemovePileUpDominatedEvents_v1 = cms.Path( 
        process.HLTBeginSequence + process.RemovePileUpDominatedEvents + process.HLTEndSequence 
        )

    # Add the particle filter as a fake trigger
    process.genParticlesForFilter = cms.EDProducer("GenParticleProducer",
                                                   saveBarCodes = cms.untracked.bool(True),
                                                   src = cms.InputTag("generatorSmeared"),
                                                   abortOnUnknownPDGCode = cms.untracked.bool(False)
                                                   )
    process.bctoefilter = cms.EDFilter("BCToEFilter",
                                       filterAlgoPSet = cms.PSet(eTThreshold = cms.double(10),
                                                                 genParSource = cms.InputTag("genParticlesForFilter")
                                                                 )
                                       )
    process.emenrichingfilter = cms.EDFilter("EMEnrichingFilter",
                                             filterAlgoPSet = cms.PSet(isoGenParETMin=cms.double(20.),
                                                                       isoGenParConeSize=cms.double(0.1),
                                                                       clusterThreshold=cms.double(20.),
                                                                       isoConeSize=cms.double(0.2),
                                                                       hOverEMax=cms.double(0.5),
                                                                       tkIsoMax=cms.double(5.),
                                                                       caloIsoMax=cms.double(10.),
                                                                       requireTrackMatch=cms.bool(False),
                                                                       genParSource = cms.InputTag("genParticlesForFilter")
                                                                       )
                                             )
    process.genmuons = cms.EDFilter("CandViewSelector",
                                    src = cms.InputTag("genParticles"),
                                    cut = cms.string("(abs(pdgId())==13) && (pt > 5.) && (abs(vz())<400.) && ((vx()*vx() + vy()*vy() )<(400.*400.)) && (abs(eta())<2.5)")
                                    )
    process.mufilter = cms.EDFilter("CandViewCountFilter",  src = cms.InputTag("genmuons"), minNumber = cms.uint32(1))

    process.HLT_BCToEFilter_v1=cms.Path(process.genParticlesForFilter + process.bctoefilter)
    process.HLT_EmFilter_v1=cms.Path(process.genParticlesForFilter + process.emenrichingfilter)
    process.HLT_EmGlobalFilter_v1=cms.Path(process.genParticlesForFilter + ~process.bctoefilter + process.emenrichingfilter)
    process.HLT_MuFilter_v1=cms.Path(process.genmuons + process.mufilter)

### END STEAM GEN Filters ###

if options.doL1Prescales :
    process.load('L1Trigger.L1TGlobal.hackConditions_cff')
    process.L1TGlobalPrescalesVetos.PrescaleXMLFile = cms.string('UGT_RS_CLOBS_UGT_BASE_RS_PRESCALES_v226_CONF.xml')
    process.L1TGlobalPrescalesVetos.FinOrMaskXMLFile = cms.string('UGT_RS_CLOBS_UGT_BASE_RS_FINOR_MASK_v91_CONF.xml')
    process.simGtStage2Digis.AlgorithmTriggersUnmasked = cms.bool(False)
    process.simGtStage2Digis.AlgorithmTriggersUnprescaled = cms.bool(False)
    process.simGtStage2Digis.PrescaleSet = cms.uint32(options.L1PrescaleColumn)

process.DQMOutput.remove(process.dqmOutput)

process.load("HLTrigger.HLTanalyzers.HLTBitAnalyser_cfi")
process.hltbitanalysis.HLTProcessName = cms.string(process.name_() )
process.hltbitanalysis.hltresults     = cms.InputTag( 'TriggerResults','',process.name_())
process.hltbitanalysis.l1results      = cms.InputTag('hltGtStage2Digis', '', process.name_())
process.hltbitanalysis.l1muons        = cms.InputTag('hltGtStage2Digis', 'Muon', process.name_())
process.hltbitanalysis.l1egamma       = cms.InputTag('hltGtStage2Digis', 'EGamma', process.name_())
process.hltbitanalysis.l1jets         = cms.InputTag('hltGtStage2Digis', 'Jet', process.name_())
process.hltbitanalysis.l1taus         = cms.InputTag('hltGtStage2Digis', 'Tau', process.name_())
process.hltbitanalysis.l1etsums       = cms.InputTag('hltGtStage2Digis', 'EtSum', process.name_())
process.hltbitanalysis.isL1MuHighQual = cms.bool(True)
process.hltbitanalysis.RunParameters.HistogramFile = cms.untracked.string('hltbits.root')                           
process.hltbitanalysis.RunParameters.isData = cms.bool(not options.isMC)
process.hltbitanalysis.RunParameters.Debug = cms.bool(False)
process.HLTBitAnalysisEndpath = cms.EndPath(process.hltbitanalysis)
