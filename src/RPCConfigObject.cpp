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
  
  this->_dbReader->openNewConnection();
  this->_dbReader->getEnvironmentDataForRunAndSite(run,site);
  this->_dbReader->getCurrentResultSet()->next();
  this->setHumidity(this->_dbReader->getCurrentResultSet()->getDouble(3));
  this->setTemperature(this->_dbReader->getCurrentResultSet()->getDouble(1));
  this->setPressure(this->_dbReader->getCurrentResultSet()->getDouble(2));
  this->setRunNumber(run);
  
  vector<string> gapTypes;
  gapTypes.push_back("topnarrow");
  gapTypes.push_back("topwide");
  gapTypes.push_back("bottom");  
  this->_dbReader->getChambersDataForRunAndSite(run,site);
  
  RPCChamberConditions * pointerToGhentRefChambers[2];
  
  if (site == "Ghent"){
    // create two conditions for chambers that are reference 
    
    RPCChamberConditions * reference1 = new RPCChamberConditions;
    RPCChamberConditions * reference2 = new RPCChamberConditions;
    pointerToGhentRefChambers[0] = reference1;
    pointerToGhentRefChambers[1] = reference2;
    reference1->setShelfNumber(1);    
    reference2->setShelfNumber(5);
    vector<unsigned> refChambers ;
    refChambers.push_back(1);
    refChambers.push_back(5);
    this->setReferenceChambers(refChambers);
    reference1->setChamberName("GhentReferenceChamberOne");
    reference2->setChamberName("GhentReferenceChamberTwo");
    vector<double> gapCurrents;
    vector<int> gapVsets;
    vector<int> gapVmons;
    vector<int> febsValues;    
    
    for (int i = 0 ; i < 3 ; i++){
      gapCurrents.push_back(0.1);
      gapVmons.push_back(9950);
      gapVsets.push_back(9950);      
    }
    
    for (int i = 0 ; i < 12 ; i++){
      febsValues.push_back(215);
    }
    
    for (int i = 0 ; i < 2 ; i++){
      pointerToGhentRefChambers[i]->setGapLabels(gapTypes);
      pointerToGhentRefChambers[i]->setIsReference(true);
      pointerToGhentRefChambers[i]->setTDCcable(0);
      pointerToGhentRefChambers[i]->setLVcable(0);
      pointerToGhentRefChambers[i]->setHVcable(0);
      pointerToGhentRefChambers[i]->setFEBTresholds(febsValues);
      pointerToGhentRefChambers[i]->setGapsCurrent(gapCurrents);
      pointerToGhentRefChambers[i]->setGapsVmon(gapVmons);
      pointerToGhentRefChambers[i]->setGapsVset(gapVsets);
      pointerToGhentRefChambers[i]->setHVmon(9950);
      pointerToGhentRefChambers[i]->setHVset(9950);
    }
    this->_chambersDetails.push_back(reference1);
  }
  
  while(this->_dbReader->getCurrentResultSet()->next()){
    RPCChamberConditions * conditions = new RPCChamberConditions;
    conditions->setIsReference(false);
    conditions->setChamberName(this->_dbReader->getCurrentResultSet()->getString(1));
    this->setTriggerLayer(this->_dbReader->getCurrentResultSet()->getString(3));
    // no records for the cables order in the DB, sorry
    conditions->setLVcable(0);
    conditions->setHVcable(0);
    conditions->setTDCcable(0);
    //
    conditions->setShelfNumber(this->_dbReader->getCurrentResultSet()->getInt(23));
    conditions->setHVset(this->_dbReader->getCurrentResultSet()->getInt(4));
    conditions->setHVmon(this->_dbReader->getCurrentResultSet()->getInt(5));
    int vset_const = this->_dbReader->getCurrentResultSet()->getInt(4); // its always the same value, even for CERN chambers its the Vset
    
    vector<double> gapCurrents;
    vector<int> gapVsets;
    vector<int> gapVmons;
    for (int i = 0 ; i < 3 ; i++){
      gapVsets.push_back(vset_const);
      gapVmons.push_back(this->_dbReader->getCurrentResultSet()->getInt(5+i));
      gapCurrents.push_back(this->_dbReader->getCurrentResultSet()->getDouble(8+i));      
      
    }
    vector<int> febsValues;
    for(int i = 0 ; i < 12 ; i++){
      febsValues.push_back(this->_dbReader->getCurrentResultSet()->getInt(11+i));
    }
    
    conditions->setFEBTresholds(febsValues);
    conditions->setGapsVset(gapVsets);
    conditions->setGapLabels(gapTypes);
    conditions->setGapsVmon(gapVmons);
    conditions->setGapsCurrent(gapCurrents);
    this->_chambersDetails.push_back(conditions);
  }
  if(site == "Ghent"){
    this->_chambersDetails.push_back(pointerToGhentRefChambers[1]);
  }
  
  this->_dbReader->closeCurrentConnection();
  
}

void RPCRunConfig::readConfigurationFromJSONDocument(const string & jsonDocument,const string & runToUse){
  
  // clear previous chamber config objects if any
  if (this->getChambersDetails().size()){
    for (int details = 0 ; details < this->getChambersDetails().size();details++){
      delete this->_chambersDetails.at(details);
    }
    this->_chambersDetails.clear();
  }
  
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
    vector<unsigned> refChambers;
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

vector<string> RPCRunConfig::getRunListFromJSONfile(const string & jsonFile){
  stringstream ss;
  ifstream filestream;
  boost::property_tree::ptree pt;
  filestream.open(jsonFile.c_str());
  string content,line;
  vector<string> runlist;
  if (filestream.is_open()){
    while (getline(filestream,line)){
      
      ss << line;
      boost::property_tree::read_json(ss,pt);
      ss.clear();
      runlist.push_back(pt.begin()->first);
      filestream.clear();
      
    }
  }
  filestream.close();
  
  return runlist;
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


