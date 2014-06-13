/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoard.h"
#include <assert.h>
#include <algorithm>
#include <boost/lexical_cast.hpp>

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
  this->_chamberConditions = NULL;
  this->clusterProfileHistogram = NULL;  
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
  
  // first clean if anything have lefted
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
	while (k <= this->getLastStripNumberOfClone(i+1) && this->getChannel(k)->hasHit()
	  
	) {
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
  //cout << this->getNumberOfClusters() << " " << clusterNumber << endl;
  if (clusterNumber <= this->getNumberOfClusters()){
  int clusterStartChannel= this->_clusterChannelNumbers.at(clusterNumber-1);
  
  while( clusterStartChannel <= this->getLastStripNumberOfClone(this->getEthaPartitionForChannel(clusterStartChannel)) && this->getStrip(clusterStartChannel)->hasHit() ){
    retval.push_back(clusterStartChannel);
    clusterStartChannel++;
  }
  }
  else cout << "Cluster number exceeds the number of existing clusters, return empty vector" << endl;
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
    
  
  for (int i = startChannel ; i <= endChannel ; i++){ 
    if (this->getChannel(i)->hasHit()){
      retval = true;
      break;
    }    
  }
  
  return retval;
}

bool RPCLinkBoard::isMatchingFiredChannelInAnyPartition(const int & trackValue,const int & clusterWidth){
  bool retval = false;
  for (int i = 0 ; i < this->getClones() ; i++){
    if (this->isMatchingFiredChannelInPartition(trackValue,i+1,clusterWidth)) {
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
  return (double)((double)this->totalReconstructedTracksWithHit/(double)this->totalReconstructedTracks)*100;
  //cout << endl;
  //cout << " Eff ratio " << this->totalReconstructedTracksWithHit << " " << this->totalReconstructedTracksWithHit << endl;
}

void RPCLinkBoard::writeClusterSizeValues(){
  vector<int> clusterSizeVector;
  for (int i = 0 ; i < this->getNumberOfClusters() ; i++){
    int partitionNum = this->getXYCoordinatesOfCluster(i+1).at(1);
    
    if(this->_clusterSizeEntries.find(partitionNum) != this->_clusterSizeEntries.end()){
      clusterSizeVector = this->_clusterSizeEntries[partitionNum];
    }
    if (this->getClusterNumber(i+1).size() < 5){
      //cout << this->getClusterNumber(i+1).size() << endl;
      clusterSizeVector.push_back(this->getClusterNumber(i+1).size());
      this->_clusterSizeEntries[partitionNum] = clusterSizeVector;
    }
  }
}

const vector<int> & RPCLinkBoard::getClusterSizeEntriesForPartition(const int & partitionNum){
  vector<int> retval;
  if (this->_clusterSizeEntries.find(partitionNum) != this->_clusterSizeEntries.end()){
    return this->_clusterSizeEntries[partitionNum];   
    
  }
  else{
    return retval;
  }
}

void RPCLinkBoard::resetClusterSizeEntries(){
  for (map<int,vector<int> >::iterator itr = this->_clusterSizeEntries.begin() ; itr != this->_clusterSizeEntries.end() ; itr++){
    itr->second.clear();
  }
  this->_clusterSizeEntries.clear();
}

TH2F * RPCLinkBoard::getTimeEvolutionProfileHistogram(const string & histoObjName){
  TH2F * timeEvoStripProfileHisto = new TH2F(histoObjName.c_str(),histoObjName.c_str(),5000,0,5000,98,0,98);
  timeEvoStripProfileHisto->GetXaxis()->SetTitle("Time difference in ");
  timeEvoStripProfileHisto->GetYaxis()->SetTitle("Channel number");
  for (int i = 0 ; i < 96 ; i++){
    vector<double> singleStripTimeResolutionVector = this->getChannel(i+1)->getTimeEvolutionVector();
    
    for (int j = 0 ; j < singleStripTimeResolutionVector.size() ; j++){
      timeEvoStripProfileHisto->Fill(singleStripTimeResolutionVector.at(j),i+1);
    }
  }
  
  return timeEvoStripProfileHisto;
  
}


TH1F * RPCLinkBoard::getHistogramOfClusterSizeForPartition(const int & partitionNum){
  string histoName  = boost::lexical_cast<string>(partitionNum);
  if (this->getExtendedChamberConditions() != NULL){
    histoName = this->getBasicChamberConditions()->getChamberName() + "_" + histoName;
  }
  histoName = "ClusterSize_" + histoName;
  
  TH1F * clsSize = new TH1F(histoName.c_str(),histoName.c_str(),10,1,11);
  for(int i = 0 ; i < this->getClusterSizeEntriesForPartition(partitionNum).size() ; i++){
    //cout << this->getClusterSizeEntriesForPartition(partitionNum).at(i) << endl;
    clsSize->Fill(this->getClusterSizeEntriesForPartition(partitionNum).at(i));
  }  
  return clsSize;
}

void RPCLinkBoard::writeTimeEvolutionValues(){
  for (int i = 0 ; i < 96 ; i++){
    this->getChannel(i+1)->writeMultiHitDifferences(1000); // use this as default value, it too big so it would contain all
  }
}

const vector<vector<double> > & RPCLinkBoard::getTimeEvolutionVectorsForAllStrips(){
  vector< vector<double> > retval;
  for (int i = 0 ; i < 96 ; i++){
    retval.push_back(this->getChannel(i+1)->getTimeEvolutionVector());
  }
  return retval;
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

TGraphAsymmErrors * RPCLinkBoard::getChannelsEfficiencyErrorGraph (const string & graphName){
  
  TH1F * pass = new TH1F((graphName+"_pass").c_str(),"pass",96,1,96);
  TH1F * total = new TH1F((graphName+"_total").c_str(),"total",96,1,96);
  TGraphAsymmErrors * graph = new TGraphAsymmErrors(pass);
  graph->SetName(graphName.c_str());
  graph->SetTitle(graphName.c_str());
  
  for (int i = 0 ; i < 96 ; i++){
     pass->SetBinContent(i+1,this->getChannel(i+1)->getEfficientTracks());
     total->SetBinContent(i+1,this->getChannel(i+1)->getAllTracks());
  }
  
  graph->Divide(pass,total);  
  pass->Delete();
  total->Delete();
  
  return graph;
  
}

TH1F * RPCLinkBoard::getDistributionOfChannelsEfficiency(const string & histoObjName){
  
  TH1F * histo = new TH1F (histoObjName.c_str(),"Channels efficiency distribution",1010,0,101);
  histo->GetXaxis()->SetTitle("Efficiency %");
  histo->GetYaxis()->SetTitle("Entries");
  
  for (int i = 0 ; i < 96 ; i++){
    if (this->getChannel(i+1)->getEfficiency() > -1 ){
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
  int bins = 96/this->getClones() * 2;
  int axisLenght = 96/this->getClones()*2 + 2;
  TH1F * residuals = new TH1F (histoObjName.c_str(),"Residual values",bins,-(axisLenght/2),axisLenght/2);
  
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
  double lowestAbsResidualValue = 96/this->getClones() + 10; // initial value higher than the possible
  double lowestResidualValue = 0;
  int clusterPartition = 0;
  double clusterPosition = 0;
  for (int i = 0 ; i < this->getNumberOfClusters() ; i++){
    clusterPartition = this->getXYCoordinatesOfCluster(i+1).at(1);
    clusterPosition = this->getXYCoordinatesOfCluster(i+1).at(0);
    if (partitionNumber == clusterPartition){
      atLeastOneClusterFound = true;
      if (abs(clusterPosition - reconstructedChannel) < lowestAbsResidualValue){ lowestAbsResidualValue = abs(clusterPosition - reconstructedChannel); lowestResidualValue = clusterPosition - reconstructedChannel; } 
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
    // this vector should have the reconstructed channel number and the expected partition 
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

int RPCLinkBoard::getSumOfAllChannelTracks (){
  
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

/** run configuration object methods */

RPCChamberConditionsBase * RPCLinkBoard::getBasicChamberConditions(){
  return this->_chamberConditions;
}

RPCChamberConditions * RPCLinkBoard::getExtendedChamberConditions(){
  return dynamic_cast<RPCChamberConditions*>(this->_chamberConditions);
}

void  RPCLinkBoard::setCurrentRunDetails(RPCChamberConditionsBase * runDetails){
  this->_chamberConditions = runDetails;
}

TH2F * RPCLinkBoard::getEfficiencyHistogramForGapOperationMode(const string & mode){
  TH2F * retval = new TH2F((this->getExtendedChamberConditions()->getChamberName()+mode).c_str(),this->getExtendedChamberConditions()->getChamberName().c_str(),2100,8000,10100,660,0,120);
  map<int,double> values = this->_chamberEfficiencyVsGapHV.find(mode)->second;
  for (map<int,double>::iterator iter = values.begin();iter != values.end();iter++){
    retval->Fill(iter->first,iter->second);
  }
  return retval;
}

void RPCLinkBoard::setEfficiencyVsHVentryForMode(const int & HV,const double & efficiency,const double & effUncertainty,const string & mode){
  map<int,double> modevals;
  if(this->_chamberEfficiencyVsGapHV.find(mode) == this->_chamberEfficiencyVsGapHV.end()){
    this->_chamberEfficiencyVsGapHV[mode] = modevals;
  }
  else{
    modevals = this->_chamberEfficiencyVsGapHV.find(mode)->second;
  }
  modevals[HV] = efficiency;
  this->_chamberEfficiencyVsGapHV[mode] = modevals;
  
}

void RPCLinkBoard::drawNestedSigmoidPlotForAllModes(const string & title){
  
  TCanvas * canvas = new TCanvas(title.c_str(),title.c_str(),1200,700);
  canvas->cd();
  vector<int> colors;
  colors.push_back(kGreen);
  colors.push_back(kBlue);
  colors.push_back(kRed);
  int color = 0;
  TLegend * leg;
  leg = new TLegend(0.150313,0.713733,0.318372,0.865571);
  for (map<string,map<int,double > >::iterator iter = this->_chamberEfficiencyVsGapHV.begin();iter != this->_chamberEfficiencyVsGapHV.end();iter++){
    TH2F * hist = getEfficiencyHistogramForGapOperationMode(iter->first);
    hist->SetMarkerColor(colors.at(color));
    hist->SetMarkerStyle(kFullCircle);
    hist->SetStats(kFALSE);
    hist->GetXaxis()->SetTitle("Applied HV");
    hist->GetYaxis()->SetTitle("Efficiency %");
    leg->AddEntry(hist,iter->first.c_str(),"p");
    if(color == 0){
      hist->Draw();
    }
    else{
      hist->Draw("same");
    }
    color++;
  }
  string filename = title+".root";
  leg->SetFillColor(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->Draw();
  canvas->SaveAs(filename.c_str());
  
}

void RPCLinkBoard::updateSigmoidHistogramWithNewValue(const string & histoFolder,const string & chamberID,const string & triggerMode,const int & HV,const double & efficiencyValue){
  
  string fullFileName = histoFolder+"SigmoidHistos_"+chamberID+".root";
  string histoName = chamberID+"_"+triggerMode;
  TFile * sigmoFile = new TFile(fullFileName.c_str(),"UPDATE");
  TH2F * currentHisto ;
  TKey *key;
  TObject *obj;
  // search for the name, if does not exists create the histogram for first time  
  TIter nextkey( sigmoFile->GetListOfKeys() );
  bool histoFound = false;
  while (( key = (TKey*)nextkey()) != NULL ) {
    obj = key->ReadObj();
    if (obj->GetName() == histoName){
      histoFound = true;
      break; // if found
    }
  }
  
  // now if the histo is found just take it, if not create it
  
  if (histoFound){    
    currentHisto = dynamic_cast<TH2F*>(sigmoFile->Get(histoName.c_str()));
  }  
  else{
    currentHisto = new TH2F(histoName.c_str(),histoName.c_str(),2100,8000,10100,1100,0,110);
  }
  
  currentHisto->Fill(HV,efficiencyValue);
  currentHisto->SetMarkerStyle(kFullCircle);
  currentHisto->SetMarkerColor(kBlue);
  currentHisto->SetStats(kFALSE);
  
  // write the histo and close the file
  currentHisto->Write(currentHisto->GetName(),TObject::kOverwrite);
  sigmoFile->Close("R");
  sigmoFile->Delete();
  
}

// Clusterization time profile

void RPCLinkBoard::printClusterTimesForClusterNumber(const int & clusterNumber){
  for (int i = 0 ; i < this->getClusterNumber(clusterNumber).size() ; i++){
    cout << this->getStrip(this->getClusterNumber(clusterNumber).at(i))->getHits().at(0) << " ";
  }
  cout << endl;
}

int RPCLinkBoard::getNumberOfClusterForStripNumber(const int & channel){
  int retval = -1;
  if (channel >= 1 && channel <= 96 ){
  for (int i = 0 ; i < this->getNumberOfClusters() ; i++){
    for (int j = 0 ; j < this->getClusterNumber(i+1).size() ; j++){
      if (this->getClusterNumber(i+1).at(j) == channel){
	retval = i+1;
	break;
      }      
    }
    if (retval != -1) break; 
  }
  }
  return retval;
}

void RPCLinkBoard::initClusterTimeProfileHistogramWithUniqueName(const string & uniqueName){
  this->clusterProfileHistogram = new TH2F(uniqueName.c_str(),uniqueName.c_str(),61,-20.5,40.5,9,-4.5,+4.5);
}

void RPCLinkBoard::writeClustersTimeProfileForClusterNumber (const int & clusterNumber) {
  
  
  if (this->getPointerToClustersTimeProfileHisto() != NULL && this->getNumberOfClusters() >= clusterNumber && this->getClusterNumber(clusterNumber).size() == 3 ){
    vector<int> theCluster = this->getClusterNumber(clusterNumber);
    int middleChannelTimeReference = 0;
    int centerOfCluster = 0;
    if (theCluster.size() % 2 == 0) {
      // its even, get the two numbes in the center
      int leftElementIndex = theCluster.size() / 2;
      int rightElementIndex = leftElementIndex+1 ;
      centerOfCluster =  this->getStrip( theCluster.at(leftElementIndex-1) )->getHits().at(0) < this->getStrip( theCluster.at(rightElementIndex-1) )->getHits().at(0) ? leftElementIndex : rightElementIndex; 
      
    }
    else {
      // its odd, get the middle channel
      centerOfCluster = (this->getClusterNumber(clusterNumber).size() - 1)/2 + 1;
    }
    middleChannelTimeReference = this->getChannel(theCluster.at(centerOfCluster-1))->getHits().at(0);
    
    //cout << " Center of cluster is: " << centerOfCluster << " value is: " << middleChannelTimeReference  << ", ";
    
    int startingPoint = centerOfCluster - int(theCluster.size());
    int endingPoint = startingPoint + int(theCluster.size());
    
    //cout << endl << startingPoint << " " << endingPoint << endl;
    
    for (int i = 0 ; i < theCluster.size(); i++){
      this->getPointerToClustersTimeProfileHisto()->Fill( int(this->getChannel( theCluster.at(i))->getHits().at(0)) - middleChannelTimeReference,i - centerOfCluster + 1);
      //cout << "cluster entry " << i+1 << " fill at " << i - centerOfCluster + 1 << " fill value " <<   int(this->getChannel(theCluster.at(i))->getHits().at(0)) - middleChannelTimeReference << endl;
    }
  }
}

void RPCLinkBoard::incrementChannelHitCountersForCurrentEvent(){
  for (int i = 0 ; i < 96 ; i++){
    this->getChannel(i+1)->incrementNumberOfCounts();    
  }
}

void RPCLinkBoard::incrementNumberOfCountsOutOfReferenceWindow(const int & reference,const int & window){
  for (int i = 0 ; i < 96 ; i++){
    this->getChannel(i+1)->incrementNumberOfCountsOutOfReferenceWindow(reference,window);
  }
}

void RPCLinkBoard::resetChannelHitCounters(){
  for (int i = 0 ; i < 96 ; i++){
    this->getChannel(i+1)->resetNumberOfCounts();
    this->getChannel(i+1)->resetNumberOfNoisyCounts();
  }
}

TH1F * RPCLinkBoard::getHistogramOfChannelRates(const string & histoObjName,const double & totalTimeInSeconds_denominator){
  
  TH1F * noiseHisto = new TH1F(histoObjName.c_str(),histoObjName.c_str(),96,1,96);
  noiseHisto->SetLineColor(kBlue);
  noiseHisto->SetFillColor(kBlue);
  noiseHisto->GetXaxis()->SetTitle("Channel number");
  noiseHisto->GetYaxis()->SetTitle("Rate in Hz");
  
  for(int i = 0 ; i < 96 ; i++){
    noiseHisto->SetBinContent(i+1, (double)((double)this->getChannel(i+1)->getNumberOfNoisyCounts() / totalTimeInSeconds_denominator));
  }
  
  return noiseHisto;
}

void RPCLinkBoard::writeTimeEvolutionValuesInTimeWindowAroundRefTime(const int & timeWindow){
  for (int i = 0 ; i < 96 ; i++){
    this->getChannel(i+1)->writeMultiHitDifferences(timeWindow);
  }
}

TH1F * RPCLinkBoard::getHistoOfChannelHitCounts(const string & hitsDistribution){
  TH1F * channelsHitsDistribution = new TH1F(hitsDistribution.c_str(),hitsDistribution.c_str(),96,1,96);
  for (int i = 0 ; i < 96 ; i++){
    channelsHitsDistribution->SetBinContent(i+1,this->getChannel(i+1)->getNumberOfCounts());
  }
  
  return channelsHitsDistribution;
}

bool RPCLinkBoard::channelIsCloseToEdge(const int & channelNumber,const int & numberOfChannelsPrecision){
  int absChNumber = channelNumber % (96/this->getNumberOfClones());
  bool retval = false;
  if (absChNumber - numberOfChannelsPrecision < this->getFirstStripNumberOfClone(1) || absChNumber + numberOfChannelsPrecision > this->getLastStripNumberOfClone(1)) retval = true;
  
  return retval;
}

// endof previous

