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
from hlt import *

### STEAM Filters ###

# GEN-based PU filter
if not options.isZeroBias and options.isMC :
    process.RemovePileUpDominatedEventsGen = cms.EDFilter("RemovePileUpDominatedEventsGen")
    process.HLT_RemovePileUpDominatedEventsGen_v1 = cms.Path( process.HLTBeginSequence + process.RemovePileUpDominatedEventsGen + process.HLTEndSequence )

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
    process.L1TGlobalPrescalesVetos.PrescaleXMLFile = cms.string('UGT_BASE_RS_PRESCALES_v214.xml')
    process.L1TGlobalPrescalesVetos.FinOrMaskXMLFile = cms.string('UGT_BASE_RS_FINOR_MASK_v91.xml')
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
process.hltbitanalysis.RunParameters.HistogramFile = cms.untracked.string('hltbits.root')                           
process.hltbitanalysis.RunParameters.isData = cms.bool(not options.isMC)
process.hltbitanalysis.RunParameters.Debug = cms.bool(False)
process.HLTBitAnalysisEndpath = cms.EndPath(process.hltbitanalysis)
