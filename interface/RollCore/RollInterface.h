/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/

#ifndef ROLL_IFACE_H
#define ROLL_IFACE_H

#include "StripInterface.h"

class RollInterface {
  
protected:
  
  StripInterface * strips;
  
public:
  
  virtual void allocStrips()=0;
  virtual void initStrips()=0;
  virtual StripInterface * getStripCollection()=0;
  virtual StripInterface * getStrip(int)=0;
  RollInterface();//this constructor prevent calling delete on this field if its not 
  virtual ~RollInterface();
  
};

#endif