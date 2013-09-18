/** 
 * Author :
 * Added to the package by Mircho Rodozov, mrodozov@cern.ch
 * Added 25.08.13
 */ 

#ifndef Offline_h
#define Offline_h

namespace RPCGhent {

#define MAXTDCHITS 500

struct RAWData {
  Int_t           iEvent;
  Int_t           ScintNTop;
  Int_t           ScintTopHits[50];   //[ScintNTop]
  Int_t           ScintNBot;
  Int_t           ScintBotHits[50];   //[ScintNBot]
  Int_t           ScintTopQDC[20];
  Int_t           ScintBotQDC[20];
  Int_t           TDCNHits;
  Int_t           TDCCh[MAXTDCHITS];   //[TDCNHits]
  Int_t           TDCTime[MAXTDCHITS];   //[TDCNHits]
};

struct RPCCluster {
  Int_t Size;
  Int_t FirstStrip;
  Int_t LastStrip;
  Float_t AverageStrip;
  Int_t Partition;   // A=0, B=1, C=2
};

}

#endif

