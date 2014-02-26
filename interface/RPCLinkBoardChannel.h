/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_LINK_BOARD_CHANNEL
#define RPC_LINK_BOARD_CHANNEL

#include <vector>
#include "RollCore/ExtendedStrip.h"

using namespace std;

class RPCLinkBoardChannel : public ExStrip {
  
  const std::vector<unsigned> * _vectorOfhits;
  double _channelEfficiency;
  int allTracks;
  int efficientTracks;
  vector<double> timeEvolutionDifferences;
  
protected:
  
public:
  
  RPCLinkBoardChannel(); // default constructor
  ~RPCLinkBoardChannel(); // destuctor
  
  void setHits (const std::vector<unsigned> & vectorOfHits);//{ this->_vectorOfhits = &vectorOfHits; }; // set vector of hits, with receiving reference to the data object trought the parent Chamber object
  const std::vector<unsigned> & getHits () const;// { return *this->_vectorOfhits;} // get the vector of hits
  bool hasHit(); // returns true if the channel has at least one hit 
  bool hasMultipleHits (); // returns true if the channel has more than one hit 
  void setEfficiency (const double & channelEfficiency);// { this->_channelEfficiency = channelEfficiency; } // set the efficiency value. the value is first calculated 
  double getEfficiency () ;// { return (this->efficientTracks/this->allTracks)*100  ;}  // get the efficiency of this channel
  
  // overload in next inheritance in case of need 
  virtual void resetAllCounters(); 
  
  void writeMultiHitDifferences();
  void incrementEfficiencyCounters(const bool & hitIsFound);
  void resetEfficiencyCounters();
  void resetTimeEvoEntriesVector();
  const int & getAllTracks () const;// { return this->allTracks;}
  const int & getEfficientTracks () const;// { return this->efficientTracks;}
  const int & getLinkBoardChannelNumberInChamber() { return this->getOnlineNumber() ; } // returns the number of the channel within the chamber (so called online number), which is different from the strip number. Between 1 and 96
  const int & getStripNumberInRoll() { return this->getOfflineNumber(); } // returns the number of the channel within the Roll (CMSSW convention). Strip objects within one chamber may have the same strip number as another Strip object from the same chamber, but each number is specified for given partition, where the combination of partition number (or label) and the strip number is always unique
  const vector<double> & getTimeEvolutionVector() { return this->timeEvolutionDifferences; }
  
};

//typedef RPCLinkBoardChannel RPCChamberChannel;

#endif