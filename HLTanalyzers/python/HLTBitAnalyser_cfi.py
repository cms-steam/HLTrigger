import FWCore.ParameterSet.Config as cms

hltbitanalysis = cms.EDAnalyzer("HLTBitAnalyzer",
    ### Trigger objects
    l1results = cms.InputTag("hltGtStage2Digis", ""      , "HLT"),                            
    l1muons   = cms.InputTag("hltGtStage2Digis", "Muon"  , "HLT"),
    l1egamma  = cms.InputTag("hltGtStage2Digis", "EGamma", "HLT"),
    l1jets    = cms.InputTag("hltGtStage2Digis", "Jet"   , "HLT"),
    l1taus    = cms.InputTag("hltGtStage2Digis", "Tau"   , "HLT"),
    l1etsums  = cms.InputTag("hltGtStage2Digis", "EtSum" , "HLT"),

    isL1MuHighQual = cms.bool(True),

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
