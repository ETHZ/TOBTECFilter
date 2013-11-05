import FWCore.ParameterSet.Config as cms


process = cms.Process("TobTecTest")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/shome/mangano/filterTOBTEC/CMSSW_5_3_11/src/MySub/TaggerProducers/allGoodEvents.root'
#        'file:/shome/mangano/filterTOBTEC/CMSSW_5_3_11/src/MySub/TaggerProducers/allBadEvents.root'
    )
)

import MySub.TaggerProducers.taggerproducers_cfi
process.BorisTobTecTaggerRECO = MySub.TaggerProducers.taggerproducers_cfi.tobtectagger.clone()

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('test.root'),
    outputCommands = cms.untracked.vstring(
        'keep *_*_*_TobTecTest'
        ),
)

process.p = cms.Path(
                     process.BorisTobTecTaggerRECO
)
process.e = cms.EndPath(process.out)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.schedule = cms.Schedule(process.p,process.e)
 
