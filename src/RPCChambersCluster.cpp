/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCChambersCluster.h"

using namespace std;
using namespace ROOT::Math;

//using namespace RooFit;

void RPCChambersCluster::createNewClusterOfChambersWithRE4type(const int & nChambers,const string & RE4type){
  if (this->_numberOfChambers){ // not ok, some chambers are already created , one has first to erase them
    cout << "The object still holds existing chamber objects ! use deleteAllChambers method to clear them first." << endl;
  }
  else { // its ok to proceed
    //this->_chambersPointer = new RPCChamber[nChambers];
    for (int i = 0 ; i < nChambers;i++){
      RPCChamber * aChamber = new RPCChamber(RE4type);
      aChamber->allocAndInit();
      this->_chambersPointer[i] = aChamber;
    }
    this->_numberOfChambers = nChambers;
  }
}

void RPCChambersCluster::createNewClusterOfTriggerObjects(const int & nTriggerObjects){
  if (this->getNumberOfTriggerObjects()){
    cout << "The object still holds existing chamber objects ! use deleteAllTriggers method to clear them first." << endl;
  }
  else {
    for(int i = 0 ; i < nTriggerObjects ; i++){
      RPCChamber * aTriggerObject = new RPCChamber(kRPC_RE_4_2_chamber); // defined in raw types file, take a look
      aTriggerObject->allocAndInit();
      aTriggerObject->setIsTriggerChamber(true);
      this->_triggerObjectsPointer[i] = aTriggerObject;
    }
    this->_numberOfTriggers = nTriggerObjects;
  }
}

void RPCChambersCluster::setDataSourceForNchambers(const int & nChambers,const vector<vector<vector< unsigned> > > & dataSource){
  
  for (int i = 0 ; i < nChambers ; i ++){
    this->getChamberNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }
}

void RPCChambersCluster::setDataSourceForNtriggerObjects(const int & nTriggerObject,const vector<vector<vector< unsigned> > > & dataSource){
  
  for (int i = 0 ; i < nTriggerObject ; i++ ){
    this->getTriggerObjectNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }  
}

RPCChambersCluster::RPCChambersCluster (){
  this->_numberOfChambers = 0;
  this->_numberOfTriggers = 0;
}

RPCChambersCluster::RPCChambersCluster (const int & numberOfChambers,const int & numberOfTriggerObjs,const string & RE4TypeOfChambers){
  //RPCChambersCluster(); // destructor is called if the default constructor is used as normal method
  this->_numberOfChambers = 0;
  this->_numberOfTriggers = 0;
  this->createNewClusterOfChambersWithRE4type(numberOfChambers,RE4TypeOfChambers);
  this->createNewClusterOfTriggerObjects(numberOfTriggerObjs);
  
}

RPCChambersCluster::~RPCChambersCluster () {
  //cout << " destructor called " << endl;
  this->deleteAllObjects();
  // clear other resources here
  
}

void RPCChambersCluster::deleteAllChambers () {
  if (this->getNumberOfChambers() > 0){
    for (unsigned i=0;i < this->getNumberOfChambers();i++) {
      delete this->_chambersPointer.at(i);
    }
  }
  this->_chambersPointer.clear();
  this->_numberOfChambers = 0;
}

void RPCChambersCluster::deleteAllTriggerObjects(){
  if (this->getNumberOfTriggerObjects() > 0){
    RPCChamber * pointer;
    for (int i=0; i < this->getNumberOfTriggerObjects();i++){
      delete this->_triggerObjectsPointer.at(i);
    }
  }
  this->_triggerObjectsPointer.clear();
  this->_numberOfTriggers = 0;
}

void RPCChambersCluster::deleteAllObjects(){
  this->deleteAllChambers();
  this->deleteAllTriggerObjects();
}

RPCChamber * RPCChambersCluster::getChamberNumber(const int & chamberNumber){
  return dynamic_cast<RPCChamber*>(this->_chambersPointer.at(chamberNumber-1));
}

RPCChamber * RPCChambersCluster::getTriggerObjectNumber(const int & triggerObjectNumber){
  return  dynamic_cast<RPCChamber*>(this->_triggerObjectsPointer.at(triggerObjectNumber-1));
}

/** analisys methods goes here, define some concrete logic that extract data from the events */

void RPCChambersCluster::variousStudyExperimentalFunction(TFile * fileToSave,TH1F * histo[10],const int & eventNum){
  
  // Save file is used to save all the reconstructed graphs with tracks in order to inspect them
  
  RPCChamber * currentChamberObj;
  RPCChamber * triggerObj;
  RPCLinkBoardChannel * currentChannelObj;
  int numberOfChambers = this->getNumberOfChambers();
  triggerObj = this->getTriggerObjectNumber(1);
  TGraphErrors * graphToFit;
  //currentChamberObj = this->getChamberNumber(1);
  int numberOfReferenceChambers = 0; // change this value for the three cases - CERN, Ghent and BARC
  vector<int> tempCluster;
  
  TH1F * hist1,* hist2,* hist3,* hist4, * numberOfClustersHisto, * topMinusEachChamberAvg, * bottomMinusEachChamberAvg,*clsSize ,
  * numberOfClustersInSamePartitions, * histClustersPartitionDistr, * SingleMultiHits;
  
  hist1 = histo[0];
  hist2 = histo[1];
  hist3 = histo[2];
  hist4 = histo[3];
  numberOfClustersHisto = histo[4];
  topMinusEachChamberAvg = histo[5];
  bottomMinusEachChamberAvg = histo[6];
  clsSize = histo[7];
  histClustersPartitionDistr = histo[8];
  SingleMultiHits = histo[9];
  
  int timeReference = 0;
  int timeWindow = 0;
  int firstScintilatorTime = 0 ;
  int secondScintilatorTime = 0;
  int difference_reference = 0;
  int coincidence_time = 0 ;
  if (triggerObj->getChannel(32)->hasHit()){
    coincidence_time = triggerObj->getChannel(32)->getHits().at(0);
  }
  
  ESiteFileType siteType = kIsCERNrawFile; // later make the method to take file type argument and to use it in here
  
  //TH1F * firstTriggerEntries  = new TH1F("ScintStats1","ScintStats1",0,500,500);
  //firstTriggerEntries->SetLineColor(kBlue);
  //firstTriggerEntries->SetFillColor(kBlue);
  
  switch (siteType) 
  {
    case kIsCERNrawFile:
      
      timeWindow = 500;
      
      numberOfReferenceChambers = 3;
      
      int countTwoHits=0;
      for (int i = 0 ; i < 13 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( difference_reference == 0 || ( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      difference_reference = 0;
      
      for (int i = 13 ; i < 31 ; i++) {
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( difference_reference == 0 || ( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    secondScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      //timeReference = timeReference/2;
      /*
    case kIsGENTrawFile:
      timeWindow = 20;
      for(int i = 0 ; i < 32 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit()) {
	  triggerObj->getChannel(i+1)->getHits().at(0);
	}
	break;
      }
    case kIsBARCrawFile:
      timeWindow = 0; 
      timeReference = 0;
      */
      
  }
  
  timeReference = (firstScintilatorTime+secondScintilatorTime)/2;
  
  //cout << " time reference : " << timeReference << endl;
  //cout << " trigger entries : ";
  for (int i=0; i < 32 ; i++){
    if (triggerObj->getChannel(i+1)->hasHit()){
//       cout << " trig channel : " << i+1 << " " << triggerObj->getChannel(i+1)->getHits().at(0) << " ";
    }
  }
  
  cout << endl;
//   cout << " most close trigger entries : top " << firstScintilatorTime << " bottom : " << secondScintilatorTime;
//   cout << endl;
  
  
  hist1->Fill(abs(firstScintilatorTime-secondScintilatorTime));
  hist2->Fill(coincidence_time - firstScintilatorTime);
  hist3->Fill(coincidence_time - secondScintilatorTime);
  
//   cout << "difference : " << firstScintilatorTime-secondScintilatorTime << endl;
  
  for (int totalChambers = 0 ; totalChambers < numberOfChambers ; totalChambers++){
    
    currentChamberObj = this->getChamberNumber(totalChambers+1);
     cout << "Chamber " << totalChambers+1 ;
    for (int j=0 ; j < 96 ;j++){
      
      currentChamberObj = this->getChamberNumber(totalChambers+1);
      currentChannelObj = currentChamberObj->getChannel(j+1);      
      if (currentChannelObj->hasHit()){
 	cout << " channel " << currentChannelObj->getOnlineNumber() << " time " << currentChannelObj->getHits().at(0);
      }
    }
     cout << endl;
    currentChamberObj->findAllClustersForTriggerTimeReferenceAndTimeWindow(timeReference,timeWindow);    
    // now check the clusterization methods
    // fill number of cluster when there is at least one
    if(currentChamberObj->getNumberOfClusters()){
      numberOfClustersHisto->Fill(currentChamberObj->getNumberOfClusters());
    }
  }
  
  cout << "-------------------" << endl;
//   cout << "Check new cluster methods" << endl;
  
  
  for (int totalChambers = 0 ; totalChambers < numberOfChambers ; totalChambers++){
    
    currentChamberObj = this->getChamberNumber(totalChambers+1);
//     cout << "Chamber " << totalChambers+1 ;
    for (int i = 0 ; i < currentChamberObj->getNumberOfClusters() ; i++){
      
      // make the difference to get the tolerance within one cluster 
      
      int smallestTime = 0;
      int biggestTime = 0;
      int currentValue = 0;
      int sizeOfCurrentCluster = currentChamberObj->getClusterNumber(i+1).size();
      tempCluster = currentChamberObj->getClusterNumber(i+1);
      int numberOfHits = 0 ;
      for (int j = 0 ; j < sizeOfCurrentCluster; j++){
	// Fill the size here
	SingleMultiHits->Fill(currentChamberObj->getStrip(tempCluster.at(j))->getHits().size());
	
      }
      
      clsSize->Fill(sizeOfCurrentCluster);
      
      for (int j = 0 ; j < sizeOfCurrentCluster ; j++ ){
	
	currentValue = currentChamberObj->getStripsHitsTimesForCluster(i+1).at(j);
	
	if ( j == 0 ) {
	  // init the values
	  smallestTime = currentChamberObj->getStripsHitsTimesForCluster(i+1).at(j);
	  biggestTime = currentChamberObj->getStripsHitsTimesForCluster(i+1).at(j);
	}
	else{
	  
	  if ( smallestTime >= currentValue ){
	    smallestTime = currentValue;
	  }
	  if( biggestTime <= currentValue){
	    biggestTime = currentValue;
	  } 
	}
      }
      
      if (biggestTime - smallestTime != 0){
	hist4->Fill(biggestTime-smallestTime);
      }
      int avgTimeForCluster = currentChamberObj->getAverageTimeForCluster(i+1);
      topMinusEachChamberAvg->Fill(abs(firstScintilatorTime - avgTimeForCluster));
      bottomMinusEachChamberAvg->Fill(abs(secondScintilatorTime - avgTimeForCluster));
      
//       cout << endl << "Cluster " << i+1 << " toptime " << biggestTime << " leasttime " << smallestTime ;
      
    }
    
//     cout << endl;
  }
  
//   cout << "-------Second check done-----------" << endl;
  
  // try with single cluster per chamber 
  
  vector<int> vectorOfReferenceChambers;
  
  for (int i=0; i < this->getNumberOfChambers() ; i ++){
    currentChamberObj = this->getChamberNumber(i+1);
    if (currentChamberObj->isReferenceChamber() && !currentChamberObj->getNumberOfClusters()){
      // the reference chamber does not have a hit (cluster), probably inefficient (or else) , skip the execution
      
      break;
    }
  }
  
  // TODO // remove the following when the configuration object is introduced
  vectorOfReferenceChambers.push_back(1); vectorOfReferenceChambers.push_back(4); vectorOfReferenceChambers.push_back(6);
  
  /** Determination of track starts here */ // Move this part in separated method 
  
  //TFile * goodTracks = new TFile("GoodTracks.root","UPDATE");
  //TFile * badTracks = new TFile("BadTracks.root","UPDATE");
  int globalCount = 1;
  for ( int i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
    for( int j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
      for( int k = 0 ; k < this->getChamberNumber(vectorOfReferenceChambers[2])->getNumberOfClusters() ; k++ ){
	
	// check the multiplicity. use 5 as upper limit number
	// with the test run 2202 - CERN channel 33 is noisy so there is a condition only for it here TODO - remove the channel 33 condition
	
	/** Hits : first , use the time to distinguish useless crap - like noisy channels 
	 *  
	 * 2. Check the partition plane (YZ plane) for vertical tracks. If the track is vertical don't search for consecutiveness and don't fill the YZ histo
	 * 3. Check the partitions plane for consecutiveness - one could not expect track that passes 3 -> 1 -> 3 partitions
	 * 
	*/ 
	
	if(this->getChamberNumber(vectorOfReferenceChambers[0])->getSizeOfCluster(i+1) > 5 ||
	  this->getChamberNumber(vectorOfReferenceChambers[1])->getSizeOfCluster(j+1) > 5 ||
	  this->getChamberNumber(vectorOfReferenceChambers[2])->getSizeOfCluster(k+1) > 5 ||
	  this->getChamberNumber(1)->getClusterNumber(i+1).at(0) == 33) continue;
	
	// the partition logic start  here - track could pass more than one partition
	int direction = 0 ; // direction should describe how the partition changes from one reference chamber to another. It 
	int RefChamberClusterPartition[3] ;
	int currentDifference = 0;
	bool positive = false;
	bool negative = false;
	int partitionPenetrated = 1;
	// the Y coordinate is the partition number ( 1 2 or 3 - A B or C)
	
	RefChamberClusterPartition[0] = this->getChamberNumber(vectorOfReferenceChambers[0])->getXYCoordinatesOfCluster(i+1).at(1);
	RefChamberClusterPartition[1] = this->getChamberNumber(vectorOfReferenceChambers[1])->getXYCoordinatesOfCluster(j+1).at(1);
	RefChamberClusterPartition[2] = this->getChamberNumber(vectorOfReferenceChambers[2])->getXYCoordinatesOfCluster(k+1).at(1);
	
	for ( int ii = 0; ii < 2 ; ii++ ){
	  direction = (RefChamberClusterPartition[ii] - RefChamberClusterPartition[ii+1]);
	  if (direction != 0) { 
	    direction = direction/abs(direction); 
	    partitionPenetrated++;
	  } // get only the sign ( +1 or -1)
	  if (direction && direction == -1)  positive = true;
	  if (direction && direction == 1 )  negative = true;
	  
	}
	
	// cannot have a track that goes in both direction
	// partition logic end here
	stringstream ss;
	ss << globalCount;
	string histoCounter = ss.str();
	
	
	TH2F * histXZ = new TH2F(histoCounter.c_str(),"XZ plane",110,0,110,68,0,34);
	
	
	histXZ->SetMarkerColor(kBlue);
	histXZ->SetMarkerStyle(kOpenTriangleDown);
	
	double * xc = new double[3];
	double * yc = new double[3];
	double * zc = new double[3];
	
	vector<double> coordinates ;
	double xCoordinate = 0;
	int yCoordinate = 0;
	int zCoorinate = 0;
	
	coordinates = this->getChamberNumber(1)->getXYCoordinatesOfCluster(i+1);
	xCoordinate = coordinates.at(0);
	yCoordinate = coordinates.at(1);
	zCoorinate = 10*vectorOfReferenceChambers[0];
	
	int prevPartition = yCoordinate;
	xc[0] = xCoordinate;
	yc[0] = yCoordinate;
	zc[0] = 1*10;
	
	histXZ->Fill(zc[0],xCoordinate);
	cout << xCoordinate << " " << yCoordinate << endl;
	
	coordinates = this->getChamberNumber(4)->getXYCoordinatesOfCluster(j+1);
	xCoordinate = coordinates.at(0);
	yCoordinate = coordinates.at(1);
	
	xc[1] = xCoordinate;
	yc[1] = yCoordinate;
	zc[1] = 4*10;
	
	histXZ->Fill(zc[1],xCoordinate);
	prevPartition = yCoordinate;
	
	cout << xCoordinate << " " << yCoordinate << endl;	
	coordinates = this->getChamberNumber(6)->getXYCoordinatesOfCluster(k+1);
	xCoordinate = coordinates.at(0);
	yCoordinate = coordinates.at(1);
	
	xc[2] = xCoordinate;
	yc[2] = yCoordinate;
	zc[2] = 6*10;
	
	histXZ->Fill(zc[2],xCoordinate);
	cout << xCoordinate << " " << yCoordinate << endl;
	
	if ( positive && negative ) continue;
	
	TF1 * fitfunc = new TF1("FitTrack","[0]+x*[1]",0,100);
	Double_t * params = new Double_t[2];
	histXZ->Fit(fitfunc);
	fitfunc->GetParameters(params);
	cout << "par1 " << params[0] << " par2 " << params[1] << " chi2 " << fitfunc->GetChisquare() << endl;
	double channelToSearchHitIn ;
	
	for (int jj = 0 ; jj < this->getNumberOfChambers() ; jj++){
	  if (jj+1 != vectorOfReferenceChambers[0] || jj+1 != vectorOfReferenceChambers[1] || jj+1 != vectorOfReferenceChambers[1])
	    // add additional rule that the chamber should exist in the calibration object 
	  {
	    
	    channelToSearchHitIn = fitfunc->Eval((jj+1)*10);
	    cout << "Evaluated for chamber number " << jj+1 << " value : " << channelToSearchHitIn << endl;
	    
	  }
	  
	}
	// now here - what to return, and how to get the hits in the chambers under test from the function
	
	
	
	if (fitfunc->GetChisquare() > 20) continue; // cut the execution 
	if(fitfunc->GetChisquare() < 20){
	  //goodTracks->Write(trackHistoName.c_str());
	  histClustersPartitionDistr->Fill(partitionPenetrated);
	  //histXZ->SaveAs((trackHistoName+".root").c_str());  
	  // here search for hits in the chambers under test  
	  
	}
	
	/*
	else{
	  badTracks->Write(trackHistoName.c_str());
	}	
	*/
	
	//trackHistoName+=".root";
	//histXZ->SaveAs(trackHistoName.c_str());
	
	histXZ->Delete();
	
      }
    }
  }
//   badTracks->Close("R");
//   badTracks->Delete();
//   goodTracks->Close("R");
//   goodTracks->Delete();
  
}

map<int,vector<double> > RPCChambersCluster::getReconstructedHits(vector<int> vectorOfReferenceChambers, const int & timeWindow,const int & timeReference,bool & isVerticalTrack,const int & Chi2goodness, const ESiteFileType & fileType){
  
  map<int,vector<double> > mapOfHits; //
  // the default value for Chi2goodness is 20 
  double best_chi2goodnes_value = Chi2goodness+10 ; // this variable is used as reference so that it holds the best chi2 found for a track, so its used only a track with better chi2 to be accepted
  
  for (int i = 0 ; i < this->getNumberOfChambers() ; i++){
    this->getChamberNumber(i+1)->findAllClustersForTriggerTimeReferenceAndTimeWindow(timeReference,timeWindow);
    //cout  << "Chamber is " << i+1 << endl;
  }
  
  vector<vector<int> > vectorOfClusterNumberCombinations;  
  
  if (fileType == kIsCERNrawFile ){
  
    assert(vectorOfReferenceChambers.size() == 3);
    
    for ( int i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
      for( int j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
	for( int k = 0 ; k < this->getChamberNumber(vectorOfReferenceChambers[2])->getNumberOfClusters() ; k++ ){
	  
	  vector<int> singleCombination;
	
	  singleCombination.push_back(i+1);
	  singleCombination.push_back(j+1);
	  singleCombination.push_back(k+1);
	  
	  for (int f = 0 ; f < singleCombination.size() ; f++){	  
	    if(this->getChamberNumber(vectorOfReferenceChambers[f])->getSizeOfCluster(singleCombination.at(f)) > 5 ) continue;	  
	    // don't insert combination if there is too big cluster. 
	  }
	  
	  vectorOfClusterNumberCombinations.push_back(singleCombination);
	  
	}      
      }
    }
  }
  
  if (fileType == kIsBARCrawFile || fileType == kIsGENTrawFile ){
    // add implementation for BARC and Ghent stand . 
    
    assert(vectorOfReferenceChambers.size() == 2);
    
    for ( int i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
      for( int j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
      
	vector<int> singleCombination;
	singleCombination.push_back(i+1);
	singleCombination.push_back(j+1);
	
	for (int f = 0 ; f < singleCombination.size() ; f++){
	  if(this->getChamberNumber(vectorOfReferenceChambers[f])->getSizeOfCluster(singleCombination.at(f)) > 5 ) continue;
	  // don't insert combination if there is too big cluster. 
	}
	
	vectorOfClusterNumberCombinations.push_back(singleCombination);
      
      }
    }
  }
  
  for (int combinationsVectorElement = 0 ; combinationsVectorElement < vectorOfClusterNumberCombinations.size() ; combinationsVectorElement ++){
    
    // the partition logic start  here - track could pass more than one partition
    
    int direction = 0 ; // direction should describe how the partition changes from one reference chamber to another. It 
    vector<int> RefChamberClusterPartition;
    bool positive = false;
    bool negative = false;
    int partitionPenetrated = 1;
    
    // the Y coordinate is the partition number (1 2 or 3 - A B or C)
    
    vector<int> clusterNum = vectorOfClusterNumberCombinations.at(combinationsVectorElement);
    
    for (int ii = 0; ii < clusterNum.size() ; ii++){
      RefChamberClusterPartition.push_back(this->getChamberNumber(vectorOfReferenceChambers[ii])->getXYCoordinatesOfCluster(clusterNum.at(ii)).at(1));
    }
    
    isVerticalTrack = true;
    
    for ( int ii = 0; ii < RefChamberClusterPartition.size() - 1 ; ii++ ){
      direction = (RefChamberClusterPartition.at(ii) - RefChamberClusterPartition.at(ii+1));
      if (direction != 0) { 
	direction = direction/abs(direction); 
	partitionPenetrated++;
      } // get only the sign ( +1 or -1)
      if (direction && direction == -1)  { positive = true; isVerticalTrack = false; }
      if (direction && direction == 1 )  { negative = true; isVerticalTrack = false; }
    }
    
    if ( positive && negative ) continue;
    // cannot have a track that goes in both direction
    
    TH1F * histXZ = new TH1F("fitHistogram","XZ plane",110,0,11);
    histXZ->GetYaxis()->SetRangeUser(0,34);
    histXZ->SetMarkerColor(kBlue);
    histXZ->SetMarkerStyle(kCircle);
    histXZ->GetXaxis()->SetTitle("Shelf number");
    histXZ->GetYaxis()->SetTitle("Channel number");
    TF1 * fitfunc = new TF1("FitTrack","[0]+x*[1]",0,11);
    
    vector<double> coordinates;
    double xCoordinate = 0;
    int yCoordinate = 0;
    int zCoorinate = 0;
    
    for (int ii=0 ; ii < vectorOfReferenceChambers.size() ; ii++){
	  
      coordinates = this->getChamberNumber(vectorOfReferenceChambers[ii])->getXYCoordinatesOfCluster(clusterNum[ii]);
      xCoordinate = coordinates.at(0);
      yCoordinate = coordinates.at(1);
      zCoorinate = 10*vectorOfReferenceChambers[ii];
      Double_t errorValue = this->getChamberNumber(vectorOfReferenceChambers[ii])->getSizeOfCluster(clusterNum[ii]);
      histXZ->SetBinContent(zCoorinate,xCoordinate);  
      histXZ->SetBinError(zCoorinate,errorValue/2);
      //cout << xCoordinate << " " << yCoordinate << endl;
      
    }
    
    Double_t params[2];
    histXZ->Fit(fitfunc,"RQ");
    fitfunc->GetParameters(params);
    
    //cout << "par1 " << params[0] << " par2 " << params[1] << " chi2 " << fitfunc->GetChisquare() << endl;
    // The resudials - difference between estimated fit value and the middle of the nearest cluster
    
    int prevReference = 0 , nextReference = 0 , prevReferencePartition = 0 , nextReferencePartition = 0; 
    bool currentChamberIsReference = false;
    int startCounter = 0, endCounter = 0;
    
    if ( fitfunc->GetChisquare() < Chi2goodness && fitfunc->GetChisquare() < best_chi2goodnes_value ) {
      best_chi2goodnes_value = fitfunc->GetChisquare(); // the next loop this value would be evaluated
      // in case of only one partition, get the partition number of the first reference point
      
      int referenceChambersIncrementor = 0;
      bool negativeChannelNumberIsFound = false;
      
      for ( int currentChNumber = 0 ; currentChNumber < this->getNumberOfChambers() ; currentChNumber++ ) {
	// check where the chamber is according to the reference chambers
	vector<double> vectorOfpartitionsAndHit;
	double channelNum = fitfunc->Eval(currentChNumber+1);
	
	if(channelNum < 0 || channelNum > 96/this->getChamberNumber(1)->getClones()){
	  // clear the map and break the loop, it would continue from the next cluster combination
	  mapOfHits.clear();
	  break;
	}
	
	/** four cases 1. the chamber is before the first reference 2. the chamber is after the last reference 3. the chamber is between two references 4. the chamber is a reference */
	
	for(int refCheck = 0 ; refCheck < vectorOfReferenceChambers.size(); refCheck++){
	  // find the surounding references
	  if (currentChNumber+1 == vectorOfReferenceChambers.at(refCheck)){
	    currentChamberIsReference = true;
	    break;
	  }	      
	  if ( vectorOfReferenceChambers.at(refCheck) > currentChNumber+1 && refCheck == 0 ){
	    // its before the first reference chamber
	    nextReference = vectorOfReferenceChambers.at(refCheck);
	    nextReferencePartition = this->getChamberNumber(nextReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	    break;
	    
	  }
	  if ( vectorOfReferenceChambers.at(refCheck) < currentChNumber+1 && refCheck == vectorOfReferenceChambers.size() - 1 ){
	    // its after the last chamber
	    prevReference = vectorOfReferenceChambers.at(refCheck);
	    prevReferencePartition = this->getChamberNumber(prevReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	    break;    
	  }
	  if ( vectorOfReferenceChambers.at(refCheck) < currentChNumber+1 && vectorOfReferenceChambers.at(refCheck+1) > currentChNumber+1 ){
	    // its between two references
	    prevReference = vectorOfReferenceChambers.at(refCheck) ;
	    nextReference = vectorOfReferenceChambers.at(refCheck+1);
	    prevReferencePartition = this->getChamberNumber(prevReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	    nextReferencePartition = this->getChamberNumber(nextReference)->getXYCoordinatesOfCluster(clusterNum[refCheck+1]).at(1);
	    break;    
	  }
	}
	
	if(!currentChamberIsReference){
	  
	  if (nextReference && prevReference == 0){
	    if (positive){
	      prevReferencePartition = 1;      
	    }
	    if(negative){
	      prevReferencePartition = this->getChamberNumber(1)->getClones();      
	    }    
	  }
	  
	  if (prevReferencePartition && nextReferencePartition == 0){
	    if (positive){
	      nextReferencePartition = this->getChamberNumber(1)->getClones();      
	    }
	    if(negative){
	      nextReferencePartition = 1;      
	    }    
	  }
	  
	  if (partitionPenetrated == 1){
	    prevReferencePartition = yCoordinate;
	    nextReferencePartition = yCoordinate;
	  }
	  
	  if (positive){ startCounter = prevReferencePartition; endCounter = nextReferencePartition; }
	  else { startCounter = nextReferencePartition ; endCounter = prevReferencePartition ; }
	  
	  for (int currentCounter = startCounter ; currentCounter <= endCounter; currentCounter ++ ){
	    assert(currentCounter > 0 && currentCounter < 4);
	    vectorOfpartitionsAndHit.push_back(currentCounter);    
	  }        
	}
	
	else{
	  vectorOfpartitionsAndHit.push_back(this->getChamberNumber(currentChNumber+1)->getXYCoordinatesOfCluster(clusterNum[referenceChambersIncrementor]).at(1));
	  referenceChambersIncrementor ++;
	}
	
	prevReference = 0 ; nextReference = 0 ; prevReferencePartition = 0 ; nextReferencePartition = 0; currentChamberIsReference = false;
	//cout << "Chamber " << l+1 << " " <<  coordinates.at(1) << " " << fitfunc->Eval(l+1) << " " << endl;
	
	int channelNumberToStore = channelNum;
	if (channelNumberToStore < 96/this->getChamberNumber(1)->getClones()){
	  channelNumberToStore += 1;
	} // add one to represent the fired channel, or none if the channel is on the right border

	vectorOfpartitionsAndHit.push_back(channelNumberToStore); // the last element is the number of the channel
	
	/** Debug lines
	cout << "Chamber is " << currentChNumber+1 << " " ;
	for (int thesize = 0 ; thesize < vectorOfpartitionsAndHit.size() ; thesize++){
	  cout << vectorOfpartitionsAndHit.at(thesize) << " " ;
	}
	cout << endl;
	*/
	
	mapOfHits[currentChNumber+1] = vectorOfpartitionsAndHit;
	
      }      
    }
    
    //histXZ->SaveAs("reconstructed_track.root"); // if one wants to see the track histogram
    fitfunc->Delete();
    histXZ->Delete();
    
  }
  
  return mapOfHits;
}

vector<vector<int> > RPCChambersCluster::getPartitionsVectorForVectorOfReferenceChambers(const int & chamberNumber,const vector<int> & vectorOfReferenceChambers,const vector<int> & vectorOfClusterNumbersCombination){
  
  int prevReference = 0 , nextReference = 0 , prevReferencePartition = 0 , nextReferencePartition = 0; 
  bool currentChamberIsReference = false;
  int startCounter = 0, endCounter = 0;
  
  int positive = 0, negative = 0;
  int yCoordinate = 0, referenceChambersIncrementor = 0 ;
  vector<int> clusterNum = vectorOfClusterNumbersCombination;
  
  vector<vector<int> > vectorOfPartitions;
  
  for ( int currentChNumber = 0 ; currentChNumber < this->getNumberOfChambers() ; currentChNumber++ ) {
    // check where the chamber is according to the reference chambers
    vector<double> vectorOfpartitionsAndHit;
    
    for(int refCheck = 0 ; refCheck < vectorOfReferenceChambers.size(); refCheck++){
      // find the surounding references
      yCoordinate = vectorOfReferenceChambers.at(0); // used when the track is vertical to assign this partition to all chambers as reference
      
      if (currentChNumber+1 == vectorOfReferenceChambers.at(refCheck)){
	currentChamberIsReference = true;
	break;
      }
      
      if ( vectorOfReferenceChambers.at(refCheck) > currentChNumber+1 && refCheck == 0 ){
	// its before the first reference chamber
	nextReference = vectorOfReferenceChambers.at(refCheck);
	nextReferencePartition = this->getChamberNumber(nextReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	break;
      }
	      
      if ( vectorOfReferenceChambers.at(refCheck) < currentChNumber+1 && refCheck == vectorOfReferenceChambers.size() - 1 ){
	// its after the last chamber
	prevReference = vectorOfReferenceChambers.at(refCheck);
	prevReferencePartition = this->getChamberNumber(prevReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	break;
      }
      
      if ( vectorOfReferenceChambers.at(refCheck) < currentChNumber+1 && vectorOfReferenceChambers.at(refCheck+1) > currentChNumber+1 ){
	// its between two references
	prevReference = vectorOfReferenceChambers.at(refCheck) ;
	nextReference = vectorOfReferenceChambers.at(refCheck+1);
	prevReferencePartition = this->getChamberNumber(prevReference)->getXYCoordinatesOfCluster(clusterNum[refCheck]).at(1);
	nextReferencePartition = this->getChamberNumber(nextReference)->getXYCoordinatesOfCluster(clusterNum[refCheck+1]).at(1);
	break;
      }      
    }
    
    if(!currentChamberIsReference){
      if(!positive && !negative){
	  prevReferencePartition = yCoordinate ;
	  nextReferencePartition = yCoordinate ;
      }
      
      if (nextReference && prevReference == 0){
	if (positive){
	  prevReferencePartition = 1;
	}
	if(negative){
	  prevReferencePartition = this->getChamberNumber(1)->getClones();
	}
      }
      
      if (prevReferencePartition && nextReferencePartition == 0){
	if (positive){
	  nextReferencePartition = this->getChamberNumber(1)->getClones();
	}
	if(negative){
	  nextReferencePartition = 1;
	}
      }
	      
      if (positive){ startCounter = prevReferencePartition; endCounter = nextReferencePartition; }
      else { startCounter = nextReferencePartition ; endCounter = prevReferencePartition ; }
      
      for (int currentCounter = startCounter ; currentCounter <= endCounter; currentCounter ++ ){
	if (currentCounter == 0 || currentCounter == 4){
	  cout << "Problem with partition calculation" << endl;
	}
	vectorOfpartitionsAndHit.push_back(currentCounter);
	
      }      
    }
	    // end of 	    
    else{
      vectorOfpartitionsAndHit.push_back(this->getChamberNumber(currentChNumber+1)->getXYCoordinatesOfCluster(clusterNum[referenceChambersIncrementor]).at(1));
      referenceChambersIncrementor += 1;
    }    
  }  
}

int RPCChambersCluster::getTimeReferenceValueForSiteType(ESiteFileType fileType){
  
  // switch the possible cases
  // the values returned should be based on study of the trigger (scintilator) channels 
  // the time reference is your trigger event time. within one event this is te coordinate to follow
  
  assert(this->getNumberOfTriggerObjects() > 0); // it is expected we have set the chambers and triggers
  RPCChamber * triggerObj = this->getTriggerObjectNumber(1);
  int timeReference = 0;
  int coincidence_time = 0 ;
  int difference_reference = 0 , firstScintilatorTime = 0 , secondScintilatorTime = 0;
  
  switch (fileType)
  {
    case kIsCERNrawFile:
    
      
      assert(triggerObj->getChannel(32)->hasHit());
      coincidence_time = triggerObj->getChannel(32)->getHits().at(0);
      
      for (int i = 0 ; i < 13 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( difference_reference == 0 || ( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      difference_reference = 0;
      
      for (int i = 13 ; i < 31 ; i++) {
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( difference_reference == 0 || ( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    secondScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      timeReference = (firstScintilatorTime - secondScintilatorTime)/2;
      timeReference = coincidence_time;
      
    break;
    
    //timeReference = timeReference/2;
    
    case kIsGENTrawFile:
    
      for(int i = 0 ; i < 16 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit()) {
	  firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	  break;
	}
      }
      for (int i = 16 ; i <32; i++){
	if(triggerObj->getChannel(i+1)->hasHit()) {
	  secondScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	}
      }
      
      timeReference = (firstScintilatorTime + secondScintilatorTime) / 2;
      //cout << "first scint time " << firstScintilatorTime << " secondScintilatorTime " << secondScintilatorTime << endl;
      //cout << " ghent reference time " << timeReference << endl;
      break;
    
    case kIsBARCrawFile:
    
      timeReference = 0;
      break;
    
  }
  
  return timeReference;
  
}

int RPCChambersCluster::getTimeWindowForSiteType(ESiteFileType siteType){
  
  // make the study with plots what should be this window width for each site 
  
  int timeWindow = 0 ;
  
  switch(siteType){
    case kIsCERNrawFile:
    
      timeWindow = 1000;
    break;
    case kIsGENTrawFile:
    
      timeWindow = 50;
    break;
    case kIsBARCrawFile:
    
      timeWindow = 0;
    break;
  }
  
  return timeWindow;
}

void RPCChambersCluster::configureChambersWithConfigObject(RPCRunConfig * runConfigObject){
  for (int i = 0 ; i < this->getNumberOfChambers() ; i++){
    this->getChamberNumber(i+1)->setCurrentRunDetails(runConfigObject->getBasicConditionsForChamber(i+1));
  }
}

