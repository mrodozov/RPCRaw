/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 18.12.13
*/

/** Adaptable class to derive (mostly) single objects from. The class is helper for reading databases, performing concrete queries, implement formats reading (JSON, txt, csv)
 * creating, configuring and hooking chamber condition objects with the chamber objects, and storing global run variables (temperature, humidity etc).
 * Run details concerning the environment and the chambers are handled mostly from here. 
 */

#ifndef RPC_CONFIG_OBJ_H
#define RPC_CONFIG_OBJ_H

#include "RPCDBReader.h"
#include "RPCChamberConditions.h"


class RPCAbstractRunConfig {
  
protected:
  
  int _runNumber;
  string _triggerLayer;
  double _humidity;
  double _pressure;
  double _temperature;
  int _numberOfEvents;
  int _hvOnTestChambers;
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
  vector<unsigned> referenceChambers;
  
public:
  
  void setReferenceChambers(const vector<unsigned> & referenceChambersVector){ this->referenceChambers = referenceChambersVector;}
  const vector<unsigned> & getReferenceChambers(){ return this->referenceChambers;}
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
  virtual void readConfigurationFromJSONDocument(const string & jsonDocument,const string & runToUse ); // Get configuration for a run from a JSON file, overload if change of implementation is needed. currently using boost property list
  vector<string> getRunListFromJSONfile(const string & jsonFile); // Get runlist as vector of runs from the JSON file. CERN cases only
  
  RPCRunConfig();
  ~RPCRunConfig();
};

#endif