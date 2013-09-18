/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/
#ifndef RPC_LINK_BOARD_CHIP
#define RPC_LINK_BOARD_CHIP

#include "RPCLinkBoard.h"

class RPCLinkBoardChip : public Chip {
  
protected:
  
  virtual RPCLinkBoardChannel * getPointerToStripPointers(); // this could be done without interface 
  virtual void setStripsPointersToPointToObjects(StripInterface* Allocated_ExStrip_Objects);
  
public:
  
  virtual RPCLinkBoardChannel* getChipStrip(const int& stripNumber);
  virtual void assignStripsFromRoll(RollInterface* aRoll);
  
  RPCLinkBoardChip();
  ~RPCLinkBoardChip();
  
};

#endif