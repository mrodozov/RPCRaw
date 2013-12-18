#ifndef RPC_CONFIG_OBJ_H
#define RPC_CONFIG_OBJ_H

#include "RPCDBReader.h"
#include "RPCChamberConditions.h"

class RPCAbstractRunConfig {
  
public:
  
  virtual void readConfigurationFromFile(const string & fileName)=0;
  // configuration from DB as well
  
};

class RPCRunConfig : public RPCAbstractRunConfig {
  
  RPCDBReader * _dbReader;
  int _runNumber;
  string _triggerLayer; // better introduce constants
  double _humidity;
  double _pressure;
  double _temperature;
  vector<RPCChamberConditions*> _chambersDetails;
  virtual void readConfigurationFromFile(const string & fileName);
  
protected:  
  
public:
  
  RPCChamberConditions * getConditionsObjectForChamber(const int & chamberNum) { return this->_chambersDetails.at(chamberNum-1);} 
  
};

#endif