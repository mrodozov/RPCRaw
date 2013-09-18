/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/

#ifndef STRIP_INTERFACE_H
#define STRIP_INTERFACE_H

class StripInterface {
  
  virtual const int & getOfflineNumber()=0;
  virtual const int  & getOnlineNumber()=0;
  
public:
  
  virtual ~StripInterface();
  
};


/** 
* @brief this class is meant to be base polymorphic class for Strip Objects
*/

#endif