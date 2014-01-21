/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 18.12.13
*/

#include "../interface/RPCConfigObject.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <assert.h>

using namespace std; using boost::property_tree::ptree;

RPCAbstractRunConfig::RPCAbstractRunConfig(){
  
}

RPCAbstractRunConfig::~RPCAbstractRunConfig(){
  
}

void RPCRunConfig::readConfigurationFromFile(const string & fileName) {
  
}

void RPCRunConfig::readConfigurationFromDBforRunAndSite(const int & run,const string & site){
  
}

void RPCRunConfig::readConfigurationFromJSONDocument(const string & jsonDocument,const string & runToUse){
  
  stringstream ss;
  ifstream filestream;
  filestream.open(jsonDocument.c_str());
  string content,line ;
  vector<string> lines;
  if (filestream.is_open()){
    while (getline(filestream,line)){
      lines.push_back(line);
      filestream.clear();
    }
  }
  
  else{
    cout << "cannot open file " << jsonDocument << endl;
  }
  filestream.close();
  
  ptree atree;
  
  string runDescription;
  // use single run passed as argument, if exists
  // find the line for the run number and push it as single element

  for (int counter = 0;counter < lines.size();counter++){
    ss << lines.at(counter);
    boost::property_tree::read_json(ss,atree);
    ss.clear();
    if (atree.find(runToUse) != atree.not_found()){
      runDescription = lines.at(counter);
      break;
    }
      if (atree.find(runToUse) == atree.not_found() && counter == lines.size() - 1){
	cout << "run to use" << runToUse << " not found, check the json file or does the run exists " << endl;
	break;
      }
  }
  
  try{
    
    ss << runDescription;
    ptree pt,chambersDetails,chambers,singleChamber,dictionaryChamberDetails;
    boost::property_tree::read_json(ss,pt);
    ss.clear();
    boost::property_tree::basic_ptree<std::string,std::string>::const_iterator iter,secondIter,thirdIter,refChambersIter;
    chambersDetails = pt.begin()->second;
    this->setTemperature(boost::lexical_cast<double>(chambersDetails.get_child("temperature").data()));
    this->setPressure(boost::lexical_cast<double>(chambersDetails.get_child("preasure").data()));
    this->setHumidity(boost::lexical_cast<double>(chambersDetails.get_child("humidity").data()));
    this->setTriggerLayer(chambersDetails.get_child("gapsMode").data());
    this->setNumberOfEvents(boost::lexical_cast<int>(chambersDetails.get_child("events").data()));
    this->setRunNumber(boost::lexical_cast<int>(pt.begin()->first)); // should match runToUse argument
    vector<int> refChambers;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,chambersDetails.get_child("referenceChambers")){
      refChambers.push_back(boost::lexical_cast<int>(v.second.data()));
    }
    this->setReferenceChambers(refChambers);
    
    int chamberCounter = 1;
    
    for(iter = pt.begin();iter != pt.end();++iter){
      string runNum = iter->first;
      //cout << iter->first << endl;
      chambersDetails = iter->second;
      chambers = chambersDetails.get_child("chambers");
      vector<string> gapTypes;
      gapTypes.push_back("topnarrow");
      gapTypes.push_back("topwide");
      gapTypes.push_back("bottom");
      
      for (thirdIter = chambers.begin();thirdIter != chambers.end() ; thirdIter++){
	singleChamber = thirdIter->second;
	dictionaryChamberDetails = singleChamber.get_child("bottom");
	RPCChamberConditions * conditions = new RPCChamberConditions;
	conditions->setShelfNumber(boost::lexical_cast<int>(singleChamber.get_child("shelf").data()));
	conditions->setChamberName(singleChamber.get_child("name").data());
	
	conditions->setIsReference(  find(this->getReferenceChambers().begin(),this->getReferenceChambers().end(),conditions->getShelfNumber()) != this->getReferenceChambers().end() );
	if (conditions->getIsReference()){
	  conditions->setHVset(boost::lexical_cast<int>(singleChamber.get_child("bottom.vset").data()));
	}
	else{
	  conditions->setHVset(boost::lexical_cast<int>(chambersDetails.get_child("HVApplied").data()));
	}
	conditions->setHVcable(boost::lexical_cast<int>(singleChamber.get_child("HVcable").data()));
	conditions->setLVcable(boost::lexical_cast<int>(singleChamber.get_child("LV").data()));
	conditions->setTDCcable(boost::lexical_cast<int>(singleChamber.get_child("TDCcable").data()));
	vector<string> gapLabels;
	vector<double> gapCurrents;
	vector<int> gapVsets;
	vector<int> gapVmons;
	for (int gapNum = 0 ; gapNum < gapTypes.size();gapNum++){
	  
	  string gapLabel = gapTypes.at(gapNum);
	  gapLabels.push_back(gapLabel);
	  gapCurrents.push_back(boost::lexical_cast<double>(singleChamber.get_child((gapLabel+".current")).data()));
	  gapVsets.push_back(boost::lexical_cast<int>(singleChamber.get_child((gapLabel+".vset")).data()));
	  gapVmons.push_back(boost::lexical_cast<int>(singleChamber.get_child((gapLabel+".vmon")).data()));
	  
	}
	vector<int> febsValues;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,singleChamber.get_child("FEBS")){
	  febsValues.push_back(boost::lexical_cast<int>(v.second.data()));
	}
	conditions->setGapLabels(gapLabels);
	conditions->setGapsCurrent(gapCurrents);
	conditions->setGapsVmon(gapVmons);
	conditions->setGapsVset(gapVsets);
	conditions->setFEBTresholds(febsValues);
	chamberCounter++;
	this->_chambersDetails.push_back(conditions);
      }
      
    }
   
  }
  catch (std::exception const& e)
  {
        std::cerr << e.what() << std::endl;
  }
  
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


