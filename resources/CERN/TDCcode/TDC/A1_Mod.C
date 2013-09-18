#define A1_Mod_cxx
#include "A1_Mod.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void A1_Mod::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L A1_Mod.C
//      Root > A1_Mod t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
  c1 = new TCanvas( 3 );
  c1->Divide( 2, 2 );
  hTOF1 = new TH1F( "hTOF1", "First ToF;Time of Flight [ns];Counts", 100, 0, 100 );
  hTOF2 = new TH1F( "hTOF2", "Second ToF;Time of Flight [ns];Counts", 100, 0, 100 );
  hTOFDiff = new TH1F( "hTOFDiff", "ToF2 - ToF1;Time [ns];Counts", 100, 0, 100 );
  hTrigger = new TH1F( "hTrigger", "Trigger;Time 0.1 ns;Counts;", 100000, 10000, 100000 );
  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   
      nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      //Module2_NUMBER_OF_CHANNELS;
      vector<unsigned int> * HITS_PER_CHANNEL = HitsPerChannel[ 1 ];
      vector<unsigned int> * CH32 = _Data[ 1 ][ 32 ];//Module2_LE_CH32;
      vector<unsigned int> * CH96 = _Data[ 1 ][ 96 ];//Module2_LE_CH96;
      vector<unsigned int> * CH64 = _Data[ 1 ][ 64 ];//Module2_LE_CH64;
      //std::cout << "HITS_PER_CHANNEL 32: " << HITS_PER_CHANNEL->at( 32 ) << std::endl;
      if( HITS_PER_CHANNEL->at( 32 ) > 0 ){
        long trig = CH32->at( 0 );
        hTrigger->Fill( trig );
        long tof1( 0 ), tof2( 0 );
        //std::cout << "trigger : " << trig << std::endl;
        if( HITS_PER_CHANNEL->at( 96 ) > 0 ){
          tof1 = CH96->at( 0 ) - trig;
          hTOF1->Fill( tof1/10 );
//          std::cout << "TOF1 = " << tof1 / 10<< "ns " << std::endl;
        }
        
        if( HITS_PER_CHANNEL->at( 64 ) > 0 ){
          tof2 = CH64->at( 0 ) - trig;
          hTOF2->Fill( tof2/10 );
//          std::cout << "TOF2 = " << tof2 / 10 << " ns "<< std::endl;
        }
        if( (tof1 != 0 ) && (tof2 != 0 ) ){
          //std::cout << "TOF2 -  TOF1 = " << tof2 - tof1 << std::endl;
          hTOFDiff->Fill( ( tof2 - tof1 )/10 );
        }
      } 
      
   }
   c1->cd( 1 );
   hTOF1->Draw();
   c1->cd( 2 );
   hTOF2->Draw();
   c1->cd( 3 );
   hTOFDiff->Draw();
   c1->cd( 4 );
   hTrigger->Draw();

}

void A1_Mod::PlotCoinc( unsigned aChannelFirst, unsigned aChannelSecond ){

  if (fChain == 0) return;
  if( c1 ) delete c1;
  c1 = new TCanvas( 3 );
  stringstream stmp;
  unsigned aModule = 0;
  unsigned ChannelCoinc = 30;
  stmp << "Coincidence - Channels " << aChannelFirst << " : " << aChannelSecond << ";Time [0.1 ns]; Counts";

  
  TH1I * hChannel = new TH1I( "hChannel", stmp.str().c_str(), 10000, 0, 10000 );
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  
  for (Long64_t jentry = 0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   
    nbytes += nb;
      // if (Cut(ientry) < 0) continue;
    vector<unsigned int> * HITS_PER_CHANNEL = HitsPerChannel[ aModule ];
    vector<unsigned int> * COINC = _Data[ aModule ][ ChannelCoinc ];
    vector<unsigned int> * TRIG = _Data[ aModule ][ 31 ];   
   //std::cout << "HITS_PER_CHANNEL 32: " << HITS_PER_CHANNEL->at( 32 ) << std::endl;
    if( (HITS_PER_CHANNEL->at( aChannelFirst ) > 0 ) && (HITS_PER_CHANNEL->at( aChannelSecond ) > 0 ) && (HITS_PER_CHANNEL->at( ChannelCoinc ) > 0 ) ){
      long ch = COINC->at( 0 );
      long trig = TRIG->at( 0 );
      std::cout << "Trig -  = " << trig - ch << std::endl;
      hChannel->Fill( trig - ch );
    } 
      
  }

  hChannel->Draw();



}

void A1_Mod::PlotChannel( unsigned aModule, unsigned aChannel ){
  if (fChain == 0) return;
  if( c1 ) delete c1;
  c1 = new TCanvas( 3 );
  stringstream stmp;
  stmp << "Module " << aModule + 1 << ", Channel #" << aChannel << ";Time [0.1 ns]; Counts";


  TH1I * hChannel = new TH1I( "hChannel", stmp.str().c_str(), 10000, 0, 10000 );
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   
    nbytes += nb;
      // if (Cut(ientry) < 0) continue;
    vector<unsigned int> * HITS_PER_CHANNEL = HitsPerChannel[ aModule ];
    vector<unsigned int> * CH = _Data[ aModule ][ aChannel ];//Module2_LE_CH32;
    vector<unsigned int> * TRIG = _Data[ aModule ][ 31 ];
      //std::cout << "HITS_PER_CHANNEL 32: " << HITS_PER_CHANNEL->at( 32 ) << std::endl;
    if( HITS_PER_CHANNEL->at( aChannel ) > 0 ){
      long ch = CH->at( 0 );
      long trig = TRIG->at( 0 );
      std::cout << "Trig - Ch = " << trig - ch << std::endl;
      hChannel->Fill( trig - ch );
    } 
      
  }

  hChannel->Draw();
}

void A1_Mod::PlotAll( unsigned aModule){
  if (fChain == 0) return;
  if( c1 ) delete c1;
  c1 = new TCanvas(3);
  c1->Divide(4,3);
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;

  TH1I* hChannel[12];
  char name_h[256]; 
  for (int i=0;i<10;i++){
    if (i<5) sprintf(name_h,"TOP_%iA_ch%i",i+1,i+19);
    if (i>4) sprintf(name_h,"BOT_%iA_ch%i",i-4,i-2);
    hChannel[i] = new TH1I(name_h,name_h,30000,-30000,30000);
  }
  hChannel[10] = new TH1I("A1_ch30","A1_ch30",1600,-0.5,1599.5);
  hChannel[11] = new TH1I("Coincidence_ch31","Coincidence_ch31",2000,18999.5,20999.5);


  for (unsigned aChannel = 0;aChannel<12;aChannel++){
    cout<<aChannel<<endl;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   
      nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      unsigned real_ch; 
      if (aChannel < 5) real_ch = aChannel+19;
      if (aChannel > 4 && aChannel < 10) real_ch = aChannel-2;
      if (aChannel == 10) real_ch = 30;
      if (aChannel == 11) real_ch = 31;
      vector<unsigned int> * HITS_PER_CHANNEL = HitsPerChannel[ aModule ];
      vector<unsigned int> * CH = _Data[ aModule ][ real_ch ];//Module2_LE_CH32;
      vector<unsigned int> * TRIG = _Data[ aModule ][ 31 ];
      if( HITS_PER_CHANNEL->at( real_ch ) > 0 ){
	long ch = CH->at( 0 );
	long trig = TRIG->at( 0 );
	//	std::cout << "Trig - Ch = " << trig - ch << std::endl;
	if (real_ch != 31) hChannel[aChannel]->Fill( trig - ch );  
	if (real_ch == 31) hChannel[aChannel]->Fill( trig);
        hChannel[aChannel]->Fill( ch ); 
      }
    }
    if (aChannel < 10){
      c1->cd(aChannel+3);
      hChannel[aChannel]->Draw();  
    }
  }
  c1->cd(1);
  hChannel[10]->Draw();
  c1->cd(2);
  hChannel[11]->Draw();



}
