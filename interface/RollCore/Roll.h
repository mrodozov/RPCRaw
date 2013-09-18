/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/

#ifndef ROLL_H
#define ROLL_H

//#include <string>
#include "RollInterface.h"
#include "DataObject.h"
#include "Strip.h"

class Roll : public RollInterface{
  
  std::string WheelOrDisk;
  int WheelOrDiskNumber;
  int Sector;
  std::string ID;
  std::string ID_N;
  int numberOfUnpluged;
  int numberOfClonedRolls;
  int * RawIDofClone;
  std::string * RollIDofClone;
  std::string * RollPartOfClone;
  //end of members
  
protected:
  
public:
  
  // methods important for the logic 
  
  virtual Strip * getStripCollection(); // to override, this method should return concrete type 
  virtual Strip * getStrip(int number); // to override, this method should return concrete type
  virtual void allocStrips(); // override , this method should allocate the objects that are associated with the abstract field 'strips' (objects of type Strip in this case)
  virtual void initStrips(); // use the existing implementations of all the previous classes with invoking LastMotherClass::initStrips() and add new functionality after that
  
  Roll();
  
  Roll(const std::string & W_D_,const int & W_D_N_,const int & sec_,const std::string & roll_id_,const std::string & roll_id_num_);  
  
  Roll(const string & RollOnlineName);
  
  ~Roll();
  
  // not to overried in derived
  
  void  ParseStringToIdentifiers(const std::string& fullRollOnlineString,std::string& WheelOrD,int& WheelOrDiskN,int& Sect,std::string& ID_,std::string& IDN_);
  virtual void initClonesData(const int & numberOfClones);
  void setDisconectedStrips();
  void Clone();
  
  // end of crucial for migration
  
  // not neccesary but good to override
  
  virtual void initWithValues(const std::string & W_D,const int & W_D_N,const int & sec,const std::string & roll_id,const std::string & roll_id_num);
  virtual void initWithValues(const string & RollNameString);
  
  // end of not neccesary but good 
  
  // 
  
  const int & getWheel_Disk_Number();
  
  void setWheelDiskNumber(const int & number);
  
  const int & getSector();
  
  void setSector(const int & sector);
  
  void setMaskedStripsFromSource(const DataObject & source); // to be implemented with const DataObject
  
  void setDeadStripsFromSource(const DataObject & source); // to be implemented with const DataObject
  
  void setRollRawIDfromSource(const DataObject & source); // to be implemented with const DataObject
  
  const int & getRawIDofClone(const int & clone);
  
  //void configureWith(const DataObject & configurationObject);
  
  std::string returnFieldAsString(const int & field);
  
  const std::string & getWheel_Disk();
  
  void setWheelDisk(const string & WheelDisk);
  
  const std::string & getRollID();
  
  void setRoll_ID_(const string & ID);
  
  const std::string & getOptionalRollID();
  
  void setRollOptionalID(const string & OptionalID);
  
  int getMasked();
  
  int getDead();
  
  void setNumberOfUnpluggedStrips(const int & unplugged);
  
  int getUnpluged();
  
  int  getMaskedFromClone(const int & cloneNum);
  
  int  getDeadFromClone(const int & cloneNum);
  
  int  getUnplugedFromClone(const int & cloneNum);
  
  void printOutStatus();
  
  void printStatusOfClones();
  
  int * getMaskedByNumbers(); // get pointer to dynamic array of 
  
  int * getDeadByNumbers();
  
  int * getMaskedByNumbersFromClone(const int & cloneNum);
  
  int * getDeadByNumbersFromClone(const int & cloneNum);
  
  // 
  
  virtual void WriteResults( std::ofstream & theStream);
  
  const int & getClones(); // obsolete , see getNumberOfClones , more appropriate method name
  
  const int & getNumberOfClones();
  
  void setNumberOfClones(const int & number);
  
  void setRawIDofClone(const int & cloneNum,const int & RawID);
  
  string getFullOnlineRollID ();
  
  std::string getRollIDofClone_withEtaPartSeparated(const int&); // to be improved, a whole vector 
  
  std::string getRollIDofClone(const int & cloneNum);
  
  std::string getRollIDofCloneWithNewIdentifiers(const int & cloneNum);
  
  std::string getRollIDofCloneWithConstructionDBidentifiers(const int & cloneNum);
  
  const std::string & getRollPartOfClone(const int & cloneNum);
  
  const std::string & getRollOfflineIDofClone(const int &);
  
  bool isInEndcap();
  
  bool isInBarrel();
  
  virtual void Reset();
  
};


#endif