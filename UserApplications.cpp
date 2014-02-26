/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 06.08.13
*/

#include "UserApplications.h"
#include <occi.h>
#include <boost/lexical_cast.hpp>

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
  
  //RPCDBReader * reader = new RPCDBReader;
  RPCRunConfig *runconfig = new RPCRunConfig;
  
  runconfig->readConfigurationFromDBforRunAndSite(1163,"Ghent");
  for (unsigned i = 0 ; i < runconfig->getChambersDetails().size() ; i++){
    RPCChamberConditions * cond = runconfig->getConcreteConditionsForChamber(i+1);
    cout << cond->getChamberName() << " " << cond->getIsReference() << " " << cond->getHVsetForGap(1) << endl;
    cout << cond->getChamberName() << " " << cond->getLabelForGap(3) << " " << cond->getGapsCurrent().at(0) <<  " " << cond->getGapsCurrent().at(1) << " " << cond->getGapsCurrent().at(2) << " " << cond->getShelfNumber() <<  endl;
  }
  
  cout << runconfig->getTemperature() << " " << runconfig->getHumidity() << " " << runconfig->getPressure() << endl;
  
  //reader->openNewConnection();
  
  //reader->getChambersDataForRunAndSite(2202,"CERN");  
  //reader->printResultWithNcolumns(8);
  
  //reader->getChambersDataForRunAndSite(1163,"Ghent");
  //reader->printResultWithNcolumns(23);
  
  /*
  reader->getEnvironmentDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(3);
  */
  /*
  reader->getChambersDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(8);
  
  reader->getEnvironmentDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(3);
  
  reader->getEnvironmentDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(3);
  */
  //reader->closeCurrentConnection();  
  
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
  numberOfEventsToUse = converter->getTotalEvents();
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
  string jsonFileWithConfig = arg_v[4] , runToUse = arg_v[5];
  bool DEBUG = atoi(arg_v[6]);
  int timeWindow = 0 ,  timeReference = 0 ; // time windowses to use
  vector<int> vectorOfReferenceChambers; // vector of consecutive reference chambers, filled by the run config object
  int allTracks = 0, verticalTracks=0; int noTrackCounter = 0; // track counters just to keep eye on the stats
  // some pointer for further use 
  RPCChamber * chamberObj; // pointer to point to each chamber , use it in a loop
  RPCLinkBoardChannel * channelObj; // pointer to point to each channel of each chamber , use it in a loop
  
  /** this following structure (the vector) is used to get the result from the reconstruction. If track is reconstructed,
   * it contains the partitions and channel numbers obtained from the reconstructed line (from the fit). 
   * The structure is - the int (the key) gives the chamber number, while each vector of doubles corresponding 
   * to the key is structured like this : the last vector value is the channel number reconstructed (since the result given by the fit is not integer)
   * and from the first to the last-1 the values are the partitions (1,2 to or 3) where we may search for hit using the 
   * channel number, in case the track was in more than one partition. If the track was in one partition only (vertical track),
   * then the 
   */
  
  map<int,vector<double> > mapOfCurrentEventReconstructedHits; 
  
  /** file converter, that addapts different inputs as common output, and loop on the events */

  RPCRawConverter * converter = new RPCRawConverter(rawdatafile);
  cout << "all events " <<  converter->getTotalEvents() << endl;  
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChamberObjectsNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  numberOfEventsToUse = converter->getTotalEvents();
  
  /** create a run configuration object, use it to configure the execution. JSON file for CERN, DB records for GHENT */
  
  RPCRunConfig * runConfig = new RPCRunConfig();
  /** get the file type */
  
  // put the different cases of run configuration reading within the two cases
  
  ESiteFileType siteType = converter->getCurrentFileType();
 
  if(siteType == kIsCERNrawFile){
    runConfig->readConfigurationFromJSONDocument(jsonFileWithConfig,runToUse);
  }
  
  if(siteType == kIsGENTrawFile){
    runConfig->readConfigurationFromDBforRunAndSite(boost::lexical_cast<int>(runToUse),"Ghent");
  }
  
  vectorOfReferenceChambers = runConfig->getReferenceChambers();
  
  // run details configured
  
  /* Create cluster of chambers, and set the run details to each chamber in the cluster */
  
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster(numberOfChamberObjectsNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->setDataSourceForNchambers(numberOfChamberObjectsNeeded,converter->getChambersData());
  cosmicTestChambersStack->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  
  for(unsigned i = 0 ; i < cosmicTestChambersStack->getNumberOfChambers() ; i++){
    cosmicTestChambersStack->getChamberNumber(i+1)->resetEfficiencyCounters();
  }  
  
  for (unsigned chambersInConfig = 0; chambersInConfig < runConfig->getChambersDetails().size() ; chambersInConfig++){
    RPCChamberConditions * conditions = runConfig->getConcreteConditionsForChamber(chambersInConfig+1);
    cosmicTestChambersStack->getChamberNumber(conditions->getShelfNumber())->setCurrentRunDetails(conditions);
    cosmicTestChambersStack->getChamberNumber(conditions->getShelfNumber())->resetEfficiencyCounters();
  }
  
  // actual analysis program apart from definitions and config
  
  for (int i = 0 ; i < numberOfEventsToUse ; i++ , converter->nextEvent()){
    
    //cout << " Event : " << converter->getEventNumber() << endl; // print the event if you like
    
    if (siteType == kIsCERNrawFile && i < 2) continue; // skip just once if its CERN file. The first event is always empty
    
    timeWindow = cosmicTestChambersStack->getTimeWindowForSiteType(siteType);
    timeReference = cosmicTestChambersStack->getTimeReferenceValueForSiteType(siteType);
    bool trackIsVertical = true;
    mapOfCurrentEventReconstructedHits = cosmicTestChambersStack->getReconstructedHits(vectorOfReferenceChambers,timeWindow,timeReference,trackIsVertical,2,siteType);
    
    if (mapOfCurrentEventReconstructedHits.empty()){ // if the map is empty there was no reconstructed track
      //cout << " No track reconstructed for event " << converter->getEventNumber() << endl;
      noTrackCounter ++;
      continue; // skip execution, there is no track reconstucted
    }
    
    else { // else, track was found. search was it vertical (passing single partition) or passed more then one.
      
      for (unsigned totalChambers = 0; totalChambers < cosmicTestChambersStack->getNumberOfChambers() ; totalChambers++ ){
	
	chamberObj = cosmicTestChambersStack->getChamberNumber(totalChambers+1);
	chamberObj->writeClusterSizeValues(); //
	chamberObj->writeTimeEvolutionValues(); //
	/* // check on the multiple hits, remove in next version
	for (int ch=0 ; ch < 96 ; ch++){
	  if(chamberObj->getChannel(ch+1)->hasMultipleHits()){
	    cout << "Event " << converter->getEventNumber() << " Chamber " << totalChambers+1 << " Channel " << ch+1 << " hits";

	    for(unsigned hitnumber=0;hitnumber < chamberObj->getChannel(ch+1)->getHits().size() ; hitnumber++){
	      cout << " " << chamberObj->getChannel(ch+1)->getHits().at(hitnumber);
	    }
	    
	    cout << endl;
	  }
	}
	*/
	
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
	    
	    /*
	    if (DEBUG) {
	    if (chamberObj->getExtendedChamberConditions() != NULL ){
	      
	      cout << " Cannot found near enough hits for track found in event : " << converter->getEventNumber() << " in chamber " << totalChambers+1 << endl;
	      
	      for (unsigned iiii = 0 ; iiii < mapOfCurrentEventReconstructedHits.size() ; iiii++){
		vector<double> tempVector = mapOfCurrentEventReconstructedHits[iiii+1];
		 cout << "chamber number : " <<  iiii+1 << " partition number " << tempVector.at(0) << " channel " << tempVector.at(tempVector.size() - 1) << " " << " absolute channel " << (partitionNum-1)*32 + tempVector.at(tempVector.size() - 1) << endl;
		 cout << "partitions number : ";
		 for (unsigned aCounter = 0 ; aCounter < tempVector.size() - 1 ; aCounter++) { cout << tempVector.at(aCounter) << " "; }
		 cout << endl;
	      }
	       
	      cout << " Hits in the chamber " << totalChambers+1 << " channels : " << endl;
	      for (int allStrips__ = 1 ; allStrips__ <= 32 ; allStrips__++){
		channelObj = chamberObj->getChannel((partitionNum-1)*32 + allStrips__);
		if (channelObj->hasHit()) {
		  cout << (partitionNum-1)*32 + allStrips__ << " ";
		}
	      }
	      cout << endl;
	      
	    }
	    
	    }
	    */  
	    
	  }  
	  
	  chamberObj->getChannel(chNum)->incrementEfficiencyCounters(channelGotHit);
	  chamberObj->incrementEfficiencyCounters(channelGotHit);  
	}
	else {
	  for (unsigned partitionCounter = 0 ; partitionCounter < partitionsAndChannelsVector.size() - 1; partitionCounter++){
	    partitionNum = partitionsAndChannelsVector.at(partitionCounter);
	    channelGotHit = chamberObj->isMatchingFiredChannelInPartition(channelNum,partitionNum,5);
	    if(channelGotHit) break;
	  }  
	  chamberObj->incrementAbsoluteChannelCounters(channelGotHit,channelNum);
	}
      }
      if (trackIsVertical) verticalTracks++;
      allTracks++;
      
    }
  }
  
  // and thats it, the entire program, here comes the output
  
  cout << " all reconstructed tracks " << allTracks << endl;
  cout << " vertical tracks " << verticalTracks << endl;
  cout << " no tracks for " << noTrackCounter << " events" << endl;
  
  TH1F * efficiencyHisto,* efficiencyDistro, * residualsHisto, * part_residual_histo;
  TH1F * tracksDistributionHisto,* absolute_channel_efficiency_histo,* clsSizeHistograms;  
  TH2F * timeEvoHistoPointer;
  string efficiency_title ;
  string track_title ;
  
  /** writing the results of the application */ 
  /** TODO - make this part more short (an elegant) with defining a subroutine (the histogram write part) */
  
  for (unsigned chamberNum = 0 ; chamberNum < cosmicTestChambersStack->getNumberOfChambers() ; chamberNum++){
    
    if (cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getExtendedChamberConditions() == NULL) continue ;
    
    
    cout << " Number of all tracks chamber " << chamberNum+1 << " : " << cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getSumOfAllTracks() << endl;
    cout << " Total and efficient tracks " << cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getNumberOfEfficientTracks() << " "
    << cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getNumberOfAllTracks() << endl;
    cout << " Total efficiency " << cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getChamberEfficiency() << endl;
    
    stringstream ss;
    ss << chamberNum+1;
    
    string chamber_Name =  "_" + ss.str() + "_" + cosmicTestChambersStack->getChamberNumber(chamberNum+1)->getExtendedChamberConditions()->getChamberName();
    
    efficiency_title = "efficiencyHisto_"+runToUse + chamber_Name+".root";
    string efficiency_title_pic = "efficiencyHisto_"+runToUse + chamber_Name+".png";
    track_title = "trackHisto_"+runToUse + chamber_Name+".root";
    string track_title_pic = "trackHisto_"+runToUse + chamber_Name+".png";
    string eff_distro_title = "efficiency_distro_"+runToUse + chamber_Name+".root";
    string res_distro_title = "residuals_distro_"+runToUse + chamber_Name+".root";
    string part_res_distro_title = "partition_residual_distro"+runToUse + chamber_Name + ".root" ;
    string absolute_channel_efficiency = "abs_channel_eff_"+runToUse + chamber_Name + ".root";
    
    double stripEfficiencySum = 0;
    double stripsWithAtLeastOneTrack = 0;
    int sum_OfAllTracks = 0 ;
    int sum_EffTracks = 0;
    //setprecision(9);
    
    chamberObj = cosmicTestChambersStack->getChamberNumber(chamberNum+1);
    
    for (int i = 0 ; i < 96 ; i++){
      channelObj = chamberObj->getChannel(i+1);
      if (channelObj->getAllTracks() > 0 ){
	stripEfficiencySum += channelObj->getEfficiency();
	stripsWithAtLeastOneTrack ++;
	sum_OfAllTracks += channelObj->getAllTracks();
	sum_EffTracks += channelObj->getEfficientTracks();
      }
    }
    
    // now check the efficiency as average of the strip efficiency
    cout << "Channel efficiency average for " << stripsWithAtLeastOneTrack << " strips with hits : " << stripEfficiencySum/stripsWithAtLeastOneTrack << ". Efficiency sum " << stripEfficiencySum << endl;
    cout << "Track stats : sum of all tracks in strips : " << sum_OfAllTracks << ", sum of efficient tracks : " << sum_EffTracks << endl;
    
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
    
    for(int i = 0 ; i < chamberObj->getClones() ; i++){
      clsSizeHistograms = chamberObj->getHistogramOfClusterSizeForPartition(i+1);
      string histoFileToSave = clsSizeHistograms->GetName();
      histoFileToSave += ".root";
      clsSizeHistograms->SaveAs(histoFileToSave.c_str());
    }
    
    timeEvoHistoPointer = chamberObj->getTimeEvolutionProfileHistogram(("TimeResolutionPerStrip_"+chamberObj->getExtendedChamberConditions()->getChamberName()).c_str());
    timeEvoHistoPointer->SaveAs(("TimeResolutionPerStrip_"+chamber_Name+".root").c_str());
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
  
  
  RPCRunConfig * config = new RPCRunConfig();
  string file = _argv[1];
  config->readConfigurationFromJSONDocument(file,_argv[2]);
  
  //cout << config->getHumidity() << " " << config->getPressure() << " " << config->getTriggerLayer() << " " << config->getRunNumber() << endl;
  for (unsigned refCh = 0 ; refCh < config->getReferenceChambers().size() ; refCh ++){
    cout << config->getReferenceChambers().at(refCh) << endl;
  }
  
  for (unsigned chamberNum = 0 ; chamberNum < config->getChambersDetails().size();chamberNum++){
    
    RPCChamberConditions * condition = config->getConcreteConditionsForChamber(chamberNum+1);
    cout << "name " << condition->getChamberName() << endl;
    cout << "shelf " << condition->getShelfNumber() << endl;
    cout << "HV cable " << condition->getHVcable() << endl;
    cout << "LV cable " << condition->getLVcable() << endl;
    cout << "TDC cable " << condition->getTDCcable() << endl;
    cout << "Is reference " << condition->getIsReference() << endl;
    cout << "HV set on chamber " << condition->getHVset() << endl;
    
    for (unsigned gap=0;gap < condition->getGapLabels().size() ; gap++){
      cout << "gap label " << condition->getLabelForGap(gap+1) << " current " << condition->getCurrentForGap(gap+1)
      << " hvset " << condition->getHVsetForGap(gap+1) << " hvmon " << condition->getHVmonForGap(gap+1) << endl;
    }
    
    condition->getLabelForGap(RPCChamberConditionsBase::Etopnarrow);
    
    for(unsigned febs = 0 ; febs < condition->getFEBTresholds().size() ; febs++){
      
      cout << condition->getFEBTresholdForBoard(febs+1)<< " " ;
    }
    
    cout << endl;
  }

}

void getGlobalPlotsForRunStack(int _argc,char * _argv[]){
  
  string runFolder = _argv[1];
  string jsonFile = _argv[2];
  // get the number of chambers under test from the JSON and create objects only for them
  vector<string> runlist;
  RPCRunConfig * config = new RPCRunConfig();
  runlist = config->getRunListFromJSONfile(jsonFile);
  config->readConfigurationFromJSONDocument(jsonFile,runlist.at(0)); // get the number of chambers 
  //int chambersNeeded = config->getChambersDetails().size();
  RPCChambersCluster * cluster = new RPCChambersCluster(9,0,kRPC_RE_4_3_chamber);
  
  
  for (unsigned run = 0 ; run < runlist.size() ; run++){
    config->readConfigurationFromJSONDocument(jsonFile,runlist.at(run));
    for (unsigned chamberConfig = 0 ; chamberConfig < config->getChambersDetails().size();chamberConfig++){
      RPCChamberConditions * conditions = config->getConcreteConditionsForChamber(chamberConfig+1);
      cluster->getChamberNumber(conditions->getShelfNumber())->setCurrentRunDetails(conditions);
    }
    cout << runlist.at(run) << endl;
    for (unsigned chamber = 0 ; chamber < cluster->getNumberOfChambers() ; chamber++){
      RPCLinkBoard * aChamber = cluster->getChamberNumber(chamber+1);
      if(aChamber->getExtendedChamberConditions() != NULL && !aChamber->getExtendedChamberConditions()->getIsReference()){
	
	cout << aChamber->getExtendedChamberConditions()->getChamberName() << " shelf " << aChamber->getExtendedChamberConditions()->getShelfNumber() << " chamber in cluster " << chamber+1 << endl; 
	string shelfNumberAsString = boost::lexical_cast<string>(chamber+1);
	string efficiencyFilePath = runFolder+runlist.at(run)+"/";
	string efficiencyFileName = "efficiency_distro_"+shelfNumberAsString+".root";
	TFile * efficiencyFile = new TFile((efficiencyFilePath+efficiencyFileName).c_str(),"READ","in");
	TH1F * pointerToHisto;
	if (efficiencyFile->IsOpen()){
	  
	  pointerToHisto = dynamic_cast<TH1F*>(efficiencyFile->Get(efficiencyFileName.c_str()));
	  cout << config->getTriggerLayer() << " " << aChamber->getExtendedChamberConditions()->getHVset() 
	  << " " << pointerToHisto->GetMean() << endl;
	  double hmean = pointerToHisto->GetMean();
	  aChamber->setEfficiencyVsHVentryForMode(aChamber->getExtendedChamberConditions()->getHVset(),hmean,0,
						  config->getTriggerLayer());
	}
	
	efficiencyFile->Close("R");
	efficiencyFile->Delete();
	
      }
      
    }
    
  }
  
  for (unsigned chamber = 0 ; chamber < cluster->getNumberOfChambers() ; chamber++){
    RPCLinkBoard * ch = cluster->getChamberNumber(chamber+1);
    if (ch->getExtendedChamberConditions() != NULL && !ch->getExtendedChamberConditions()->getIsReference()){
      ch->drawNestedSigmoidPlotForAllModes(ch->getExtendedChamberConditions()->getChamberName());
    }
  }
  
}

void getDistributionOfEventsByClustersInRefChambers(int _argc,char * arg_v[]){
  string rawdataFilename=arg_v[1],ghentMap = arg_v[2];
  ///int numberOfEventsToUse = atoi(arg_v[3]);
  TFile * rawdatafile = new TFile(rawdataFilename.c_str());
  string jsonFileWithConfig = arg_v[4] , runToUse = arg_v[5];
  
  RPCRawConverter * converter = new RPCRawConverter(rawdatafile);
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChamberObjectsNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  
  /** create a run configuration object */
  
  RPCRunConfig * runConfig = new RPCRunConfig();
  runConfig->readConfigurationFromJSONDocument(jsonFileWithConfig,runToUse);
  //numberOfEventsToUse = converter->getTotalEvents();
  
  RPCChambersCluster * cosmicTestChambersStack = new RPCChambersCluster(numberOfChamberObjectsNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  cosmicTestChambersStack->setDataSourceForNchambers(numberOfChamberObjectsNeeded,converter->getChambersData());
  cosmicTestChambersStack->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  
  for (unsigned chambersInConfig = 0; chambersInConfig < runConfig->getChambersDetails().size() ; chambersInConfig++){
    RPCChamberConditions * conditions = runConfig->getConcreteConditionsForChamber(chambersInConfig+1);
    cosmicTestChambersStack->getChamberNumber(conditions->getShelfNumber())->setCurrentRunDetails(conditions);
  }
  
  ESiteFileType siteType = converter->getCurrentFileType();
  int timeWindow = 0 ;
  int timeReference = 0 ;
  vector<int> vectorOfReferenceChambers;
  if(siteType == kIsCERNrawFile){
    vectorOfReferenceChambers = runConfig->getReferenceChambers();
  }
  if(siteType == kIsGENTrawFile){
    vectorOfReferenceChambers.push_back(1); vectorOfReferenceChambers.push_back(5); // ghent references are fixed
  }
  
  
  TH1F * histoOfEventsWithClusterInRefChambers = new TH1F("eventscls","Distribution of events by track reconstruction capability",20,-2,2);
  histoOfEventsWithClusterInRefChambers->GetYaxis()->SetTitle("Number of events");
  histoOfEventsWithClusterInRefChambers->GetXaxis()->SetTitle("Track not possible (-1) vs track possible (1) events");
  histoOfEventsWithClusterInRefChambers->SetFillColor(kBlue);
  TH1F * numberOfClustersInRefChambers = new TH1F("clsnumdistr","Distribution of events by number of ref chambers with at least 1 cluster",24,0,5);
  numberOfClustersInRefChambers->GetXaxis()->SetTitle("Reference chambers with at least 1 cluster");
  numberOfClustersInRefChambers->GetYaxis()->SetTitle("Number of events");
  numberOfClustersInRefChambers->SetFillColor(kBlue);
  TH1F * samePartitionClusters = new TH1F ("samepart","Clusters in same partition vs clusters in different partition - all clusters",20,-2,2);
  samePartitionClusters->GetXaxis()->SetTitle("Same partition (1) vs diff partition (-1) clusters");
  samePartitionClusters->GetYaxis()->SetTitle("Number of events");
  samePartitionClusters->SetFillColor(kBlue);
  TH1F * samePartitionClusters2 = new TH1F ("samepart2","Clusters in same partition vs clusters in different partition - 2 clusters",20,-2,2);
  samePartitionClusters2->GetXaxis()->SetTitle("Same partition (1) vs diff partition (-1) clusters");
  samePartitionClusters2->GetYaxis()->SetTitle("Number of events");
  samePartitionClusters2->SetFillColor(kBlue);
  TH1F * samePartitionClusters3 = new TH1F ("samepart3","Clusters in same partition vs clusters in different partition - 3 clusters",20,-2,2);
  samePartitionClusters3->GetXaxis()->SetTitle("Same partition (1) vs diff partition (-1) clusters");
  samePartitionClusters3->GetYaxis()->SetTitle("Number of events");
  samePartitionClusters3->SetFillColor(kBlue);
  
  
  while(converter->nextEvent()){
    
    cout << "Event: " << converter->getEventNumber() << endl;
    if (siteType == kIsCERNrawFile && converter->getEventNumber() < 2) continue; // skip just once if its CERN file
    
    timeWindow = cosmicTestChambersStack->getTimeWindowForSiteType(siteType);
    timeReference = cosmicTestChambersStack->getTimeReferenceValueForSiteType(siteType);
    
    for(unsigned chamber = 0 ; chamber < cosmicTestChambersStack->getNumberOfChambers() ; chamber++){
      cosmicTestChambersStack->getChamberNumber(chamber+1)->findAllClustersForTriggerTimeReferenceAndTimeWindow(timeReference,timeWindow);
    }
    
    //cosmicTestChambersStack->getChamberNumber(1)->getStrip()->getAllTracks()
    
    bool clustersFoundInAllReferences = true;
    int numberOfRefChambersWithCluster = 0 ;
    int eventIsGood = 1;
    int clustersInSamePartition = 1;
    vector<int> clusterPartitionsCheck;
    
    for(unsigned refchamber = 0 ; refchamber < vectorOfReferenceChambers.size();refchamber++){
      
      if (cosmicTestChambersStack->getChamberNumber(vectorOfReferenceChambers.at(refchamber))->getNumberOfClusters() == 0){
	clustersFoundInAllReferences = false;
      }
      else{
	numberOfRefChambersWithCluster++;
	cout << cosmicTestChambersStack->getChamberNumber(vectorOfReferenceChambers.at(refchamber))->getExtendedChamberConditions()->getChamberName() << " ";
	clusterPartitionsCheck.push_back(cosmicTestChambersStack->getChamberNumber(vectorOfReferenceChambers.at(refchamber))->getXYCoordinatesOfCluster(1).at(1));
      }
    }
    
    if(clusterPartitionsCheck.size() > 1){
      for(unsigned vsizee = 0 ; vsizee < clusterPartitionsCheck.size() - 1;vsizee++){
	if (clusterPartitionsCheck.at(vsizee+1) != clusterPartitionsCheck.at(vsizee) ) clustersInSamePartition = -1;
      }
      samePartitionClusters->Fill(clustersInSamePartition);
      if (clusterPartitionsCheck.size() == 2){
	samePartitionClusters2->Fill(clustersInSamePartition);
      }
      else {
	samePartitionClusters3->Fill(clustersInSamePartition);
      }
      
    }
    
    if(!clustersFoundInAllReferences) eventIsGood = -1;
    histoOfEventsWithClusterInRefChambers->Fill(eventIsGood);
    numberOfClustersInRefChambers->Fill(numberOfRefChambersWithCluster);
    
  }
  
  histoOfEventsWithClusterInRefChambers->SaveAs(("EventsTrackStatistics_"+runToUse+".root").c_str());
  numberOfClustersInRefChambers->SaveAs(("RefChambersWithClusterStat_"+runToUse+".root").c_str());
  samePartitionClusters->SaveAs(("SamePartitionClusters_"+runToUse+".root").c_str());
  samePartitionClusters2->SaveAs(("SamePartitionClusters2_"+runToUse+".root").c_str());
  samePartitionClusters3->SaveAs(("SamePartitionClusters3_"+runToUse+".root").c_str());
  
}


