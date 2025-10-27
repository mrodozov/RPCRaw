/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCChambersCluster.h"
#include <boost/lexical_cast.hpp>

using namespace std;


void RPCChambersCluster::createNewClusterOfChambersWithRE4type(const int & nChambers,const string & RE4type){
  if (this->_numberOfChambers){ // not ok, some chambers are already created , one has first to erase them
    cout << "The object still holds existing chamber objects ! use deleteAllChambers method to clear them first." << endl;
  }
  else { // its ok to proceed
    //this->_chambersPointer = new RPCChamber[nChambers];
    for (unsigned i = 0 ; i < nChambers; i++){
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
    for(unsigned i = 0 ; i < nTriggerObjects ; i++){
      RPCChamber * aTriggerObject = new RPCChamber(kRPC_RE_4_2_chamber); // defined in raw types file, take a look
      aTriggerObject->allocAndInit();
      aTriggerObject->setIsTriggerChamber(true);
      this->_triggerObjectsPointer[i] = aTriggerObject;
    }
    this->_numberOfTriggers = nTriggerObjects;
  }
}

void RPCChambersCluster::setDataSourceForNchambers(const int & nChambers,const vector<vector<vector< unsigned> > > & dataSource){
  
  for (unsigned i = 0 ; i < nChambers ; i ++){
    this->getChamberNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }
}

void RPCChambersCluster::setDataSourceForNtriggerObjects(const int & nTriggerObject,const vector<vector<vector< unsigned> > > & dataSource){
  for (unsigned i = 0 ; i < nTriggerObject ; i++ ){
    this->getTriggerObjectNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }
}

RPCChambersCluster::RPCChambersCluster (){
  this->_numberOfChambers = 0;
  this->_numberOfTriggers = 0;
}

RPCChambersCluster::RPCChambersCluster (const int & numberOfChambers, const int & numberOfTriggerObjs, const string & RE4TypeOfChambers){
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
    for (unsigned i=0; i < this->getNumberOfChambers(); i++) {
      delete this->_chambersPointer.at(i);
    }
  }
  this->_chambersPointer.clear();
  this->_numberOfChambers = 0;
}

void RPCChambersCluster::deleteAllTriggerObjects(){
  if (this->getNumberOfTriggerObjects() > 0){
    RPCChamber * pointer;
    for (unsigned i=0; i < this->getNumberOfTriggerObjects(); i++){
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
      
      unsigned countTwoHits=0;
      for (unsigned i = 0 ; i < 13 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( (( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) ||
	       (difference_reference == 0) ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      difference_reference = 0;
      
      for (unsigned i = 13 ; i < 31 ; i++) {
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( (( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) ||
	       ( difference_reference == 0)) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    secondScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      //timeReference = timeReference/2;
      /*
    case kIsGENTrawFile:
      timeWindow = 20;
      for (unsigned i = 0 ; i < 32 ; i++){
	if (triggerObj->getChannel(i+1)->hasHit()) {
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
  for (unsigned i=0; i < 32 ; i++){
    if (triggerObj->getChannel(i+1)->hasHit()){
       //cout << " trig channel : " << i+1 << " " << triggerObj->getChannel(i+1)->getHits().at(0) << " ";
    }
  }
  
  cout << endl;
  //cout << " most close trigger entries : top " << firstScintilatorTime << " bottom : " << secondScintilatorTime;
  //cout << endl;
  hist1->Fill(abs(firstScintilatorTime-secondScintilatorTime));
  hist2->Fill(coincidence_time - firstScintilatorTime);
  hist3->Fill(coincidence_time - secondScintilatorTime);
  //cout << "difference : " << firstScintilatorTime-secondScintilatorTime << endl;
  
  for (unsigned totalChambers = 0 ; totalChambers < numberOfChambers ; totalChambers++){
    
    currentChamberObj = this->getChamberNumber(totalChambers+1);
    cout << "Chamber " << totalChambers+1 ;
    for (unsigned j=0 ; j < 96 ; j++){
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
  //cout << "Check new cluster methods" << endl;
  
  
  for (unsigned totalChambers = 0 ; totalChambers < numberOfChambers ; totalChambers++){
    
    currentChamberObj = this->getChamberNumber(totalChambers+1);
    // cout << "Chamber " << totalChambers+1 ;
    for (unsigned i = 0 ; i < currentChamberObj->getNumberOfClusters() ; i++){
      
      // make the difference to get the tolerance within one cluster 
      
      int smallestTime = 0;
      int biggestTime = 0;
      int currentValue = 0;
      int sizeOfCurrentCluster = currentChamberObj->getClusterNumber(i+1).size();
      tempCluster = currentChamberObj->getClusterNumber(i+1);
      int numberOfHits = 0 ;
      for (unsigned j = 0 ; j < sizeOfCurrentCluster; j++){
	// Fill the size here
	SingleMultiHits->Fill(currentChamberObj->getStrip(tempCluster.at(j))->getHits().size());
	
      }
      
      clsSize->Fill(sizeOfCurrentCluster);
      
      for (unsigned j = 0 ; j < sizeOfCurrentCluster ; j++ ){
	
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
      
      //cout << endl << "Cluster " << i+1 << " toptime " << biggestTime << " leasttime " << smallestTime ;
      
    }
    
    // cout << endl;
  }
  
  //cout << "-------Second check done-----------" << endl;
  // try with single cluster per chamber 
  
  vector<int> vectorOfReferenceChambers;
  
  for (unsigned i=0; i < this->getNumberOfChambers() ; i++){
    currentChamberObj = this->getChamberNumber(i+1);
    if (currentChamberObj->isReferenceChamber() && !currentChamberObj->getNumberOfClusters()){
      // the reference chamber does not have a hit (cluster), probably inefficient (or else) , skip the execution      
      break;
    }
  }
  
  // remove the following when the configuration object is introduced // done, change the implementation
  vectorOfReferenceChambers.push_back(1); vectorOfReferenceChambers.push_back(4); vectorOfReferenceChambers.push_back(6);
  
  /** Determination of track starts here */ // Move this part in separated method 
  
  //TFile * goodTracks = new TFile("GoodTracks.root","UPDATE");
  //TFile * badTracks = new TFile("BadTracks.root","UPDATE");
  
  int globalCount = 1;
  for (unsigned i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
    for (unsigned j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
      for (unsigned k = 0 ; k < this->getChamberNumber(vectorOfReferenceChambers[2])->getNumberOfClusters() ; k++ ){
	
	// check the multiplicity. use 5 as upper limit number
	// with the test run 2202 - CERN channel 33 is noisy so there is a condition only for it here TO DO - remove the channel 33 condition // old function
	
	/** Hits : first , use the time to distinguish useless crap - like noisy channels 
	 *  
	 * 2. Check the partition plane (YZ plane) for vertical tracks. If the track is vertical don't search for consecutiveness and don't fill the YZ histo
	 * 3. Check the partitions plane for consecutiveness - one could not expect track that passes 3 -> 1 -> 3 partitions
	 * 
	*/
	
	if((this->getChamberNumber(vectorOfReferenceChambers[0])->getSizeOfCluster(i+1) > 5) ||
	   (this->getChamberNumber(vectorOfReferenceChambers[1])->getSizeOfCluster(j+1) > 5) ||
	   (this->getChamberNumber(vectorOfReferenceChambers[2])->getSizeOfCluster(k+1) > 5)
	  ) continue;
	
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
	
	for (unsigned ii = 0; ii < 2 ; ii++ ){
	  direction = (RefChamberClusterPartition[ii] - RefChamberClusterPartition[ii+1]);
	  if (direction) {
	    direction = direction/abs(direction); 
	    partitionPenetrated++;
	    if (direction == -1)
	      positive = true;
	    else
	      negative = true;	    
	  } // get only the sign ( +1 or -1)
	}
	
	// cannot have a track that goes in both direction
	// partition logic end here
	stringstream ss;
	ss << globalCount;
	string histoCounter = ss.str();	
	
	TH2F * histXZ = new TH2F(histoCounter.c_str(),"XZ plane",110,0,110,68,0,34);	
	
	histXZ->SetMarkerColor(kBlue);
	histXZ->SetMarkerStyle(kOpenTriangleDown); //  - open triangle down not found on noise server ? 
	
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
	
	if ( positive && negative ) continue; // dafuq is this
	
	TF1 * fitfunc = new TF1("FitTrack","[0]+x*[1]",0,100);
	Double_t * params = new Double_t[2];
	histXZ->Fit(fitfunc);
	fitfunc->GetParameters(params);
	cout << "par1 " << params[0] << " par2 " << params[1] << " chi2 " << fitfunc->GetChisquare() << endl;
	double channelToSearchHitIn ;
	
	for (unsigned jj = 0 ; jj < this->getNumberOfChambers() ; jj++){
	  if (jj+1 != vectorOfReferenceChambers[0] ||
	      jj+1 != vectorOfReferenceChambers[1] ||
	      jj+1 != vectorOfReferenceChambers[1])
	      // add additional rule that the chamber should exist in the calibration object
	    // this looks like more nonsense but its probably isnt
	  {	    
	    channelToSearchHitIn = fitfunc->Eval((jj+1)*10);
	    cout << "Evaluated for chamber number " << jj+1 << " value : " << channelToSearchHitIn << endl;
	  }
	}
	// now here - what to return, and how to get the hits in the chambers under test from the function
	//if (fitfunc->GetChisquare() > 20) continue; // cut the execution 
	if(fitfunc->GetChisquare() <= 20)
	  //goodTracks->Write(trackHistoName.c_str());
	  histClustersPartitionDistr->Fill(partitionPenetrated);
	  //histXZ->SaveAs((trackHistoName+".root").c_str());  
	  // here search for hits in the chambers under test  
	else
	  continue;
	
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
   //badTracks->Close("R");
   //badTracks->Delete();
   //goodTracks->Close("R");
   //goodTracks->Delete();

}

map<int,vector<double> > RPCChambersCluster::getReconstructedHits(vector<unsigned> vectorOfReferenceChambers, const int & timeWindow,const int & timeReference,bool & isVerticalTrack,map<int,double> & scintilatorsCoordinates,const bool & keepRecoTrack,TFile * fileForRecoTracks,const int & eventNum,const double & correlationFactor, const ESiteFileType & fileType){
  
  map<int,vector<double> > mapOfHits; //
  // the default value for Chi2goodness is 20 
  //double best_chi2goodnes_value = Chi2goodness+10 ; // this variable is used as reference so that it holds the best chi2 found for a track, so its used only a track with better chi2 to be accepted
  double currentBestCorrFact = -2;
  
  int lastFitPoint = 0;
  
  for (unsigned i = 0 ; i < this->getNumberOfChambers() ; i++){
    this->getChamberNumber(i+1)->findAllClustersForTriggerTimeReferenceAndTimeWindow(timeReference,timeWindow,5);
    //cout  << "Chamber is " << i+1 << endl;
  }
  
  vector<vector<int> > vectorOfClusterNumberCombinations;
  //cartesian product - put this comment here to not search the logic in the code later
  
  if (fileType == kIsCERNrawFile ){
    
    assert(vectorOfReferenceChambers.size() == 3 );
    lastFitPoint = 9;
    
    for (unsigned i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
      for (unsigned j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
	for (unsigned k = 0 ; k < this->getChamberNumber(vectorOfReferenceChambers[vectorOfReferenceChambers.size()-1])->getNumberOfClusters() ; k++ ){
	  
	  vector<int> singleCombination;
	  
	  singleCombination.push_back(i+1);
	  singleCombination.push_back(j+1);
	  singleCombination.push_back(k+1);
	  
	  for (unsigned f = 0 ; f < singleCombination.size() ; f++){	  
	    if(this->getChamberNumber(vectorOfReferenceChambers[f])->getSizeOfCluster(singleCombination.at(f)) > 5 ) continue;	  
	    // don't insert combination if there is too big cluster. 
	  }
	  
	  vectorOfClusterNumberCombinations.push_back(singleCombination);
	  
	}
      }
    }
  }
  
  if ((fileType == kIsBARCrawFile) || (fileType == kIsGENTrawFile) ){
    // add implementation for BARC and Ghent stand . 
    lastFitPoint = 5;
    
    assert(vectorOfReferenceChambers.size() == 2);
    
    for (unsigned i = 0 ; i < this->getChamberNumber(vectorOfReferenceChambers[0])->getNumberOfClusters() ; i++ ){
      for (unsigned j = 0 ; j < this->getChamberNumber(vectorOfReferenceChambers[1])->getNumberOfClusters() ; j++ ){
      
	vector<int> singleCombination;
	singleCombination.push_back(i+1);
	singleCombination.push_back(j+1);
	
	for (unsigned f = 0 ; f < singleCombination.size() ; f++){
	  if(this->getChamberNumber(vectorOfReferenceChambers[f])->getSizeOfCluster(singleCombination.at(f)) > 5 ) continue;
	  // don't insert combination if there is too big cluster. 
	}
	vectorOfClusterNumberCombinations.push_back(singleCombination);
      
      }
    }
  }
  
  string topScintToString, botScintToString;
  
  for (unsigned combinationsVectorElement = 0 ; combinationsVectorElement < vectorOfClusterNumberCombinations.size() ; combinationsVectorElement ++){
    
    // the partition logic start  here - track could pass more than one partition
    
    int direction = 0 ; // direction should describe how the partition changes from one reference chamber to another. It 
    vector<int> RefChamberClusterPartition;
    bool positive = false;
    bool negative = false;
    int partitionPenetrated = 1;
    
    // the Y coordinate is the partition number (1 2 or 3 - A B or C)
    
    vector<int> clusterNum = vectorOfClusterNumberCombinations.at(combinationsVectorElement);
    
    for (unsigned ii = 0; ii < clusterNum.size() ; ii++){
      RefChamberClusterPartition.push_back(this->getChamberNumber(vectorOfReferenceChambers[ii])->getXYCoordinatesOfCluster(clusterNum.at(ii)).at(1));
    }
    
    isVerticalTrack = true;
    
    for (unsigned ii = 0; ii < RefChamberClusterPartition.size() - 1 ; ii++ ){
      direction = (RefChamberClusterPartition.at(ii) - RefChamberClusterPartition.at(ii+1));
      if (direction) { 
	direction = direction/abs(direction); 
	partitionPenetrated++;
      } // get only the sign ( +1 or -1)
      if (direction && direction == -1)  { positive = true; isVerticalTrack = false; }
      if (direction && direction == 1 )  { negative = true; isVerticalTrack = false; }
    }
    
    if ( positive && negative ) continue; // wth is this
    // cannot have a track that goes in both direction
    
    /*
    TH1F * histXZ = new TH1F("fitHistogram","XZ plane",110,0,11);
    histXZ->GetYaxis()->SetRangeUser(-20,52);
    histXZ->SetMarkerColor(kBlue);
    histXZ->SetMarkerStyle(kCircle);
    histXZ->GetXaxis()->SetTitle("Shelf number");
    histXZ->GetYaxis()->SetTitle("Channel number");
    */
    
    TF1 * fitfunc = new TF1("FitTrack","[0]+x*[1]",0,lastFitPoint+1);
    
    TGraphErrors * graphXZ = new TGraphErrors();
    graphXZ->GetXaxis()->SetTitle("Shelf number");
    graphXZ->GetYaxis()->SetTitle("Channel number");
    //graphXZ->SetLineColor(0);
    graphXZ->SetMarkerColor(kBlack);
    graphXZ->SetMarkerStyle(kFullCircle);
    graphXZ->SetName("fit graph");
    graphXZ->SetTitle("XZ plane");
    graphXZ->GetXaxis()->SetTitle("Muon station");
    graphXZ->GetYaxis()->SetTitle("Channel number");
    
    fitfunc->SetLineColor(kBlue);        
    
    vector<double> coordinates;
    double xCoordinate = 0;
    int yCoordinate = 0;
    int zCoorinate = 0;
    
    
    for (unsigned ii=0 ; ii < vectorOfReferenceChambers.size() ; ii++){
      
      coordinates = this->getChamberNumber(vectorOfReferenceChambers[ii])->getXYCoordinatesOfCluster(clusterNum[ii]);
      xCoordinate = coordinates.at(0);
      yCoordinate = coordinates.at(1);
      zCoorinate = 10*vectorOfReferenceChambers[ii];
      Double_t errorValue = this->getChamberNumber(vectorOfReferenceChambers[ii])->getSizeOfCluster(clusterNum[ii]);
      // histXZ->SetBinContent(zCoorinate,xCoordinate);  
      // histXZ->SetBinError(zCoorinate,errorValue/2);
      // cout << xCoordinate << " " << yCoordinate << endl;
      graphXZ->SetPoint(ii,vectorOfReferenceChambers[ii],xCoordinate);
      graphXZ->SetPointError(ii,0,errorValue/2);
    }
    
    Double_t params[2];
    graphXZ->Fit(fitfunc,"RFQ");
    fitfunc->GetParameters(params);
    
    // cout << "par1 " << params[0] << " par2 " << params[1] << " chi2 " << fitfunc->GetChisquare() << endl;
    // The resudials - difference between estimated fit value and the middle of the nearest cluster
    
    int prevReference = 0 , nextReference = 0 , prevReferencePartition = 0 , nextReferencePartition = 0; 
    bool currentChamberIsReference = false;
    int startCounter = 0, endCounter = 0;
    
    if ( abs(graphXZ->GetCorrelationFactor()) >= correlationFactor && abs(graphXZ->GetCorrelationFactor()) > currentBestCorrFact ) {
      // in case of only one partition, get the partition number of the first reference point
      currentBestCorrFact = abs(graphXZ->GetCorrelationFactor());
      
      int referenceChambersIncrementor = 0;
      bool negativeChannelNumberIsFound = false;
      
      // ---------
            
      for (unsigned currentChNumber = 0 ; currentChNumber < this->getNumberOfChambers() ; currentChNumber++ ) {
	// check where the chamber is according to the reference chambers
	vector<double> vectorOfpartitionsAndHit;
	double channelNum = fitfunc->Eval(currentChNumber+1);

	/** four cases 1. the chamber is before the first reference 2. the chamber is after the last reference 3. the chamber is between two references 4. the chamber is a reference */
	
	for (unsigned refCheck = 0 ; refCheck < vectorOfReferenceChambers.size(); refCheck++){
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
	
	// end of partition possibilities
	
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
	  
	  if (partitionPenetrated == 1 ){
	    prevReferencePartition = yCoordinate;
	    nextReferencePartition = yCoordinate;    
	    
	    int firstRef = vectorOfReferenceChambers.at(0);
	    int lastRef = vectorOfReferenceChambers.at(vectorOfReferenceChambers.size() - 1);
	    int ccham =  currentChNumber+1;
	    
	    if(! (lastRef > ccham && firstRef < ccham) ){
	      // all partitions are possible, chambers are out of the reference scope
	      prevReferencePartition = this->getChamberNumber(1)->getClones(); 
	      nextReferencePartition = 1; // 3 in case of ecap chamber      
	    }
	  }
	  
	  if (positive){ startCounter = prevReferencePartition; endCounter = nextReferencePartition; }
	  else { startCounter = nextReferencePartition ; endCounter = prevReferencePartition ; }
	  
	  for (unsigned currentCounter = startCounter ; currentCounter <= endCounter; currentCounter ++ ){
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
	
	// Debug lines
	/**
	cout << "Chamber is " << currentChNumber+1 << " partitions " ;
	for (unsigned thesize = 0 ; thesize < vectorOfpartitionsAndHit.size() - 1; thesize++){
	  cout << vectorOfpartitionsAndHit.at(thesize) << " " ;
	}
	
	cout << "channel " << vectorOfpartitionsAndHit.at(vectorOfpartitionsAndHit.size()-1) << endl;
	*/
	
	mapOfHits[currentChNumber+1] = vectorOfpartitionsAndHit;
	
      }
      
      // ---------- scintilators coordinates estimate
      
      for (unsigned scintNum = 0 ; scintNum < 31 ; scintNum++){
	if(this->getTriggerObjectNumber(1)->getChannel(scintNum+1)->hasHit() && vectorOfClusterNumberCombinations.size() == 1 ) {
	  if (scintNum < 10) { scintilatorsCoordinates[scintNum+1] = graphXZ->Eval(0); topScintToString = boost::lexical_cast<string>(scintNum+1); }
	  else { scintilatorsCoordinates[scintNum+1] = graphXZ->Eval(lastFitPoint+1); botScintToString = boost::lexical_cast<string>(scintNum+1); }
	}
      }
    }
    
    // get only vertical tracks from the A partition if there are only two scint hits
    if (keepRecoTrack && isVerticalTrack && !mapOfHits.empty() && scintilatorsCoordinates.size() == 2){
      
      graphXZ->SetName(boost::lexical_cast<string>(eventNum).c_str());
      string partition;
      if (mapOfHits.find(vectorOfReferenceChambers.at(0))->second.at(0) == 1) partition = "A";
      else if (mapOfHits.find(vectorOfReferenceChambers.at(0))->second.at(0) == 2) partition = "B";
      else partition = "C";
      
      graphXZ->SetTitle(("Correlation factor is "+boost::lexical_cast<string>(graphXZ->GetCorrelationFactor()) + " trigger channels top: " + topScintToString + " bottom: " + botScintToString ).c_str());
      if(abs(graphXZ->GetCorrelationFactor()) >= correlationFactor) {
	
	string scintCombination="_"+topScintToString+"_"+botScintToString+"_"+partition;
	TDirectory * dir = fileForRecoTracks->GetDirectory(scintCombination.c_str(),true);
	
	if(!dir) {  
	  //fileForRecoTracks->ls();
	  fileForRecoTracks->mkdir(scintCombination.c_str()) ;
	  fileForRecoTracks->cd("");
	}
	
	fileForRecoTracks->cd(scintCombination.c_str());
	//cout << fileForRecoTracks->GetPath() << endl;
      }
      else{ fileForRecoTracks->cd("") ; fileForRecoTracks->cd("badTracks") ; }
      
      graphXZ->Write(graphXZ->GetName(),TObject::kOverwrite);
      fileForRecoTracks->cd("");
      //fileForRecoTracks->Write(graphXZ->GetName(),TObject::kOverwrite);
    }
    
    fitfunc->Delete();
    //histXZ->Delete();
    graphXZ->Delete();
    
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
  
  for (unsigned currentChNumber = 0 ; currentChNumber < this->getNumberOfChambers() ; currentChNumber++ ) {
    // check where the chamber is according to the reference chambers
    vector<double> vectorOfpartitionsAndHit;
    
    for (unsigned refCheck = 0 ; refCheck < vectorOfReferenceChambers.size(); refCheck++){
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
      if(!positive && !negative){// should check if this is 'vertical'
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
      
      for (unsigned currentCounter = startCounter ; currentCounter <= endCounter; currentCounter++ ){
	if ((currentCounter == 0) || (currentCounter == 4)){
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
  
  // TODO - get the coordinates of the scintilators and exclude the events with two top or two bottom scintilator hits that are impossible in principle
  
  assert(this->getNumberOfTriggerObjects() > 0); // it is expected we have set the chambers and triggers
  RPCChamber * triggerObj = this->getTriggerObjectNumber(1);
  int timeReference = 0;
  int coincidence_time = 0 ;
  int difference_reference = 0 , firstScintilatorTime = 0 , secondScintilatorTime = 0;
  
  switch (fileType)
  {
    case kIsCERNrawFile:
      
      // scintilator occupancy show two groups by 10 channels - from 1 to 10 and from 17 to 27, probably top and bottom 
      
      assert(triggerObj->getChannel(32)->hasHit());
      coincidence_time = triggerObj->getChannel(32)->getHits().at(0);
      
      for (unsigned i = 0 ; i < 10 ; i++){
	if(triggerObj->getChannel(i+1)->hasHit() ){
	  
	  if ( difference_reference == 0 || ( coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0) ) < difference_reference ) {
	    
	    difference_reference = coincidence_time - triggerObj->getChannel(i+1)->getHits().at(0);
	    firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	    
	  }
	}
      }
      
      difference_reference = 0;
      
      for (unsigned i = 16 ; i < 26 ; i++) {
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
    
      for (unsigned i = 0 ; i < 16 ; i++){
	if (triggerObj->getChannel(i+1)->hasHit()) {
	  firstScintilatorTime = triggerObj->getChannel(i+1)->getHits().at(0);
	  break;
	}
      }
      for (unsigned i = 16 ; i <32; i++){
	if (triggerObj->getChannel(i+1)->hasHit()) {
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

void RPCChambersCluster::configureChambersWithConfigObject(RPCAbstractRunConfig * runConfigObject){
  for (unsigned i = 0 ; i < this->getNumberOfChambers() ; i++){
    this->getChamberNumber(i+1)->setCurrentRunDetails(dynamic_cast<RPCRunConfig*>(runConfigObject)->getBasicConditionsForChamber(i+1));
  }
}

bool RPCChambersCluster::isShowerEvent(){
  int sumOfScintilatorsWithHitTop = 0;
  int sumOfScintilatorsWithHitBot = 0;
  int retval = false;
  for (unsigned i = 0 ; i < 15 ; i++){
    if ( this->getTriggerObjectNumber(1)->getChannel(i+1)->hasHit() ) sumOfScintilatorsWithHitTop ++;
    if ( this->getTriggerObjectNumber(1)->getChannel(i+1+16)->hasHit() ) sumOfScintilatorsWithHitBot ++;
  }
  
  if ( (sumOfScintilatorsWithHitTop > 3) || (sumOfScintilatorsWithHitBot > 3)) retval = true;
  
  return retval;
}


void RPCChambersCluster::getScintilatorsCoordinates(){
  
  // use the trigger objects, their numbers
  RPCLinkBoard * topScintilators = this->getTriggerObjectNumber(1);
  RPCLinkBoard * bottomScintilators = this->getTriggerObjectNumber(2);
  
  
}

