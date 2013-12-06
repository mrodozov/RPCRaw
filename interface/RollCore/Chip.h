/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#ifndef RPC_CHIP_H
#define RPC_CHIP_H

#include <string>
#include <fstream>
#include <sstream>
#include "../../interface/ROOT.h"
#include "ExtendedStrip.h"
#include "RollInterface.h"
#include "ChipInterface.h"

class Chip : public ChipBaseClass{
  
  int chipNumber;
  int chipID;
  
protected:
  /** essential methods*/
  virtual ExStrip * getPointerToStripPointers(); // to be overridden 
  virtual void setStripsPointersToPointToObjects(StripInterface * Allocated_ExStrip_Objects); // to be overridden
  
public:
  
  virtual ExStrip * getChipStrip(const int & stripNumber); // tested
  virtual void assignStripsFromRoll(RollInterface * aRoll); // tested
  
  /** end of essential methods */
  
  void setChipNumber(const int & ChipNumber); // tested
  void setChipID(int ID); // tested
  
  const int & getChipNumber(); // tested
  const int & getChipID(); // tested
  
//  RollChip();//decide what should be the constructor if not empty
  
  double getMeanValueOfStrips(); // tested
  double getAverageValueOfStrips(); // tested
  TH1F * getHistoOfChipStrips(const std::string & runString); // tested
  TH1F * getDistributionOfChipsStripRateValues(const std::string & runString); // not tested 
  double getMaxRateValueOfChipStrips();  // tested
  
  Chip();
  
  ~Chip();
  
};

#endif