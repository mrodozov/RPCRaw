/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 18.12.13
*/

#include "../interface/RPCConfigObject.h"

using namespace std;

RPCAbstractRunConfig::RPCAbstractRunConfig(){
  
}

RPCAbstractRunConfig::~RPCAbstractRunConfig(){
  
}

void RPCRunConfig::readConfigurationFromFile(const string & fileName) {
  
}

void RPCRunConfig::readConfigurationFromDBforRunAndSite(const int & run,const string & site){
  
}

void RPCRunConfig::readConfigurationFromJSONDocument(const string & jsonDocument){
  //Json::Reader jsonReader;
  //Json::Value resultValue;
  //jsonReader.parse(jsonDocument,resultValue);
  
}

RPCChamberConditionsBase * RPCRunConfig::getBasicConditionsForChamber(const int & chamberNum) { 
  return this->_chambersDetails.at(chamberNum-1);
} 

RPCChamberConditions * RPCRunConfig::getConcreteConditionsForChamber(const int & chamberNum){
  return dynamic_cast<RPCChamberConditions*>(this->_chambersDetails.at(chamberNum-1));
}

RPCRunConfig::RPCRunConfig(){
  this->_dbReader = new RPCDBReader();
}

RPCRunConfig::~RPCRunConfig(){
  delete this->_dbReader;
}

void RPCRunConfig::setRunNumber(const int & runNumber){
  this->_runNumber = runNumber;
}

const int & RPCRunConfig::getRunNumber(){
  return this->_runNumber;
}

void RPCRunConfig::setTriggerLayer(const string & layer){
  this->_triggerLayer = layer;
}

const string & RPCRunConfig::getTriggerLayer(){
  return this->_triggerLayer;
}

void RPCRunConfig::setHumidity(const double & humidity){
  this->_humidity = humidity;
}

const double & RPCRunConfig::getHumidity(){
  return this->_humidity;
}

void RPCRunConfig::setPressure(const double & pressure){
  this->_pressure = pressure;
}

const double & RPCRunConfig::getPressure(){
  return this->_pressure;
}

void RPCRunConfig::setTemperature(const double & temperature){
  this->_temperature = temperature;
}

const double & RPCRunConfig::getTemperature(){
  return this->_temperature;
}

const vector<RPCChamberConditionsBase*> & RPCRunConfig::getChambersDetails(){
  return this->_chambersDetails;
}

void RPCRunConfig::setChamberDetails(const vector<RPCChamberConditionsBase*> & chamberDetails){
  this->_chambersDetails = chamberDetails;
}

RPCDBReader * RPCRunConfig::getDBreader(){
  return this->_dbReader;
}

vector<int> RPCRunConfig::getVectorOfReferenceChambers(){
  vector<int> referenceChambers;
  for(int i=0;i < this->getChambersDetails().size() ; i++){
    if(this->getBasicConditionsForChamber(i+1)->getIsReference()){
      referenceChambers.push_back(i+1);
    }
  }
  return referenceChambers;
}

