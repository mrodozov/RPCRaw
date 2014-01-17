/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 06.08.13
*/

#include "UserApplications.h"
#include <occi.h>

using namespace std;
using namespace oracle::occi;

void currentTests(int _argc,char * arguments[]){
  
  string ghentfilename = arguments[1];
  TFile * ghentFile = new TFile(ghentfilename.c_str(),"READ","in");
  
  RPCRawConverter * converter = new RPCRawConverter();
  converter->setCurrentFileType(converter->getFileTypeForFile(ghentFile));
  converter->setCurrentFile(ghentFile);
  converter->initialyzeStructure();
  converter->setGhentTDCtoRPCmap(arguments[2]);
  
  RPCChamber * aBoard = new RPCChamber(kRPC_RE_4_2_chamber);
  RPCChamber * firstChamber, * secondChamber;
  firstChamber = new RPCChamber(kRPC_RE_4_2_chamber);
  secondChamber = new RPCChamber(kRPC_RE_4_2_chamber);
  firstChamber->allocAndInit();
  secondChamber->allocAndInit();
  
  aBoard->allocAndInit();
  aBoard->setStripsHitsDataFromSource(converter->getTriggerVector(1));
  
  firstChamber->setStripsHitsDataFromSource(converter->getChamberVector(1));
  secondChamber->setStripsHitsDataFromSource(converter->getChamberVector(2));
  
  converter->nextEvent();
  converter->nextEvent();
  
  for (int i = 0 ; i < 32 ; i++){
    if (aBoard->getStrip(i+1)->hasHit()){
      cout << i << " " << aBoard->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  for (int i = 0 ; i < 96 ; i++){
    
    if (firstChamber->getStrip(i+1)->hasHit()){
      cout << i << " first: " << firstChamber->getStrip(i+1)->getHits().at(0) << endl;
    }    
    if (secondChamber->getStrip(i+1)->hasHit()){
      cout << i << " second: " << secondChamber->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  
  converter->nextEvent();
  for (int i = 0 ; i < 32 ; i++){
    if (aBoard->getStrip(i+1)->hasHit()){
      cout << i << " " << aBoard->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  for (int i = 0 ; i < 96 ; i++){
    
    if (firstChamber->getStrip(i+1)->hasHit()){
      cout << i << " first: " << firstChamber->getStrip(i+1)->getHits().at(0) << endl;
    }    
    if (secondChamber->getStrip(i+1)->hasHit()){
      cout << i << " second: " << secondChamber->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  //while (converter->nextEvent()) { cout << converter->getEventNumber() << endl; }
  
}

void testDBconnection(int _argc, char* arguments[]){
  
  /** data base object tests */
  
  RPCDBReader * reader = new RPCDBReader;
  
  reader->openNewConnection();
  reader->closeCurrentConnection(); // just test does it work
  reader->openNewConnection();
  
  reader->getChambersDataForRunAndSite(2202,"CERN");
  
  reader->printResultWithNcolumns(8);
  
  reader->getChambersDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(8);
  
  reader->getChambersDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(8);
  
  reader->getEnvironmentDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(3);
  
  reader->getEnvironmentDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(3);
  
  reader->closeCurrentConnection();
  
    
    
}

void clusterObjectTester (int _argc,char * _argv[]){
  
  /* memory test
  
  for (int i = 0 ; i < 100000000 ; i++){
    RPCChamber * aBoard = new RPCChamber(kRPC_RE_4_3_chamber);
    aBoard->allocAndInit();
    cout << i << endl;
    delete aBoard;
    
  }
  // memory leak test, its ok not leaking. also checked with valgrind
  */
  
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster;
  cosmicTestChambersStack->createNewClusterOfChambersWithRE4type(5,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->deleteAllChambers();
  
  cosmicTestChambersStack->createNewClusterOfChambersWithRE4type(10,kRPC_RE_4_3_chamber);
  // now try to create before erase the previous
  cosmicTestChambersStack->createNewClusterOfChambersWithRE4type(3,kRPC_RE_4_3_chamber);
  // now print the ammount
  cout << cosmicTestChambersStack->getNumberOfChambers() << endl;
  // delete
  cosmicTestChambersStack->deleteAllObjects();
  cout << cosmicTestChambersStack->getNumberOfChambers() << endl;
  
}

void firstCompleteTestApplication(int arg_c,char * arg_v[]){
  
  string filename=arg_v[1],ghentMap = arg_v[2];
  int numberOfEventsToUse = atoi(arg_v[3]);
  TFile * rawdatafile = new TFile(filename.c_str());
  RPCRawConverter * converter = new RPCRawConverter(rawdatafile);
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChamberObjectsNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  
  /** */
  
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster(numberOfChamberObjectsNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->setDataSourceForNchambers(numberOfChamberObjectsNeeded,converter->getChambersData());
  cosmicTestChambersStack->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  
  //RPCChamber * singleLBforTest;
  //RPCLinkBoardChannel * singleChannel;
  //RPCChamber * triggerObject;
  //triggerObject = cosmicTestChambersStack->getTriggerObjectNumber(1);
  
  //TGraphErrors * trackGraph;
  //TGraph2DErrors * fitGraph;
  
  TFile * aFile = new TFile; // just dont record anything yet. The fit graphs could be recorded here
  TH1F * histPointer[14];
  histPointer[0] = new TH1F("scDiff","Difference between top and bottom hit time",100,0,100);
  histPointer[1] = new TH1F("coincMinusfirstScint","Difference between coincidence time and top scint hit",500,0,500);
  histPointer[2] = new TH1F("coincMinusSecondScint","Difference between coincidence time and bottom scint hit",500,0,500);
  histPointer[3] = new TH1F("toler","Time tolerance within one cluster",100,0,100);
  histPointer[4] = new TH1F("Clusters","Distribution of number of clusters in one chamber",10,0,10);
  histPointer[5] = new TH1F("diffTopMinusClusterAvg","Difference between the accepted top scint hit and each cluster average time",700,0,700);
  histPointer[6] = new TH1F("diffBottomMinusClusterAvg","Difference between the accepted bottom scint hit and each cluster average time",700,0,700);
  histPointer[7] = new TH1F("avgClsSize","Cluster size",20,0,20);
  histPointer[8] = new TH1F("clustersByPartitions","Distribution of number of partitions crossed by the track",7,0,7);
  histPointer[9] = new TH1F("SingleVsMultipleHits","Single vs Multi hits in every channel with hit",7,0,7);
  
  histPointer[10] = new TH1F();
  histPointer[11] = new TH1F();
  histPointer[12] = new TH1F();
  histPointer[13] = new TH1F();
  
  for (int i = 0 ; i < 14 ; i++){
    histPointer[i]->SetLineColor(kBlue);
    histPointer[i]->SetFillColor(kBlue);
  }
  
  for( int i = 0 ; i < numberOfEventsToUse ; i++){
  //while(converter->nextEvent()){
    converter->nextEvent();
    cout << " ----------------------------- " << endl;
    cout << " event: " << converter->getEventNumber() << ", chambers data: ";
    cosmicTestChambersStack->variousStudyExperimentalFunction(aFile,histPointer,converter->getEventNumber());
    
  }
  
  cosmicTestChambersStack->deleteAllObjects();
  
  histPointer[0]->SaveAs("ScintTimes1.root");
  histPointer[1]->SaveAs("CoincMinusTop.root");
  histPointer[2]->SaveAs("CoincMinusBottom.root");
  histPointer[3]->SaveAs("Tolerance.root");
  histPointer[4]->SaveAs("NumberOfClusters.root");
  histPointer[5]->SaveAs("TopMinusEachAverage.root");
  histPointer[6]->SaveAs("BottomMinusEachAverage.root");
  histPointer[7]->SaveAs("ClusterSize.root");
  histPointer[8]->SaveAs("PartitionTypes.root");
  histPointer[9]->SaveAs("SingleDoubleHits.root");
  
  rawdatafile->Close("R");
  rawdatafile->Delete();
  
  delete cosmicTestChambersStack;
  delete converter;
  
}

void timeEvolutionStudy(int _argc, char* _argv[]){
  
  string filename=_argv[6],ghentMap = _argv[2];
  int numberOfEventsToUse = atoi(_argv[3]);
  string histoToSave = _argv[4];
  unsigned chamberToTest = atoi(_argv[5]); // to be given from 1 not from 0
  TFile * rawdatafile = new TFile(filename.c_str());
  RPCRawConverter * converter = new RPCRawConverter(rawdatafile);
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChamberObjectsNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster(numberOfChamberObjectsNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->setDataSourceForNchambers(numberOfChamberObjectsNeeded,converter->getChambersData());
  cosmicTestChambersStack->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  
  // Setup done, now create histogram to fill. With other words, thats the analisys code
  
  TH1F * timeEvolutionHisto = new TH1F(histoToSave.c_str(),"Time evolution histogram",1000,0,500);
  timeEvolutionHisto->SetFillColor(kBlue);
  timeEvolutionHisto->SetLineColor(kBlue);
  timeEvolutionHisto->GetXaxis()->SetTitle("Time difference in nano seconds");
  
  RPCLinkBoardChannel * aChannel;
  RPCChamber * aChamber;
  vector<unsigned> vectorWithHits;
  int timeDifference = 0;
  unsigned startBin = 0 , stopBin = 0, numofbins = 0;
  ESiteFileType site = converter->getCurrentFileType();
  if (site == kIsCERNrawFile){
    startBin = 10000 ; stopBin = 25000; numofbins = 1500;
  }
  else if (site == kIsGENTrawFile) {
    startBin = 100 ; stopBin = 1000; numofbins = 1500;
  }
  else {
    // its barc file , specify it
  }
  
  
  TH2F * histoOfFirstChamber = new TH2F("","",numofbins,startBin,stopBin,98,0,98);
  TH2F * histoOfMultiHits = new TH2F("  ","",numofbins,startBin,stopBin,98,0,98);
  TH2F * histoOfFirstHits = new TH2F("   ","",numofbins,startBin,stopBin,98,0,98);
  
  for (int i = 0 ; i < numberOfEventsToUse ; i++ , converter->nextEvent()){
    
    cout << "Event : " << converter->getEventNumber() << endl;
    
    for (unsigned totalNumberOfChambers = 0 ; totalNumberOfChambers < cosmicTestChambersStack->getNumberOfChambers() ; totalNumberOfChambers++){
      
      aChamber = cosmicTestChambersStack->getChamberNumber(totalNumberOfChambers+1);
      
      for (int totalChannels = 0 ; totalChannels < 96 ; totalChannels++){
	
	//if (aChamber->getChannel(totalChannels+1)->hasMultipleHits()){
	
	  aChannel = aChamber->getChannel(totalChannels+1);
	  vectorWithHits = aChannel->getHits();
	  for (unsigned j=0 ; j < vectorWithHits.size() ; j++){    
	    assert(chamberToTest >= 0); // chamber number to be given with human number starting from 1, not from 0
	    if(totalNumberOfChambers == chamberToTest-1 ){
	      
	      histoOfFirstChamber->Fill(vectorWithHits.at(j),aChannel->getLinkBoardChannelNumberInChamber());
	      cout << aChannel->getOnlineNumber() << " " << vectorWithHits.at(j) << " " << endl;      
	      if(aChannel->hasMultipleHits() && j < vectorWithHits.size()-1){
		// only the hits after the first
		histoOfMultiHits->Fill(vectorWithHits.at(j+1),aChannel->getLinkBoardChannelNumberInChamber());
		timeDifference = vectorWithHits.at(j+1) - vectorWithHits.at(j);
		timeEvolutionHisto->Fill(timeDifference/10);
		cout << vectorWithHits.at(j+1) << " " << vectorWithHits.at(j) << " difference: " << timeDifference << endl;
		timeDifference = 0;
	      }
	      if(j == 0){
		// only the first hit
		histoOfFirstHits->Fill(vectorWithHits.at(j),aChannel->getOnlineNumber());
	      }
	    }
	  }
	//}
      }
    }
  }
  
  histoOfFirstChamber->GetXaxis()->SetTitle("Time of hit");
  histoOfFirstChamber->GetYaxis()->SetTitle("Channel number");
  histoOfMultiHits->GetXaxis()->SetTitle("Time of hit");
  histoOfMultiHits->GetYaxis()->SetTitle("Channel number");
  histoOfFirstHits->GetXaxis()->SetTitle("Time of hit");
  histoOfFirstHits->GetYaxis()->SetTitle("Channel number");
  histoOfFirstHits->SaveAs("firstHitsOnly.root");
  histoOfFirstHits->SetOption("lego");
  histoOfFirstHits->SaveAs("firstHitsOnly3D.root");
  histoOfMultiHits->SaveAs("secondaryHitsOnly.root");
  histoOfMultiHits->SetOption("lego");
  histoOfMultiHits->SaveAs("secondaryHitsOnly3D.root");
  histoOfFirstChamber->SaveAs("firstChamberMap.root");
  histoOfFirstChamber->SetOption("lego");
  histoOfFirstChamber->SaveAs("firstChamberMap3D.root");
  
  histoToSave = histoToSave+".root";
  timeEvolutionHisto->SaveAs(histoToSave.c_str());  
  
  rawdatafile->Close("R");
  rawdatafile->Delete();
}

/** Single function to study the efficiency. implement the calibration object to read the text based run details. 
 *  The calibration object should give 
 *  
 */ 

void localEfficiencyStudy(int _argc,char ** arg_v){
  
  string rawdataFilename=arg_v[1],ghentMap = arg_v[2];
  int numberOfEventsToUse = atoi(arg_v[3]);
  TFile * rawdatafile = new TFile(rawdataFilename.c_str());
  RPCRawConverter * converter = new RPCRawConverter(rawdatafile);
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChamberObjectsNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  
  /** create a run configuration object */
  
  RPCRunConfig * runConfig = new RPCRunConfig();
  //runConfig->readConfigurationFromJSONDocument();
  //runConfig->setChamberDetails();
  
  /** */
  
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster(numberOfChamberObjectsNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->setDataSourceForNchambers(numberOfChamberObjectsNeeded,converter->getChambersData());
  cosmicTestChambersStack->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  
  // vector of reference chambers needed    
  // site type 
  
  ESiteFileType siteType = converter->getCurrentFileType();
  int timeWindow = 0 ;
  int timeReference = 0 ;
  vector<int> vectorOfReferenceChambers;
  // for purpose of study, use two vectors for two different files. this to be assigned when the calibration object is introduced
  if(siteType == kIsCERNrawFile){
    vectorOfReferenceChambers.push_back(1); vectorOfReferenceChambers.push_back(5); vectorOfReferenceChambers.push_back(7);
  }
  if(siteType == kIsGENTrawFile){
    vectorOfReferenceChambers.push_back(1); vectorOfReferenceChambers.push_back(5);
  }
  
  // map of reconstructed hits within the chambers. 
  map<int,vector<double> > mapOfCurrentEventReconstructedHits;
  
  int allTracks = 0;
  int noTrackCounter = 0;
  
  RPCChamber * chamberObj; // pointer to point to each chamber in the loop
  RPCLinkBoardChannel * channelObj; // pointer to point to each channel of each chamber 
  
  for (int i = 0 ; i < numberOfEventsToUse ; i++ , converter->nextEvent()){
    cout << " Event : " << converter->getEventNumber() << endl;
    // in case of cern file, the first event is empty. so skip for i < 2 since it crashes for no record in the trigger object
    if (siteType == kIsCERNrawFile && i < 2) continue; // skip just once if its CERN file
    
    timeWindow = cosmicTestChambersStack->getTimeWindowForSiteType(siteType);
    timeReference = cosmicTestChambersStack->getTimeReferenceValueForSiteType(siteType);
    bool trackIsVertical = true;
    
    mapOfCurrentEventReconstructedHits = cosmicTestChambersStack->getReconstructedHits(vectorOfReferenceChambers,timeWindow,timeReference,trackIsVertical,2,siteType);
    
    if (mapOfCurrentEventReconstructedHits.empty()){ // if the map is empty there was no reconstructed track
      //cout << " No track reconstructed for event " << converter->getEventNumber() << endl;
      noTrackCounter ++;
      continue; // skip execution, there is no track reconstucted
    }
    
    else {
      
      for (unsigned totalChambers = 0; totalChambers < cosmicTestChambersStack->getNumberOfChambers() ; totalChambers++ ){
	
	chamberObj = cosmicTestChambersStack->getChamberNumber(totalChambers+1);
	assert(mapOfCurrentEventReconstructedHits[totalChambers+1].size());
	vector<double> partitionsAndChannelsVector = mapOfCurrentEventReconstructedHits[totalChambers+1];
	int channelNum = partitionsAndChannelsVector.at(partitionsAndChannelsVector.size()-1); // the last element is the channel
	int partitionNum = 0;
	bool channelGotHit = false;
	int chNum = channelNum;
	
	if (trackIsVertical){
	  partitionNum = partitionsAndChannelsVector.at(0); // if vertical track, there is only one entry for partition
	  channelGotHit = chamberObj->isMatchingFiredChannelInPartition(channelNum,partitionNum,5);
	  chNum += (partitionNum - 1)*(96/chamberObj->getClones());
	  chamberObj->findResidualValueForChannelInPartitions(channelNum,partitionsAndChannelsVector);
	  // if the channel doesn't got any hits, search for a hits in the next partitions and write 
	  if (!channelGotHit){
	    chamberObj->findResidualsInNeighbourPartitionsForChannelInPartition(partitionsAndChannelsVector);
	  }
	  chamberObj->getChannel(chNum)->incrementEfficiencyCounters(channelGotHit);
	}
	else {
	  for (unsigned partitionCounter = 0 ; partitionCounter < partitionsAndChannelsVector.size() - 1; partitionCounter++){
	    partitionNum = partitionsAndChannelsVector.at(partitionCounter);
	    channelGotHit = chamberObj->isMatchingFiredChannelInPartition(channelNum,partitionNum,5);
	    if(channelGotHit) break;
	  }
	  
	  chamberObj->incrementAbsoluteChannelCounters(channelGotHit,channelNum);
	}
	
	chamberObj->incrementEfficiencyCounters(channelGotHit);
	
      }
      
      allTracks++;
      
    }
  }
  
  cout << " all reconstructed tracks " << allTracks << endl;
  cout << " no tracks for " << noTrackCounter << " events" << endl;
  
  TH1F * efficiencyHisto,* efficiencyDistro, * residualsHisto, * part_residual_histo;
  TH1F * tracksDistributionHisto,* absolute_channel_efficiency_histo;  
  string efficiency_title ;
  string track_title ;
  
  /** writing the results of the application */ 
  /** TODO - make this part more short (an elegant) with defining a subroutine  */
  
  for (unsigned chamberNum = 0 ; chamberNum < cosmicTestChambersStack->getNumberOfChambers() ; chamberNum++){
    
    cout << " Number of all tracks chamber " << chamberNum+1 << " : " << cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getSumOfAllTracks() << endl;
    
    stringstream ss;
    ss << chamberNum+1;
    
    efficiency_title = "efficiencyHisto_" + ss.str()+".root";
    string efficiency_title_pic = "efficiencyHisto_" + ss.str()+".png";
    track_title = "trackHisto_" + ss.str()+".root";
    string track_title_pic = "trackHisto_" + ss.str()+".png";
    string eff_distro_title = "efficiency_distro_" + ss.str()+".root";
    string res_distro_title = "residuals_distro_" + ss.str()+".root";
    string part_res_distro_title = "partition_residual_distro" + ss.str() + ".root" ;
    string absolute_channel_efficiency = "abs_channel_eff_" + ss.str() + ".root";
    
    ss.clear();
    efficiencyHisto = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getHistogramOfChannelsEfficiency(efficiency_title.c_str());
    efficiencyHisto->SetStats(false);
    efficiencyHisto->SaveAs(efficiency_title.c_str());
    
    tracksDistributionHisto = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getHistogramOfTracksVsChannels(track_title.c_str());    
    tracksDistributionHisto->SetStats(false);
    tracksDistributionHisto->SaveAs(track_title.c_str());
    
    efficiencyDistro = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getDistributionOfChannelsEfficiency(eff_distro_title.c_str());
    efficiencyDistro->SetMarkerColor(kBlue);
    efficiencyDistro->SetFillColor(kBlue);
    
    efficiencyDistro->SaveAs(eff_distro_title.c_str());
    residualsHisto = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getResidualsHistogram(res_distro_title.c_str());
    residualsHisto->SetMarkerColor(kBlue);
    residualsHisto->SetFillColor(kBlue);
    
    residualsHisto->SaveAs(res_distro_title.c_str());
    part_residual_histo = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getNeighbourPartitionHitsHistogram(part_res_distro_title.c_str());
    part_residual_histo->SaveAs(part_res_distro_title.c_str());
    
    absolute_channel_efficiency_histo = cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getHistogramOfAbsoluteChannelsEfficiency(absolute_channel_efficiency.c_str());
    absolute_channel_efficiency_histo->SaveAs(absolute_channel_efficiency.c_str());
    
    absolute_channel_efficiency_histo->Delete();
    part_residual_histo->Delete();
    residualsHisto->Delete();
    efficiencyDistro->Delete();
    efficiencyHisto->Delete();
    tracksDistributionHisto->Delete();
    
  }
  
  rawdatafile->Close("R");
  rawdatafile->Delete();
  
}

/** Function for converter class tests - for Mariana and Francesco practice */

void converterTests(int _argc,char ** arg_v){
  
  TFile * myFile = new TFile(arg_v[1]);
  RPCRawConverter * converter = new RPCRawConverter(myFile);
  
  //converter->getChannelHitsVectorFromChamber(1);
  converter->setGhentTDCtoRPCmap(arg_v[2]);
  
  
  RPCChamber * chamber = new RPCChamber(kRPC_RE_4_2_chamber);
  chamber->allocAndInit();
  chamber->setStripsHitsDataFromSource(converter->getChamberVector(1));
  
  while(converter->nextEvent()){
    cout << " Event : " << converter->getEventNumber() << endl;
     //chamber->setStripsHitsDataFromSource(converter->getChamberVector(1));
    //chamber->findAllClustersForTriggerTimeReferenceAndTimeWindow();
    /*
    for (int i = 0 ; i < chamber->getNumberOfClusters() ; i++){
      
      vector<int> allMyChannelsWithHits;
      allMyChannelsWithHits = chamber->getClusterNumber(i+1);
      
      for (unsigned j = 0 ; j < allMyChannelsWithHits.size() ; j ++){
	
	int myChannelHitTime = 0 ;
	myChannelHitTime =  chamber->getChannel(j+1)->getHits().at(0);
	
      }
      
    }
    */
  
    
  }
  
}

void configObjectsTest(int _argc, char* _argv[]){
  
  RPCLinkBoard * aBoard = new RPCLinkBoard(kRPC_RE_4_2_chamber);
  aBoard->allocAndInit();
  vector<double> vmons;
  
  vmons.push_back(0.5);
  vmons.push_back(0.6);
  vmons.push_back(0.7);
  
  RPCChamberConditions * conditions = new RPCChamberConditions();
  aBoard->setCurrentRunDetails(conditions);  
  aBoard->getBasicChamberConditions()->setGapsVmon(vmons);
  
  cout << aBoard->getBasicChamberConditions()->getGapsVmon().at(2) << endl;
  cout << aBoard->getExtendedChamberConditions()->getHVmonForGap(3) << endl;
  
  //delete aBoard;
  
  RPCRunConfig * config = new RPCRunConfig();
  vector<RPCChamberConditionsBase*> conditionsVector;
  conditionsVector.push_back(conditions);
  config->setChamberDetails(conditionsVector);
  cout << config->getBasicConditionsForChamber(1)->getGapsVmon().at(2) << endl;

}



