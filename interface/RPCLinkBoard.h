/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_LINK_BOARD_H
#define RPC_LINK_BOARD_H

#include "RollCore/ExtendedRoll.h"
#include "RPCLinkBoardChannel.h"
#include "RPCLinkBoardChip.h"

class RPCLinkBoard : public ExRoll {
  
  bool _isTriggerChamber;
  string _uniqueConstructionID; // i.e. RE4-3-CERN-32
  const vector< vector<unsigned> > * _vectorToStripsHits;
  int _cosmicStationNumber; // station on the cosmic stand if its given. Z coordinate to be mapped with this
  vector<int> _clusterStartingNumbers; // clusters starting strips - each entry is the starting channel for a cluster, or a single hit
  
  
protected:
  
public:
  
  // channel methods
  virtual void allocStrips(); // allocate concrete type, its a factory method
  virtual void initStrips(); // init whats needed. execute the parent method with this signature inside this one
  virtual void allocAndInit() {this->allocStrips(); this->initStrips();} // this better be redefined in every inherited class, because alloc and init are redefined and this one 
  
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
  const bool & getIsTriggerChamber () const { return this->_isTriggerChamber;} // get if the object is used as trigger object
  void setStripsHitsDataFromSource(const vector< vector<unsigned> > & vectorSource); // set vector to be used as vector that holds the hits data. usually vector of 96 elements that are vector<unsigned>. this structure is changed when the event is changed 
  const vector< vector<unsigned> > & getStripsHitsDataVector () const; // get the hits vector
  
  void setStationNumber(const int & number){this->_cosmicStationNumber = number;} // set the cosmic stand station
  const int & getStationNumber () const { return this->_cosmicStationNumber;} // get the cosmic stand station
  
  void setConstructionID(const string & constructionID) { this->_uniqueConstructionID = constructionID; }
  const string & getConstructionID () const { return this->_uniqueConstructionID ;}
  
  const vector<int> & getClusterStartingChannels () { return this->_clusterStartingNumbers ;} // get vector with clusters starting strips. each entry mark channel from where a cluster starts  
  void findAllClustersForTiggerTimeReferenceAndTimeWindow(int triggerTimeReference,int timeWindow); // this method search and mark all clusters starting strips. Later, one always 
  
  
  
};

#endif