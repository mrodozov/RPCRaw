/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#ifndef RPC_CHIP_BASECLASS_H
#define RPC_CHIP_BASECLASS_H

#include "RollInterface.h"

class ChipBaseClass {

protected:
  
  StripInterface * stripPointers; 
  
public:
  
  virtual StripInterface * getChipStrip(const int & stripNumber)=0; //
  virtual void assignStripsFromRoll(RollInterface * aRoll)=0; //
  
  virtual StripInterface * getPointerToStripPointers()=0;
  virtual void setStripsPointersToPointToObjects(StripInterface * ObjectsOf)=0;
  
  ChipBaseClass();
  virtual ~ChipBaseClass();
  
};


#endif 