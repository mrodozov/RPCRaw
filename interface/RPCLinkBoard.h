/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_LINK_BOARD_H
#define RPC_LINK_BOARD_H

#include "RollCore/ExtendedRoll.h"
#include "RPCLinkBoardChannel.h"
#include "RPCLinkBoardChip.h"
#include "RPCChamberConditions.h"

class RPCLinkBoard : public ExRoll {
  
  bool _isTriggerChamber;
  bool _isReferenceChamber;
  string _uniqueConstructionID; // i.e. RE4-3-CERN-32
  const vector< vector<unsigned> > * _vectorToStripsHits;
  int _cosmicStationNumber; // station on the cosmic stand if its given. Z coordinate to be mapped with this
  vector<int>_clusterChannelNumbers; // clusters starting strips - each entry is the starting channel for a cluster, or a single hit
  // efficiency counters
  int totalReconstructedTracks;
  int totalReconstructedTracksWithHit;
  vector<double> residuals;
  vector<double> residulsInNeighbourParts;
  vector<int> _nonPartitionEfficiencyChannelEfficientTracks;
  vector<int> _nonPartitionEfficiencyChannelAllTracks;
  map<int,vector<int> > _clusterSizeEntries;
  RPCChamberConditionsBase * _chamberConditions;
  map<string, map<int,double> > _chamberEfficiencyVsGapHV;
  // get each partition cluster size
  
protected:
  
public:
  
  // channel methods
  virtual void allocStrips(); // allocate concrete type, its a factory method
  virtual void initStrips(); // init whats needed. execute the parent method with this signature inside this one first, and then add you specifications
  virtual void allocAndInit() {this->allocStrips(); this->initStrips();} // this better be redefined in every inherited class, because alloc and init are redefined. 
  
  virtual RPCLinkBoardChannel * getStrip (int stripNumber); // get channel by number. Start from 1 , not from 0
  virtual RPCLinkBoardChannel * getStripCollection (); // gets the pointer to all channel objects
  virtual RPCLinkBoardChannel * getChannel( int channelNumber); // substitution of getStrip(int number) , since some people would prefer channel 
  
  // constructors and destructor
  
  RPCLinkBoard();
  RPCLinkBoard(const string& RollOnlineName);
  ~RPCLinkBoard();
  
  // chips methods (so far not needed, but still if channels are going to be get 'by Chip', this methods do the magic)
  
  virtual RPCLinkBoardChip * getChipByNumber(const int& ChipNumber); // get previously created chip object by its number. 
  void insertNewChipWithNumberAndID(const int & num,const int & ID); // create new chip object 
  // void assignChipsFromSource(map< string, std::map< int, int > > ChipsMap); // implement this when there is a map for this chips numbers and chamber RE-IDs 
  
  // custom methods, do as you wish here
  
  void setIsTriggerChamber(const bool & isTrigger){ this->_isTriggerChamber=isTrigger;} // set if the object is used as holder for trigger channels
  const bool & isTriggerChamber () const { return this->_isTriggerChamber;} // get if the object is used as trigger object
  void setStripsHitsDataFromSource(const vector< vector<unsigned> > & vectorSource); // set vector to be used as vector that holds the hits data. usually vector of 96 elements that are vector<unsigned>. this structure is changed when the event is changed 
  const vector< vector<unsigned> > & getStripsHitsDataVector () const; // get the hits vector
  
  void setStationNumber(const int & number){this->_cosmicStationNumber = number;} // set the cosmic stand station
  const int & getStationNumber () const { return this->_cosmicStationNumber;} // get the cosmic stand station
  
  void setConstructionID(const string & constructionID) { this->_uniqueConstructionID = constructionID; } // set string to unique identify 
  const string & getConstructionID () const { return this->_uniqueConstructionID ;} // get the unique construction id of the RE4 upgrade if any
  
  void setAsReferenceChamber(const bool & isUsedAsReference) { this->_isReferenceChamber=isUsedAsReference; } 
  const bool & isReferenceChamber() { return this->_isReferenceChamber; }
  
  int getEthaPartitionForChannel(const int & channelNumber); // get the clone number for given channel
  
  RPCChamberConditionsBase * getBasicChamberConditions(); // returns base pointer that could execute any of the abstract methods regardless of their concrete implementation
  void setCurrentRunDetails(RPCChamberConditionsBase * runDetails); // asign allocated condition object to the base pointer
  virtual RPCChamberConditions * getExtendedChamberConditions(); // returns specific type with additional implementation if any. could be redefined what type to return.
  
  // Cluster methods 
  
  const vector<int> & getClusters ();// { return this->_clusterChannelNumbers ;} // get vector of clusters (vectors) . each vector holds the numbers of the strips in a cluster 
  void findAllClustersForTriggerTimeReferenceAndTimeWindow(int triggerTimeReference,int timeWindow,int maxClusterSize=5); // this method finds all clusters starting channel numbers , and store them as elements of vector
  
  int getNumberOfClusters() ;//{ return this->_clusterChannelNumbers.size(); } // get the number of clusters in the chamber if any 
  vector<int> getClusterNumber(int clusterNumber) ;
  int getSizeOfCluster(int clusterNumber);// { return this->getClusterNumber(clusterNumber).size(); } // get number of 
  vector<vector<int> > getStripsHitsTimes(); // get vector of vector - where each vector is cluster of the hits times 
  vector<int> getStripsHitsTimesForCluster(int clusterNumber);//{ return this->getStripsHitsTimes().at(clusterNumber-1);} 
  int getAverageTimeForCluster(int clusterNumber); // get the average value of the cluster time values : average of getStripsHitsTimes 
  
  vector<double> getXYCoordinatesOfCluster(const int & clusterNumber); // get the cluster XY position. X position is given in terms of channel number ,where X position never exceeds the total number of channels in one Etha partition in that chamber, Y position is according to the Etha partition number (1,2 or 3)
  vector<double> getXYZcoordinatesOfCluster(const int & clusterNumber); // get XY coordinates plus one more coordinate based on the station (shelf) number using the get station 
  
  // Residuals methods
  
  void findResidualValueForChannelInPartitions(const double & reconstructedChannel,vector<double> partitions);
  void findResidualsInNeighbourPartitionsForChannelInPartition(vector<double> partitions); // test to search for Y offset in vertical tracks only
  void fillResidualValue(const double & residualValue);//{ this->residuals.push_back(residualValue); }
  void fillNeighbourResidualValue(const int & residuals);
  void resetResiduals ();//{ this->residuals.clear();}
  void resetNeighbourPartsResiduals();
  const vector<double> & getResiduals () const;// { return this->residuals;}
  TH1F * getResidualsHistogram (const string & histoObjName);
  TH1F * getNeighbourPartitionHitsHistogram(const string & histoTitle);
  
  // Efficiency methods
  
  void incrementEfficiencyCounters(const bool & hitIsFound);  
  double getIntegratedChannelEfficiency(); // get the average of the all channels efficiency
  double getChamberEfficiency(); // calculate chamber efficiency on the top of the global hits detected by the chamber
  int getNumberOfAllTracks () { return this->totalReconstructedTracks ;}
  int getNumberOfEfficientTracks () { return this->totalReconstructedTracksWithHit ; }
  bool isMatchingFiredChannelInPartition(const int & trackValue,const int & partition,const int & clusterWidth);
  void resetEfficiencyCounters();
  void resetChannelsEfficiencyCounters() ; //{ for (int i=0; i < 96 ; i++) this->getChannel(i+1)->resetEfficiencyCounters(); } 
  void incrementAbsoluteChannelCounters(const bool & hitIsFound,const int & absoluteChannelNumber); // absoluteChannelNumber means channel number without specified partition number
  void clearAbsoluteChannelsCounters();
  
  // cluster size methods
  void writeClusterSizeValues(); // use to fill cluster size values after each event
  const vector<int> & getClusterSizeEntriesForPartition(const int & partitionNum); // use partition number starting from 1 
  void resetClusterSizeEntries(); // erase the records for cluster size for the run
  
  // time evolution methods
  void writeTimeEvolutionValues();
  const vector<vector<double> > & getTimeEvolutionVectorsForAllStrips();  
  
  /** get histograms from the records of the object */
  
  TH1F * getHistogramOfChannelsEfficiency(const string & histoObjName);
  TH1F * getDistributionOfChannelsEfficiency(const string & histoObjName);
  TH1F * getHistogramOfTracksVsChannels(const string & histoObjName);
  TH1F * getHistogramOfAbsoluteChannelsEfficiency(const string & histoObjName);
  TH1F * getHistogramOfClusterSizeForPartition(const int & partitionNum);
  TH2F * getTimeEvolutionProfileHistogram (const string & histoObjName);
  
  // Statistics methods 
  
  int getSumOfAllChannelTracks ();
  
  // Global histograms
  void updateSigmoidHistogramWithNewValue(const string & histoFolder,const string & chamberID,const string & triggerMode,const int & HV,const double & efficiencyValue);

  TH2F * getEfficiencyHistogramForGapOperationMode(const string & mode);
  void setEfficiencyVsHVentryForMode(const int & HV,const double & efficiency,const double & effUncertainty,const string & mode);
  void drawNestedSigmoidPlotForAllModes(const string & title);
  
};

typedef RPCLinkBoard RPCChamber;

#endif