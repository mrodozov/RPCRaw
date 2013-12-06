#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "Offline.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TF1.h"

#define GOODTDCLOW1 780
#define GOODTDCHIGH1 840
#define GOODTDCLOW2 760
#define GOODTDCHIGH2 820
#define TDCRANGE 1800
#define TDCOFFSET -1000
#define NOISETDCLOW 0
#define NOISETDCHIGH 650
//#define GOODTDCLOW 795
//#define GOODTDCHIGH 835
//#define TDCRANGE 49975
//#define TDCOFFSET -50000

// RE4-3 Strip Surfaces in cm2
#define AreaStrip_RE43_A 189.469
#define AreaStrip_RE43_B 250.152
#define AreaStrip_RE43_C 176.228

using namespace std;

Int_t getPartition( Int_t );

void Offline_final( string name )
{

  // strip off .root from filename
  if ( name.substr(name.find_last_of(".")) == ".root" )
    name = name.erase(name.find_last_of("."));

  // get the voltage 



  TH1F* scintcounts = new TH1F( "scintcounts", "Scintilator counts", 
                                32, 0., 32.);
  scintcounts->SetMinimum(0);
  TH2F* scintTDC = new TH2F( "scintTDC", "Scintilator TDC values", 
			     32, 0, 32, TDCRANGE+100, 0, TDCRANGE+100 ); 

  TH1F* cleanscintcounts = new TH1F( "cleanscintcounts", 
				     "Scintilator counts for single hits", 
				     32, 0., 32.);
  cleanscintcounts->SetMinimum(0);
  TH1F* dirtyscintcounts = new TH1F( "dirtyscintcounts", 
				     "Scintilator counts for multiple hits", 
				     32, 0., 32.);
  dirtyscintcounts->SetMinimum(0);

  //  TH2F* dirtyTDC = new TH2F( "dirtyTDC", 
  //			     "Scintilator TDC values for multiple hits", 
  //			     32, 0, 32, 2000, 0, 2000 );

  TH2F *scinthits = new TH2F( "scinthits", "Scint. hit multi.", 
			      20, 0, 20, 20, 0, 20);
  scinthits->SetXTitle("Number of Top Plane Hits");
  scinthits->SetYTitle("Number of Bottom Plane Hits");

  TH1F* diffscint = new TH1F( "diffscint", 
			      "Difference between top and bottom scint number",
			      32, -16, 16 );

  // Read TDC mapping file and some more init stuff
  char hisid[50], hisname[50];
  TH1F *RPCcounts[5], *RPCHitMult[5], 
    *RPCClust[5], *RPCNClust[5], *RPCClustSize[5],
    *RPCNoiseCounts[5], *RPCNoiseRates[5];
  TH1F *RPCClusterSize[5][3], *RPCNoiseProfile[5][3], *RPCHitProfile[5][3];
  TH2F *RPCTDC[5], *RPCClustSizePos[5];
  map<Int_t,Int_t> RPCChMap;
  Int_t RPCCh, TDCCh;
  ifstream mappingfile("TDCChMapping.txt");

  Int_t GOODTDCTIME[5][6];

  for ( Int_t q = 0; q < 5; q++ ) {

    sprintf(hisid, "RPC_%i", q+1 );
    sprintf(hisname, "Hit Distribution RPC %i", q+1 );
    RPCcounts[q] = new TH1F( hisid, hisname, 96, 0., 96.);

    sprintf(hisid, "RPCNoise_%i", q+1 );
    sprintf(hisname, "Noise Hit Distribution RPC %i", q+1 );
    RPCNoiseCounts[q] = new TH1F( hisid, hisname, 96, 0., 96.);

    sprintf(hisid, "RPCNoiseRates_%i", q+1 );
    sprintf(hisname, "Noise Rates RPC %i", q+1 );
    RPCNoiseRates[q] = new TH1F( hisid, hisname, 96, 0, 96 );
    RPCNoiseRates[q]->SetXTitle("Strip Number");
    RPCNoiseRates[q]->SetYTitle("Noise Rate (Hz/cm^{2})");

    sprintf(hisid, "RPCHitMult_%i", q+1 );
    sprintf(hisname, "Hit Multiplicity RPC %i", q+1 );
    RPCHitMult[q] = new TH1F( hisid, hisname, 96, 0, 96 );

    sprintf(hisid, "RPCTDC_%i", q+1 );
    sprintf(hisname, "TDC Response RPC %i", q+1 );
    RPCTDC[q] = new TH2F( hisid, hisname, 96, 0, 96, TDCRANGE+100, 
			  0, TDCRANGE+100 );
    RPCTDC[q]->SetXTitle("Strip Number");
    RPCTDC[q]->SetYTitle("TDC Time (ns)");

    sprintf(hisid, "RPCClust_%i", q+1 );
    sprintf(hisname, "Cluster Distribution RPC %i", q+1 );
    RPCClust[q] = new TH1F( hisid, hisname, 96, 0., 96.);

    sprintf(hisid, "RPCNClust_%i", q+1 );
    sprintf(hisname, "Number of Clusters RPC %i", q+1 );
    RPCNClust[q] = new TH1F( hisid, hisname, 15, 0, 15 );

    sprintf(hisid, "RPCClustSize_%i", q+1 );
    sprintf(hisname, "Cluster Size RPC %i", q+1 );
    RPCClustSize[q] = new TH1F( hisid, hisname, 35, 0, 35 );

    sprintf(hisid, "RPCClustSizePos_%i", q+1 );
    sprintf(hisname, "Cluster Size vs. Position RPC %i", q+1 );
    RPCClustSizePos[q] = new TH2F( hisid, hisname, 192, 0, 96, 35, 0, 35 );

    // Plots for each partition
    for ( Int_t p = 0; p < 3; p++ ) {

      sprintf(hisid, "RPC_%i_%i", q+1, p+1 );
      if ( p == 0 ) 
	sprintf(hisname, "RPC Hit Profile %i, Partition A", q+1 );
      else if ( p == 1 )
	sprintf(hisname, "RPC Hit Profile %i, Partition B", q+1 );
      else if ( p == 2 )
	sprintf(hisname, "RPC Hit Profile%i, Partition C", q+1 );
      RPCHitProfile[q][p] = new TH1F( hisid, hisname, 96, 0., 96.);

      sprintf(hisid, "RPCClustSize_%i_%i", q+1, p+1 );
      if ( p == 0 ) 
	sprintf(hisname, "RPC Cluster Size %i, Partition A", q+1 );
      else if ( p == 1 )
	sprintf(hisname, "RPC Cluster Size %i, Partition B", q+1 );
      else if ( p == 2 )
	sprintf(hisname, "RPC Cluster Size %i, Partition C", q+1 );
      RPCClusterSize[q][p] = new TH1F( hisid, hisname, 10, 1, 11 );     

      sprintf(hisid, "RPCNoise_%i_%i", q+1, p+1 );
      if ( p == 0 ) 
	sprintf(hisname, "RPC Noise Profile %i, Partition A", q+1 );
      else if ( p == 1 )
	sprintf(hisname, "RPC Noise Profile %i, Partition B", q+1 );
      else if ( p == 2 )
	sprintf(hisname, "RPC Noise Profile %i, Partition C", q+1 );
      RPCNoiseProfile[q][p] = new TH1F( hisid, hisname, 96, 0, 96 ); 

    }

    for ( Int_t j = 0; j < 96; j++ ) {
      mappingfile >> RPCCh >> TDCCh;
      if ( TDCCh != -1 )
	RPCChMap[TDCCh] = RPCCh;
    }
  }
  mappingfile.close();

  TFile f((name+".root").c_str());
  TTree* mytree = (TTree*)f.Get("RAWData");

  RAWData mydata;

  mytree->SetBranchAddress("iEvent", &mydata.iEvent);
  mytree->SetBranchAddress("ScintNTop", &mydata.ScintNTop);
  mytree->SetBranchAddress("ScintTopHits", mydata.ScintTopHits);
  mytree->SetBranchAddress("ScintNBot", &mydata.ScintNBot);
  mytree->SetBranchAddress("ScintBotHits", mydata.ScintBotHits);
  mytree->SetBranchAddress("ScintTopQDC", mydata.ScintTopQDC);
  mytree->SetBranchAddress("ScintBotQDC", mydata.ScintBotQDC);
  mytree->SetBranchAddress("TDCNHits", &mydata.TDCNHits);
  mytree->SetBranchAddress("TDCCh", mydata.TDCCh);
  mytree->SetBranchAddress("TDCTime", mydata.TDCTime);

  bool TopScintHit, BotScintHit;

  int NGoodScintTriggers = 0;
  int NGoodRPCTriggers[3] = { 0 };
  int NGoodRPCEvents[5][3] = { {0} };

  vector<Int_t>BotScintHits, TopScintHits, RPCHits[5], RPCNoiseHits[5];
  vector<Int_t>RPCPartitions[5];
  vector<RPCCluster> RPCClusters[5];

  float noiserates[5][96] = { {0.} };
  float noiseratespart[5][3] = { {0.} };

  // first determine the good RPC time windows for each connector
  for ( Int_t i = 0; i < mytree->GetEntries(); i++ ) {
    cout << " i = " << i << endl;

    //    if ( i >= 796 ) continue;

    mytree->GetEntry(i);

    for ( Int_t j = 0; j < mydata.TDCNHits; j++ ) {
      if ( mydata.TDCCh[j] >= 32 ) { // RPC hit
	int RPCChannel = RPCChMap[mydata.TDCCh[j]];
	int RPCNumber = (int)(RPCChannel/100);
	int RPCStrip = (int)(RPCChannel%100);

	RPCTDC[RPCNumber]->Fill( RPCStrip, mydata.TDCTime[j] );

      }
    }
  }

  TF1 *slicefit = new TF1("slicefit","gaus(0)",740,840);
  slicefit->SetParameter(0,50);
  slicefit->SetParLimits(0,1,100000);
  slicefit->SetParameter(1,800);
  slicefit->SetParLimits(1,750,830);
  slicefit->SetParameter(2,10);
  slicefit->SetParLimits(2,5,20);

  // run over RPCs
  for ( Int_t q = 0; q < 5; q++ ) {

    // run over connectors
    for ( Int_t c = 0; c < 6; c++ ) {
      TH1D* proj = (RPCTDC[q])->ProjectionY("proj",c*16+1,16+c*16);
      proj->Fit(slicefit,"QRL");
      GOODTDCTIME[q][c] = (Int_t)slicefit->GetParameter(1);

      cout << "RPC " << q << " connt = " << c 

	   << " " << GOODTDCTIME[q][c] 
	   << " " << slicefit->GetParameter(0)
	   << " " << slicefit->GetParameter(1)
	   << " " << slicefit->GetParameter(2) << endl;
    }

    RPCTDC[q]->Reset();
  }

  for ( Int_t i = 0; i < mytree->GetEntries(); i++ ) {

    //if ( i >= 796 ) continue;

    mytree->GetEntry( i );

    TopScintHit = false;
    BotScintHit = false;

    BotScintHits.clear();
    TopScintHits.clear();

    //    cout << mydata.TDCNHits << endl;
    // first check scintilator hits
    for ( Int_t j = 0; j < mydata.TDCNHits; j++ ) {

      // get scintilator hits
      if ( mydata.TDCCh[j] < 16 ) { // bottom scintilator
	BotScintHit = true;
	scintcounts->Fill( mydata.TDCCh[j] );
	scintTDC->Fill( mydata.TDCCh[j], mydata.TDCTime[j] );
	BotScintHits.push_back( mydata.TDCCh[j] );
      }
      else if ( mydata.TDCCh[j] < 32 ) { // top scintilator
	TopScintHit = true;
	scintcounts->Fill( mydata.TDCCh[j] );
	scintTDC->Fill( mydata.TDCCh[j], mydata.TDCTime[j] );
	TopScintHits.push_back( mydata.TDCCh[j] - 16 );
      }
    }

    scinthits->Fill( TopScintHits.size(), BotScintHits.size() );

    if ( (TopScintHits.size() !=1) || (BotScintHits.size() !=1) ) {
      for ( unsigned int j = 0; j < BotScintHits.size(); j++ ) 
	dirtyscintcounts->Fill( BotScintHits[j] );
      for ( unsigned int j = 0; j < TopScintHits.size(); j++ )
	dirtyscintcounts->Fill( TopScintHits[j]+16 );
      //cout << "hits top " << TopScintHits.size() 
      //   << " bot " << BotScintHits.size() << endl;
      continue;
    }

    cleanscintcounts->Fill( BotScintHits[0] );
    cleanscintcounts->Fill( TopScintHits[0]+16 );

    diffscint->Fill( TopScintHits[0]-BotScintHits[0] );
    //cout << "boe : " << TopScintHits[0] << " " << BotScintHits[0] << endl;

    //if ( TopScintHits[0] != BotScintHits[0] ) continue;

    //if ( (TopScintHits[0] < 7) || (TopScintHits[0] > 12) ) continue;
    //if ( (BotScintHits[0] < 7) || (BotScintHits[0] > 12) ) continue;

    //if ( abs(TopScintHits[0]-BotScintHits[0])>1 ) continue; 

    NGoodScintTriggers++;

    for ( Int_t q = 0; q < 5; q++ ) {
      RPCHits[q].clear();
      RPCNoiseHits[q].clear();
      RPCClusters[q].clear();
      RPCPartitions[q].clear();
    }
    
    // RPC hits
    for ( Int_t j = 0; j < mydata.TDCNHits; j++ ) {
      if ( mydata.TDCCh[j] >= 32 ) { // RPC hit
	int RPCChannel = RPCChMap[mydata.TDCCh[j]];
	int RPCNumber = (int)(RPCChannel/100);
	int RPCStrip = (int)(RPCChannel%100);
	int RPCConnector = (int)(RPCStrip/16);
	RPCTDC[RPCNumber]->Fill( RPCStrip, mydata.TDCTime[j] );

	// extract good hits, in time window
	/*
	if ( (((RPCNumber < 3) || ((RPCNumber == 3) && (RPCStrip < 64)))
	      && (mydata.TDCTime[j] > GOODTDCLOW1) 
	      && (mydata.TDCTime[j] < GOODTDCHIGH1)) 
	     ||
	     (((RPCNumber == 4) || ((RPCNumber == 3) && (RPCStrip >= 64)))
	      && (mydata.TDCTime[j] > GOODTDCLOW2) 
	      && (mydata.TDCTime[j] < GOODTDCHIGH2)) ) {
	*/
	if ( (mydata.TDCTime[j] > 
	      ((GOODTDCTIME[RPCNumber][RPCConnector])-30))
	     && (mydata.TDCTime[j] < 
		 ((GOODTDCTIME[RPCNumber][RPCConnector])+30)) ) {
	  // suppress some hot channels
	  /*
	  if ( RPCNumber == 0 ) {
	    if ( RPCStrip == 0 ) 
	      //|| RPCStrip == 7 || RPCStrip == 10 || 
	      // RPCStrip == 11 ||
	      // RPCStrip == 12 || RPCStrip == 13 || RPCStrip == 32 ||
	      // RPCStrip == 50 || RPCStrip == 76 || RPCStrip == 78 )
	      continue;
	      }*/

	  RPCcounts[RPCNumber]->Fill( RPCStrip );
	  RPCHits[RPCNumber].push_back( RPCStrip );

	} else if ( (mydata.TDCTime[j] > NOISETDCLOW)
	       && (mydata.TDCTime[j] < NOISETDCHIGH) ) { 
	  // noise hits
	  RPCNoiseCounts[RPCNumber]->Fill( RPCStrip );
	  RPCNoiseHits[RPCNumber].push_back( RPCStrip );

	}
      } 
    }

    for ( Int_t q = 0; q < 5; q++ ) {

      // RPC cluster size and multiplicity

      RPCHitMult[q]->Fill( RPCHits[q].size() );

      if ( RPCHits[q].size() > 0 ) {

	//	NGoodRPCEvents[q]++;

	sort( RPCHits[q].begin(), RPCHits[q].end() );
	RPCCluster tempcluster;
	Int_t previousstrip = -1000;
	for ( vector<Int_t>::iterator it = RPCHits[q].begin(); 
	      it != RPCHits[q].end(); it++ ) {
	  //cout << "q = " << q << " hits = " << *it << endl;
	  
	  if ( (*it == (previousstrip+1))
	       && (previousstrip != 31)
	       && (previousstrip != 63) ) {
	    tempcluster.Size++;
	    tempcluster.LastStrip = *it;
	    tempcluster.AverageStrip = .5*(tempcluster.LastStrip
					   +tempcluster.FirstStrip);
	    tempcluster.Partition = getPartition( tempcluster.AverageStrip );
	  } else {
	    if ( previousstrip != -1000 )
	      RPCClusters[q].push_back(tempcluster);
	    tempcluster.Size = 1;
	    tempcluster.FirstStrip = *it;
	    tempcluster.LastStrip = *it;
	    tempcluster.AverageStrip = .5*(tempcluster.LastStrip
					   +tempcluster.FirstStrip);
	    tempcluster.Partition = getPartition( tempcluster.AverageStrip );
	  }
	  previousstrip = tempcluster.LastStrip;
	}
	RPCClusters[q].push_back(tempcluster);
      }
      
      RPCNClust[q]->Fill( RPCClusters[q].size() );

      //      if ( RPCNClust[0] != 1 || RPCNClust[2] != 1 ) continue;

      for ( vector<RPCCluster>::iterator su = RPCClusters[q].begin();
	    su != RPCClusters[q].end(); su++ ) {
	//cout << "q = " << q << " cluster size = " << (*su).Size << endl;
	RPCClustSize[q]->Fill( (*su).Size );
	RPCClust[q]->Fill( (*su).AverageStrip );
	RPCClustSizePos[q]->Fill( (*su).AverageStrip, (*su).Size );
	// check the partition of the cluster
	
	if ( (*su).AverageStrip <= 31 ) 
	  RPCPartitions[q].push_back(0);
	else if ( (*su).AverageStrip <= 63 ) 
	  RPCPartitions[q].push_back(1);
	else 
	  RPCPartitions[q].push_back(2);
	/*
	if ( ((*su).AverageStrip >= 8) && ((*su).AverageStrip <= 16) )
	  RPCPartitions[q].push_back(0);
	else if ( ((*su).AverageStrip >= 40) && ((*su).AverageStrip <= 48) )
	  RPCPartitions[q].push_back(1);
	else if ( ((*su).AverageStrip >= 72) && ((*su).AverageStrip <= 80) )
	  RPCPartitions[q].push_back(2);
	*/	
      }
     
      //cout << "---------" << endl;

      // Noise rates
      for ( vector<Int_t>::iterator it = RPCNoiseHits[q].begin(); 
	    it != RPCNoiseHits[q].end(); it++ ) 
	noiserates[q][*it]++;

    }

    // check if this was a good event, ie. 1 hit in trigger RPCs, in 
    // same partition
    //    if ( (RPCPartitions[0].size() == 1) && (RPCPartitions[2].size() == 1)
    //	 && (RPCPartitions[0][0] == RPCPartitions[2][0]) ) {
    if ( RPCClusters[0].size() == 1 && RPCClusters[4].size() == 1
	 && RPCClusters[0][0].Partition == RPCClusters[4][0].Partition ) {

      //      NGoodTriggers[RPCPartitions[0][0]]++;
      NGoodRPCTriggers[RPCClusters[0][0].Partition]++;
      
      /*    cout << "Good Trigger : " << RPCPartitions[0][0] << " "
	   << "strips = " << RPCClusters[0][0].AverageStrip << " " 
	   << RPCClusters[1][0].AverageStrip 
	   << RPCClusters[2][0].AverageStrip << " " 
	   << endl;
      */
      for ( Int_t q = 1; q < 4; q++ ) {

	// fill the distributions per partition
	for ( vector<Int_t>::iterator it = RPCHits[q].begin(); 
	      it != RPCHits[q].end(); it++ )
	  RPCHitProfile[q][RPCClusters[0][0].Partition]->Fill(*it);
	
	for ( vector<Int_t>::iterator it = RPCNoiseHits[q].begin(); 
	      it != RPCNoiseHits[q].end(); it++ )
	  RPCNoiseProfile[q][RPCClusters[0][0].Partition]->Fill(*it);

	for ( vector<RPCCluster>::iterator su = RPCClusters[q].begin();
	      su != RPCClusters[q].end(); su++ ) 
	  RPCClusterSize[q][RPCClusters[0][0].Partition]->Fill( (*su).Size );

	// efficiency
	bool goodrpcevent = false;
	//	for ( vector<Int_t>::iterator it = RPCPartitions[q].begin();
	//    it != RPCPartitions[q].end(); it++ )
	//  if ( *it == RPCPartitions[0][0] ) {

	for ( vector<RPCCluster>::iterator su = RPCClusters[q].begin();
	      su != RPCClusters[q].end(); su++ ) {	  
	  if ( ((*su).Partition == RPCClusters[0][0].Partition) 
	       && ( ((RPCClusters[0][0].AverageStrip
		      +q*(RPCClusters[4][0].AverageStrip
			  -RPCClusters[0][0].AverageStrip)/4.)
		     -(*su).AverageStrip) <= 3) 
	       ) {
	    goodrpcevent = true;
	    /*
	    cout << "shit Event RPC " << q+1  
		 << " Partition " << RPCPartitions[0][0] << " "
		 << "strips = " << RPCClusters[0][0].AverageStrip << " " 
		 << (*su).AverageStrip << " "
		 << RPCClusters[4][0].AverageStrip << " " 
		 << RPCClusters[0][0].AverageStrip
	      +(RPCClusters[4][0].AverageStrip
			 -RPCClusters[0][0].AverageStrip)/4.*q
		 << " number of clusters : " << RPCClusters[q].size()
		 << endl;	    
	    */
	  }
	}
	if ( goodrpcevent ) 
	  //NGoodRPCEvents[q][RPCPartitions[0][0]]++;
	  NGoodRPCEvents[q][RPCClusters[0][0].Partition]++;
	/*	else {
	for ( vector<RPCCluster>::iterator su = RPCClusters[q].begin();
	      su != RPCClusters[q].end(); su++ ) 	
	  cout << "shit Event : " << RPCPartitions[0][0] << " "
	       << "strips = " << RPCClusters[0][0].AverageStrip << " " 
	       << (*su).AverageStrip <<" "
	       << RPCClusters[2][0].AverageStrip << " " 
	       << 0.5*(RPCClusters[0][0].AverageStrip
		       +RPCClusters[2][0].AverageStrip)
	       << " number of clusters : " << RPCClusters[q].size()
	       << endl;	    
	       }*/
	  
      }

    }
  }

  f.Close();
  
  //  cout << "NGoodTriggers = " << NGoodTriggers << endl;

  // RPC global noise rates
 
  for ( Int_t q = 0; q < 5; q++ ) {
    for ( Int_t p = 0; p < 96; p++ ) {
      //noiserates[q][p] /= (TDCRANGE-GOODTDCHIGH+GOODTDCLOW)*1.e-9*NGoodTriggers;
      noiserates[q][p] /= (NOISETDCHIGH-NOISETDCLOW)*1.e-9*NGoodScintTriggers;
      //cout << "noise p = " << noiserates[q][p] << endl;

      // normalize to strip surface
      if ( p < 32 ) 
	noiserates[q][p] /= AreaStrip_RE43_A;
      else if ( p < 64 )
	noiserates[q][p] /= AreaStrip_RE43_B;
      else 
	noiserates[q][p] /= AreaStrip_RE43_C;

      RPCNoiseRates[q]->Fill( p, noiserates[q][p] );
    }
  }    
 
  // RPC total noise rates per partition
  Float_t TotalRPCArea = 
    32.*(AreaStrip_RE43_A + AreaStrip_RE43_B + AreaStrip_RE43_C);
  for ( Int_t q = 1; q < 4; q++ ) {
    for ( Int_t p = 0; p < 3; p++ ) {
      for ( Int_t s = 1; s <= 96; s++ ) 
	noiseratespart[q][p] += RPCNoiseProfile[q][p]->GetBinContent(s);

      //      cout << "shit q, p " << q << " " << p << " " 
      //   << noiseratespart[q][p] 
      //   << " good " 
      //   << NGoodRPCTriggers[p] <<endl;
      
      noiseratespart[q][p] /= (NOISETDCHIGH-NOISETDCLOW)*1.e-9
	*NGoodRPCTriggers[p]*TotalRPCArea;
    }
  }

  string outputlogfilename(name);
  // RPC efficiencies to log file
  ofstream outputlogfile_eff((outputlogfilename
			      +"_Offline_final_eff.log").c_str());

  //  outputlogfile_eff << HV << " ";
  float RPCEff[5][3], RPCEffError[5][3];
  for ( Int_t q = 1; q < 4; q++ ) {   // loop over chambers
    cout << "Efficiency RPC " << q << " : " << endl;
    for ( Int_t g = 0; g < 3; g++ ) {  // loop over partitions
      cout << "Good RPC Partition " << g << ": " 
	   << NGoodRPCEvents[q][g] << endl;
      RPCEff[q][g] = (float)(NGoodRPCEvents[q][g])/NGoodRPCTriggers[g];
      RPCEffError[q][g] = sqrt( RPCEff[q][g]*(1.-RPCEff[q][g])/NGoodRPCTriggers[g]);
      cout << RPCEff[q][g]*100. << " +- " << RPCEffError[q][g]*100. << " %"
	   << endl;
      outputlogfile_eff << RPCEff[q][g] << " " << RPCEffError[q][g] << " "; 
    }
  }
  outputlogfile_eff << endl;
  outputlogfile_eff.close();

  // Cluster sizes to log file
  ofstream outputlogfile_cls((outputlogfilename
			      +"_Offline_final_cls.log").c_str());
  for ( Int_t q = 0; q < 5; q++ ) {

    outputlogfile_cls << RPCClustSize[q]->GetMean() << " "
		      << RPCClustSize[q]->GetRMS() << " ";

  }
  outputlogfile_cls << endl;
  outputlogfile_cls.close();

  // Cluster sizes per partition to log file
  ofstream outputlogfile_cls2((outputlogfilename
			      +"_Offline_final_cls_part.log").c_str());
  for ( Int_t q = 0; q < 5; q++ ) {
    for ( Int_t p = 0; p < 3; p++ ) {

    outputlogfile_cls2 << RPCClusterSize[q][p]->GetMean() << " "
		       << RPCClusterSize[q][p]->GetRMS() << " ";
    }

  }
  outputlogfile_cls2 << endl;
  outputlogfile_cls2.close();

  // output files for database
  char csvfilename[100];
  for ( Int_t q = 1; q < 4; q++ ) {   // loop over chamber
    for ( Int_t p = 0; p < 3; p++ ) { // loop over partition
      if ( p == 0 )
	sprintf(csvfilename,"Station_%i_Partition_A.csv", q+1);
      else if ( p == 1 )
	sprintf(csvfilename,"Station_%i_Partition_B.csv", q+1);
      else if ( p == 2 )
	sprintf(csvfilename,"Station_%i_Partition_C.csv", q+1);
      ofstream outputcsvfile((outputlogfilename
			      +"_Offline_final_"
			      +csvfilename).c_str());
      outputcsvfile << NGoodRPCTriggers[p] << ","
		    << RPCEff[q][p]*100. << ","
		    << RPCEffError[q][p]*100. << ","
		    << RPCClusterSize[q][p]->GetMean() << ","
		    << noiseratespart[q][p] << ",";

      for ( Int_t s = 1; s <= 11; s++ ) 
	outputcsvfile 
	  << (RPCClusterSize[q][p]->GetBinContent(s))/NGoodRPCTriggers[p] 
	  << ",";

      outputcsvfile << "215,215,215,215,215,215,215,215,215,215,215,215,";

      for ( Int_t s = 1; s <= 96; s++ )
	outputcsvfile 
	  << RPCHitProfile[q][p]->GetBinContent(s) << ",";
      
      for ( Int_t s = 1; s <= 96; s++ )
	outputcsvfile 
	  << RPCNoiseProfile[q][p]->GetBinContent(s) << ",";

      outputcsvfile << ",," << endl;

      outputcsvfile.close();
    }
  }
	
  string outputfilename(name);
  outputfilename += "_Offline.root";
  TFile outputfile(outputfilename.c_str(), "recreate");

  TCanvas *c1[5];

  for ( Int_t q = 0; q < 5; q++ ) {
    c1[q] = new TCanvas();
    c1[q]->Draw();
    c1[q]->SetLogy();
    RPCcounts[q]->Draw();

    RPCcounts[q]->Write();
    RPCNoiseCounts[q]->Write();
    RPCNoiseRates[q]->Write();
    RPCHitMult[q]->Write();
    RPCTDC[q]->Write();
    RPCClust[q]->Write();
    RPCNClust[q]->Write();
    RPCClustSize[q]->Write();
    RPCClustSizePos[q]->Write();

    for ( Int_t p = 0; p < 3; p++ ) {
      RPCHitProfile[q][p]->Write();
      RPCNoiseProfile[q][p]->Write();
      RPCClusterSize[q][p]->Write();
    }

  }
  scintcounts->Write();
  scintTDC->Write();
  //  dirtyTDC->Write();
  cleanscintcounts->Write();
  dirtyscintcounts->Write();
  scinthits->Write();
  diffscint->Write();

  outputfile.Close();

}

Int_t getPartition( Int_t strip ) 
{
  if ( strip <= 31 ) 
    return 0;
  else if ( strip <= 63 )
    return 1;
  else
    return 2;  
}

