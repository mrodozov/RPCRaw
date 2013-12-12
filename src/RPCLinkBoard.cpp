/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoard.h"
#include <assert.h>

using namespace std;

/** ihneritance methods goes here to make the objects of this class works fine */

// strips methods

void RPCLinkBoard::allocStrips(){
  // this is the factory method used only once after the object is constructed. 
  this->strips = new RPCLinkBoardChannel[96];
  
}

void RPCLinkBoard::initStrips(){
  ExRoll::initStrips();
  // do additional inits
  this->_nonPartitionEfficiencyChannelAllTracks.resize(96/this->getClones());
  this->_nonPartitionEfficiencyChannelEfficientTracks.resize(96/this->getClones());
  this->clearAbsoluteChannelsCounters();
  this->setIsTriggerChamber(false);
  this->setStationNumber(-1); // which means its not set
}

RPCLinkBoardChannel * RPCLinkBoard::getStripCollection (){
  return dynamic_cast<RPCLinkBoardChannel*>(this->strips);
}

RPCLinkBoardChannel * RPCLinkBoard::getStrip(int element){
  assert (element >= 1 && element <= 96);
  return & getStripCollection()[element-1];
}

RPCLinkBoardChannel * RPCLinkBoard::getChannel(int channel){
  this->getStrip(channel);
}

// constructors and destructors

RPCLinkBoard::RPCLinkBoard () : ExRoll() {
  this->resetEfficiencyCounters(); // set counters to 0
}

RPCLinkBoard::RPCLinkBoard (const string& RollOnlineName) : ExRoll(RollOnlineName) {
  // set whats required here
  
}

RPCLinkBoard::~RPCLinkBoard (){
  
  this->_nonPartitionEfficiencyChannelAllTracks.clear();
  this->_nonPartitionEfficiencyChannelEfficientTracks.clear();
  delete [] this->getStripCollection();
  this->strips = NULL;
  //cout << " RPCLinkBoard destructor called " << endl;
   
}

// chip methods. chip objects are grouping the chamber channels (strips) into pack of 8, and also provide several common methods to the group

RPCLinkBoardChip * RPCLinkBoard::getChipByNumber(const int & ChipNumber){
  return dynamic_cast<RPCLinkBoardChip*>(this->getChipsMap().find(ChipNumber)->second);
}

void RPCLinkBoard::insertNewChipWithNumberAndID(const int & num,const int & ID){
  this->incrementNumberOfChips(); // they should be zero assigned by the constructor
  RPCChamberChip * aChip = new RPCChamberChip; // this to be changed onl when this method is overloaded by the next class if needed
  aChip->setChipNumber(num);
  aChip->setChipID(ID);
  aChip->assignStripsFromRoll(this);
  //this->Chips[getNumberOfChipsAssigned()-1] = aChip; // make this not directly - ahaaa ! not to use the pointer directly and to remain private for the class. its not directly
  this->insertNewChipToChipsMap(num,aChip);
} // insert chip


/**  place your custome methods here */


void RPCLinkBoard::setStripsHitsDataFromSource(const vector< vector<unsigned> > & vectorSource){
  this->_vectorToStripsHits = &vectorSource;
  // now set each strip vector
  for (int i = 0 ; i < vectorSource.size() ; i++){
    //cout << i+1 << endl;
    this->getStrip(i+1)->setHits(vectorSource.at(i));
  }
  //cout << "out" << endl;
  
}

const vector< vector<unsigned> > & RPCLinkBoard::getStripsHitsDataVector () const {
  return *this->_vectorToStripsHits;
}


void RPCLinkBoard::findAllClustersForTriggerTimeReferenceAndTimeWindow(int triggerTimeReference,int timeWindow,int maxClusterSize){
  
  // first clean if anything left
  if(this->getNumberOfClusters()){
    this->_clusterChannelNumbers.clear();
  }
  
  for (int i = 0; i < this->getClones() ; i ++ ){
    
    // getClones() gives the number of the so called Etha Partitions. In case of Endcap they are always 3 - A,B,C partitions
    
    for (int j = this->getFirstStripNumberOfClone(i+1) ; j <= this->getLastStripNumberOfClone(i+1) ; j++ ){
      
      // getFirst and getLast strip number of clone is returning the first and the last channel in given partition, so we could loop on the channels of a given partition
      int bottomLimit = triggerTimeReference-timeWindow;
      int topLimit = triggerTimeReference+timeWindow;
      if (
	this->getChannel(j)->hasHit() // see if the channel has at least one hit
	&& (  
	    this->getChannel(j)->getHits().at(0) >= bottomLimit &&
	    this->getChannel(j)->getHits().at(0) <= topLimit
	   ) // 
	) {
	
	vector<int> vectorOfStripNumber;
	//cout << "Channels in cluster  : ";      
	int k = j;      
	// increment a counter starting from the first for all consecutive hits
	while (k <= this->getLastStripNumberOfClone(i+1) && this->getChannel(k)->hasHit()) {
	  //cout << k << " ";
	  //vectorOfStripNumber.push_back(k);
	  k++;
	}
	if (k-j < maxClusterSize+1){
	  this->_clusterChannelNumbers.push_back(j);
	}
	
	// else do nothing, just do not allow wider clusters
	
	//cout << endl;
	
	j = k; // skip all strips in the cluster and continue from the next one
	k = 0 ; // reset the internal counter
	
      } 
    }
  }
}

vector<vector<int> > RPCLinkBoard::getStripsHitsTimes(){
  
  vector<vector<int> > retval;
  for(int i = 0 ; i < this->getNumberOfClusters() ; i++){
    vector<int> timesVector;
    int clusterSize = this->getClusterNumber(i+1).size();     
    for(int j=this->getClusterNumber(i+1).at(0); j <= this->getClusterNumber(i+1).at(clusterSize-1) ; j++){
      timesVector.push_back(this->getChannel(j)->getHits().at(0));
    }
    retval.push_back(timesVector);
  }
  
  return retval;
}


vector<int> RPCLinkBoard::getClusterNumber(int clusterNumber){
  vector<int> retval;
  int clusterStartChannel= this->_clusterChannelNumbers.at(clusterNumber-1);
  
  while( clusterStartChannel <= this->getLastStripNumberOfClone(this->getEthaPartitionForChannel(clusterStartChannel)) && this->getStrip(clusterStartChannel)->hasHit() ){
    retval.push_back(clusterStartChannel);
    clusterStartChannel++;
  }
  return retval;
}


int RPCLinkBoard::getAverageTimeForCluster(int clusterNumber){
  int retval = 0 ;
  int channel = 0;
  if(!this->getClusterNumber(clusterNumber).empty()){
    for (int i = 0; i < this->getClusterNumber(clusterNumber).size() ; i++){
      channel = this->getClusterNumber(clusterNumber).at(i);
      retval += this->getChannel(channel)->getHits().at(0);
    }
    retval = retval / this->getClusterNumber(clusterNumber).size();
  }
  
  return retval;
}

vector<double> RPCChamber::getXYCoordinatesOfCluster(const int & clusterNumber){
  
  assert(this->getNumberOfClusters() >= clusterNumber);
  vector<double> retval;
  int startStrip = this->getClusterNumber(clusterNumber).at(0);  
  double halfClusterWidth = 0;
  halfClusterWidth = this->getSizeOfCluster(clusterNumber);
  halfClusterWidth = halfClusterWidth/2;
  int yCoordinate = this->getEthaPartitionForChannel(startStrip);
  int numberOfChannelInPartition = startStrip - (96/this->getClones())*(yCoordinate-1); // calculates the channel number in the partition it belongs.  retval.push_back(numberOfChannelInPartition+halfClusterWidth-1);
  retval.push_back(numberOfChannelInPartition + halfClusterWidth - 1);
  retval.push_back(yCoordinate);
  return retval;
  
}

vector<double> RPCChamber::getXYZcoordinatesOfCluster(const int & clusterNumber){ // probably to be removed in future
  
  assert(this->getStationNumber() != -1);
  vector<double> retval;
  
  retval = this->getXYCoordinatesOfCluster(clusterNumber);
  retval.push_back(this->getStationNumber());
  return retval;
  
}

int RPCChamber::getEthaPartitionForChannel(const int & channelNumber){
  int retval=0;
  for (int i = 0 ; i < this->getClones() ; i++){
    if ((96/this->getClones())*(i+1) >= channelNumber){
      retval = i+1;
      break;
    }
  }
  
  return retval;
  
}


bool RPCChamber::isMatchingFiredChannelInPartition (const int & trackValue,const int & partition,const int & halfClusterWidth) {
  
  // the track value is the fit value from the track
  bool retval = false;
  int channelNumberOffset = ( partition -1 ) * (96/this->getClones());
  int numberOfChannelsInOnePartition = 96/this->getClones();
  int startChannel =   channelNumberOffset + trackValue - halfClusterWidth;
  int endChannel = channelNumberOffset + trackValue + halfClusterWidth;
  
  if (trackValue-halfClusterWidth < 1 ) { startChannel = channelNumberOffset+1 ;}
  if (trackValue+halfClusterWidth > numberOfChannelsInOnePartition ) { endChannel = channelNumberOffset + numberOfChannelsInOnePartition; }
  
  // check if the case exceeds the first or the last strip in a given partition. The evaluation value 
  
  for (int i = startChannel ; i <= endChannel ; i++){
    
    if (this->getChannel(i)->hasHit()){
      retval = true;
      break;
    }
  }
  
  return retval;
}

void RPCLinkBoard::incrementEfficiencyCounters(const bool & hitIsFound){
  if(hitIsFound){
    this->totalReconstructedTracksWithHit++;
  }
  this->totalReconstructedTracks++;
  
}

double RPCLinkBoard::getChamberEfficiency(){
  return (this->totalReconstructedTracksWithHit/this->totalReconstructedTracks)*100;
}

double RPCLinkBoard::getIntegratedChannelEfficiency(){
  double retval=0;
  TH1F * efficiencyHistogram = new TH1F("effhisto","Channel efficiency distribution",1000,0,150);
  for(int i = 0 ; i < 96 ; i++){
    if (this->getChannel(i+1)->getEfficiency() != -1 )  efficiencyHistogram->Fill( this->getChannel(i+1)->getEfficiency() );
  }  
  retval = efficiencyHistogram->GetMean();
  delete efficiencyHistogram;
  return retval;
}

void RPCLinkBoard::resetEfficiencyCounters(){
  this->totalReconstructedTracks=0;
  this->totalReconstructedTracksWithHit=0;
}

void RPCLinkBoard::incrementAbsoluteChannelCounters(const bool& hitIsFound, const int& absoluteChannelNumber){
  if(hitIsFound){
    this->_nonPartitionEfficiencyChannelEfficientTracks[absoluteChannelNumber-1] += 1;
  }
  this->_nonPartitionEfficiencyChannelAllTracks[absoluteChannelNumber-1] += 1;
}

void RPCLinkBoard::clearAbsoluteChannelsCounters(){
  for (int i = 0 ; i < this->_nonPartitionEfficiencyChannelAllTracks.size();i++){
    this->_nonPartitionEfficiencyChannelAllTracks[i] = 0;
    this->_nonPartitionEfficiencyChannelEfficientTracks[0] = 0;
  }
}


TH1F * RPCLinkBoard::getHistogramOfChannelsEfficiency(const string & histoObjName){
  
  TH1F * histogram = new TH1F (histoObjName.c_str(),"Channels efficiency",96,1,96);
  histogram->GetXaxis()->SetTitle("Channel number");
  histogram->GetYaxis()->SetTitle("Efficiency %");
  
  for (int i = 0 ; i < 96 ; i++){
    histogram->SetBinContent(i+1,this->getChannel(i+1)->getEfficiency());
  }
  return histogram;
}

TH1F * RPCLinkBoard::getDistributionOfChannelsEfficiency(const string & histoObjName){
  
  TH1F * histo = new TH1F (histoObjName.c_str(),"Channels efficiency distribution",1100,0,110);
  histo->GetXaxis()->SetTitle("Efficiency %");
  histo->GetYaxis()->SetTitle("Entries");
  
  for (int i = 0 ; i < 96 ; i++){
    if (this->getChannel(i+1)->getEfficiency() != -1 ){
      histo->Fill(this->getChannel(i+1)->getEfficiency());
    }
  }
  return histo;
}

TH1F * RPCLinkBoard::getHistogramOfTracksVsChannels(const string & histoObjName){
  TH1F * histo = new TH1F (histoObjName.c_str(),"Tracks in each channel",96,1,96);
  histo->GetXaxis()->SetTitle("Channel number");
  histo->GetYaxis()->SetTitle("Number of tracks");
  
  for (int i = 0 ; i < 96 ; i++){
    histo->SetBinContent(i+1,this->getChannel(i+1)->getAllTracks());
  }
  return histo;
}


int RPCLinkBoard::getSizeOfCluster(int clusterNumber){ return this->getClusterNumber(clusterNumber).size();}

vector<int> RPCLinkBoard::getStripsHitsTimesForCluster(int clusterNumber){ return this->getStripsHitsTimes().at(clusterNumber-1);} 

const vector<int> & RPCLinkBoard::getClusters () { return this->_clusterChannelNumbers ;}

int RPCLinkBoard::getNumberOfClusters() { return this->_clusterChannelNumbers.size(); }

void RPCLinkBoard::resetChannelsEfficiencyCounters() { for (int i=0; i < 96 ; i++) this->getChannel(i+1)->resetEfficiencyCounters(); }

void RPCLinkBoard::fillResidualValue(const double & residualValue) { this->residuals.push_back(residualValue); }

void RPCLinkBoard::resetResiduals (){ this->residuals.clear();}

const vector<double> & RPCLinkBoard::getResiduals () const { return this->residuals;}

TH1F * RPCLinkBoard::getResidualsHistogram (const string & histoObjName){
  int bins = 96/this->getClones() * 10;
  int axisLenght = 96/this->getClones();
  TH1F * residuals = new TH1F (histoObjName.c_str(),"Residual values",bins,0,axisLenght);
  
  for(int i = 0 ; i < this->getResiduals().size() ; i++){
    residuals->Fill(this->getResiduals().at(i));
  }
  
  return residuals;
  
}


TH1F * RPCLinkBoard::getNeighbourPartitionHitsHistogram(const string & histoTitle){
  
  TH1F * histo = new TH1F (histoTitle.c_str(),histoTitle.c_str(),7,-3,3);
  for (int i = 0 ; i < this->residulsInNeighbourParts.size() ; i++){
    histo->Fill(this->residulsInNeighbourParts.at(i));
  }
  
  histo->SetLineColor(kBlue);
  histo->SetFillColor(kBlue);
  histo->GetXaxis()->SetTitle("Hits with partition offset found in case of no hits in the expected partition");
  histo->GetYaxis()->SetTitle("Number of entries");
  
  return histo;
  
}


void RPCLinkBoard::findResidualValueForChannelInPartitions(const double & reconstructedChannel,vector<double> partitions){
  
  if (partitions.size() == 2) { // do nothing
  assert(!partitions.empty());
  int partitionNumber = partitions.at(0);
  bool atLeastOneClusterFound = false;
  double lowestResidualValue = 96/this->getClones() + 10; // initial value higher than the possible
  int clusterPartition = 0;
  double clusterPosition = 0;
  for (int i = 0 ; i < this->getNumberOfClusters() ; i++){
    clusterPartition = this->getXYCoordinatesOfCluster(i+1).at(1);
    clusterPosition = this->getXYCoordinatesOfCluster(i+1).at(0);
    if (partitionNumber == clusterPartition){
      atLeastOneClusterFound = true;
      if (abs(clusterPosition - reconstructedChannel) < lowestResidualValue){ lowestResidualValue = abs(clusterPosition - reconstructedChannel); } 
    }
  }
  
  if (atLeastOneClusterFound) {
      this->fillResidualValue(lowestResidualValue);
    }
  }
}

void RPCLinkBoard::fillNeighbourResidualValue(const int & residualValue){  this->residulsInNeighbourParts.push_back(residualValue);}

void RPCLinkBoard::findResidualsInNeighbourPartitionsForChannelInPartition(vector<double> partitions) {
  
  assert(!partitions.empty());
  if (partitions.size() == 2){
    
    int partitionNumber = partitions.at(0);
    int channelNumber = partitions.at(1);
    
    for(int i = 0 ; i < this->getClones() ; i++){      
      if(i+1 != partitionNumber && this->isMatchingFiredChannelInPartition(channelNumber,i+1,5)){
	
	// search for matching channels in neighbour partitions 
	int subtractionResult = partitionNumber - (i+1);
	this->fillNeighbourResidualValue(subtractionResult);
	if ( subtractionResult > 3 || subtractionResult < -3){
	  cout << " Error, impossible partition subtraction "  << subtractionResult << " " << partitionNumber << " " << i+1 << endl;
	}
      }
    }
  }
}

int RPCLinkBoard::getSumOfAllTracks (){
  
  int tracksNumber = 0 ;
  
  for (int i = 0 ; i < 96 ; i++){
    tracksNumber += this->getChannel(i+1)->getAllTracks();
  }
  
  return tracksNumber;
  
}

TH1F * RPCLinkBoard::getHistogramOfAbsoluteChannelsEfficiency(const string & histoObjName){
  
  TH1F * histo = new TH1F (histoObjName.c_str(),histoObjName.c_str(),96/this->getClones(),1,96/this->getClones());
  //cout << "-----------------" << endl;
  
  for(int i = 0 ; i < this->_nonPartitionEfficiencyChannelAllTracks.size(); i++){
    double efficiency = double(double(this->_nonPartitionEfficiencyChannelEfficientTracks[i])/double(this->_nonPartitionEfficiencyChannelAllTracks[i]));
    efficiency = efficiency*100;
    histo->SetBinContent(i+1,efficiency);
    //cout << "Channel: " << i+1 << " Efficiency: " << efficiency << endl;
  }
  
  //cout << "------------------" << endl;
  histo->SetLineColor(kBlue);
  histo->SetFillColor(kBlue);
  histo->GetXaxis()->SetTitle("Absolute channel number");
  histo->GetYaxis()->SetTitle("Efficiency in %");
  
  return histo;
  
}

