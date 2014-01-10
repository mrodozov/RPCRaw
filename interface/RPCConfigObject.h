/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 18.12.13
*/

#ifndef RPC_CONFIG_OBJ_H
#define RPC_CONFIG_OBJ_H

#include "RPCDBReader.h"
#include "RPCChamberConditions.h"
#include <json/json.h>

class RPCAbstractRunConfig {
  
protected:
  
  int _runNumber;
  string _triggerLayer;
  double _humidity;
  double _pressure;
  double _temperature;
  int _numberOfEvents;
  vector<RPCChamberConditionsBase*> _chambersDetails;
  
public:
  
  virtual void readConfigurationFromFile(const string & fileName)=0;
  
  virtual void setRunNumber(const int & runNumber)=0;
  virtual const int & getRunNumber()=0;
  virtual void setTriggerLayer(const string & layer)=0;
  virtual const string & getTriggerLayer()=0;
  virtual void setHumidity(const double & humidity)=0;
  virtual const double & getHumidity()=0;
  virtual void setPressure(const double & pressure)=0;
  virtual const double & getPressure()=0;
  virtual void setTemperature(const double & temperature)=0;
  virtual const double & getTemperature()=0;
  virtual const vector<RPCChamberConditionsBase*> & getChambersDetails()=0;
  virtual void setChamberDetails(const vector<RPCChamberConditionsBase*> & chamberDetails)=0;
  virtual void setNumberOfEvents(const int & nevents)=0;
  virtual const int & getNumberOfEvents()=0;
  
  virtual ~RPCAbstractRunConfig();
  RPCAbstractRunConfig();
  
};

class RPCRunConfig : public RPCAbstractRunConfig {
  
  RPCDBReader * _dbReader;
  
public:
  
  void setRunNumber(const int & runNumber);
  const int & getRunNumber();
  void setTriggerLayer(const string & layer);
  const string & getTriggerLayer();
  void setHumidity(const double & humidity);
  const double & getHumidity();
  void setPressure(const double & pressure);
  const double & getPressure();
  void setTemperature(const double & temperature);
  const double & getTemperature();
  const vector<RPCChamberConditionsBase*> & getChambersDetails();
  void setChamberDetails(const vector<RPCChamberConditionsBase*> & chamberDetails);
  void setNumberOfEvents(const int & nevents){ this->_numberOfEvents = nevents;}
  const int & getNumberOfEvents(){ return this->_numberOfEvents;}
  
  RPCDBReader * getDBreader();
  
  RPCChamberConditionsBase * getBasicConditionsForChamber(const int & chamberNum);
  virtual RPCChamberConditions * getConcreteConditionsForChamber(const int & chamberNum); // overload the return type if new class is derived from RPCChamberConditions 
  virtual void readConfigurationFromFile(const string & fileName); // overload if change of implementation is needed
  virtual void readConfigurationFromDBforRunAndSite(const int & run,const string & site); // overload if change of implementation is needed
  virtual void readConfigurationFromJSONDocument(const string & jsonDocument); // overload if change of implementation is needed
  
  vector<int> getVectorOfReferenceChambers();
  
  RPCRunConfig();
  ~RPCRunConfig();
};

#endif