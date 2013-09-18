/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#ifndef STRIP_H
#define STRIP_H

#include <string>
#include "StripInterface.h"


class Strip : public StripInterface{
  
  int wheel_disk_num;
  int sector;
  int number;
  int offlineNumber;
  std::string wheel_disk;
  std::string roll_id;
  std::string roll_id_num;
  std::string offlineID;
  std::string wheel_disk_as_string;
  bool isDead ; // maybe this fields should become boolean
  bool isMasked ; // and this ....
  bool isUnpluged ; // and this one ... :)
  
public:
  
  virtual void initWithValues(const  std::string & W_D,const int & W_D_N,const int & sec,const std::string & R_ID,const std::string & R_ID_N,const int & num);
  
  // constructors
  Strip (const std::string & W_D,const int & W_D_N,const int & sec,const std::string & R_ID,const std::string & R_ID_N,const int & num);
    
  Strip(); //in case that i need array of strips
  
  ~Strip();
  
  const bool & getIsMasked();
  
  const bool & getIsDead();
  
  const bool & getIsDisconnected();
  
  const std::string & getOfflineID();
  
  const int & getOfflineNumber();
  
  const int & getOnlineNumber();
  
  // mutators
  
  void Mask();
  
  void Kill();
  
  void Disconnect();
  
  void unsetMasked();
  
  void unsetDead();
  
  void SetNotDisconnected();
  
  virtual void ClearConditions();
  
  void setOfflineID( const std::string &);
  
  void setOfflineNumber(const int &);
  
  void printOutStatus();
  // new methods
  void setOnlineNumber(const int &); // declared as const ref because the variable passed should not be modified inside of the method's body ! :)
  
};


#endif
