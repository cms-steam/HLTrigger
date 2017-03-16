import FWCore.ParameterSet.Config as cms

hltbitanalysis = cms.EDAnalyzer("HLTBitAnalyzer",
    ### Trigger objects
    l1results = cms.InputTag("hltGtDigis::HLT"),                            
    l1muons   = cms.InputTag("hltGtDigis::HLT"),
    l1egamma  = cms.InputTag("hltGtDigis::HLT"),
    l1jets    = cms.InputTag("hltGtDigis::HLT"),
    l1taus    = cms.InputTag("hltGtDigis::HLT"),
    l1etsums  = cms.InputTag("hltGtDigis::HLT"),

    hltresults            = cms.InputTag("TriggerResults::HLT"),
    HLTProcessName        = cms.string("HLT"),

    ### GEN objects
    mctruth               = cms.InputTag("genParticles::HLT"),
    genEventInfo          = cms.InputTag("generator::SIM"),

    ### SIM objects
    simhits               = cms.InputTag("g4SimHits"),
                                
    ### Run parameters
    RunParameters = cms.PSet(
        HistogramFile = cms.untracked.string('hltbitanalysis.root'),
        isData        = cms.bool(False),
        Debug         = cms.bool(False),
        GenTracks     = cms.bool(True)
    )

)
