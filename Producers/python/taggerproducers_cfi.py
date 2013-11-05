import FWCore.ParameterSet.Config as cms


tobtectagger = cms.EDProducer('BorisTobTecTagger',
 trackCollection = cms.InputTag("generalTracks","","RECO"),  
 nPhiBins = cms.int32(6),                                               
 nEtaBins = cms.int32(48),                                               
 etaMin = cms.double(-2.4), 
 etaMax = cms.double(2.4), 
 etaOffset = cms.double(0.05), 
)
