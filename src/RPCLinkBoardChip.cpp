/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoard.h"

using namespace std;

RPCLinkBoardChannel * RPCLinkBoardChip::getPointerToStripPointers(){
  
  return dynamic_cast<RPCLinkBoardChannel*>(this->stripPointers);
  
}

void RPCLinkBoardChip::setStripsPointersToPointToObjects(StripInterface * Allocated_ExStrip_Objects){
  
  this->stripPointers = Allocated_ExStrip_Objects;
  
}

void RPCLinkBoardChip::assignStripsFromRoll(RollInterface* aRoll){
  
  RPCLinkBoardChannel * pointerToStrips[8];
  
  for(int i=0;i < 8;i++){
     pointerToStrips[i] =  dynamic_cast<RPCLinkBoard*>(aRoll)->getStrip(this->getChipNumber()*8+(i+1));    
  }
  
  this->setStripsPointersToPointToObjects(*pointerToStrips);
  
}

RPCLinkBoardChannel * RPCLinkBoardChip::getChipStrip(const int & stripNumber){
  
  return & this->getPointerToStripPointers()[stripNumber-1];
  
}

RPCLinkBoardChip::RPCLinkBoardChip() : Chip() {
  
}

RPCLinkBoardChip::~RPCLinkBoardChip(){
  
}

// do as you wish here