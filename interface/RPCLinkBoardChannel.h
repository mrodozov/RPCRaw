/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_LINK_BOARD_CHANNEL
#define RPC_LINK_BOARD_CHANNEL

#include <vector>
#include "RollCore/ExtendedStrip.h"

class RPCLinkBoardChannel : public ExStrip {
  
  const std::vector<unsigned> * _vectorOfhits;
  
protected:
  
public:
  
  void setHits (const std::vector<unsigned> & vectorOfHits){ this->_vectorOfhits = &vectorOfHits; }; // set vector of hits, with receiving reference to the data object trought the parent Chamber object
  const std::vector<unsigned> & getHits () const { return *this->_vectorOfhits;} // get the vector of hits
  bool hasHit(){ return !this->_vectorOfhits->empty();} // returns true if the channel has at least one hit 
  bool hasMultipleHits (); // returns true if the channel has more than one hit 
  
};


#endif