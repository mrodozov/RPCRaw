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
  
  //
  
  void incrementEfficiencyCounters(const bool & hitIsFound);
  void resetEfficiencyCounters();
  const int & getAllTracks () const;// { return this->allTracks;}
  const int & getEfficientTracks () const;// { return this->efficientTracks;}
  
};

//typedef RPCLinkBoardChannel RPCChamberChannel;

#endif