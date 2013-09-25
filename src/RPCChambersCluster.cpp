/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCChambersCluster.h"
#include "../interface/RPCRawTypes.h"

using namespace std;

void RPCChambersCluster::createNewClusterOfChambersWithRE4type(const int & nChambers,const string & RE4type){
  if (this->_numberOfChambers){ // not ok, some chambers are already created , one has first to erase them
    cout << "The object still holds existing chamber objects ! use deleteAllChambers method to clear them first." << endl;
  }
  else { // its ok to proceed
    //this->_chambersPointer = new RPCLinkBoard[nChambers];
    for (int i = 0 ; i < nChambers;i++){
      RPCLinkBoard * aBoard = new RPCLinkBoard(RE4type);
      aBoard->allocAndInit();
      this->_chambersPointer[i] = aBoard;
    }
    this->_numberOfChambers = nChambers;
  }
}

void RPCChambersCluster::createNewClusterOfTriggerObjects(const int & nTriggerObjects){
  if (this->getNumberOfTriggerObjects()){
    cout << "The object still holds existing chamber objects ! use deleteAllTriggers method to clear them first." << endl;
  }
  else {
    for(int i = 0 ; i < nTriggerObjects ; i++){
      RPCLinkBoard * aTriggerObject = new RPCLinkBoard(kRPC_RE_4_2_chamber); // defined in raw types file, take a look
      aTriggerObject->allocAndInit();
      aTriggerObject->setIsTriggerChamber(true);
      this->_triggerObjectsPointer[i] = aTriggerObject;
    }
    this->_numberOfTriggers = nTriggerObjects;
  }
}

void RPCChambersCluster::setDataSourceForNchambers(const int & nChambers,const vector<vector<vector< unsigned> > > & dataSource){
  
  for (int i = 0 ; i < nChambers ; i ++){
    this->getChamberNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }
}

void RPCChambersCluster::setDataSourceForNtriggerObjects(const int & nTriggerObject,const vector<vector<vector< unsigned> > > & dataSource){
  
  for (int i = 0 ; i < nTriggerObject ; i++ ){
    this->getTriggerObjectNumber(i+1)->setStripsHitsDataFromSource(dataSource.at(i));
  }  
}

RPCChambersCluster::RPCChambersCluster (){
  this->_numberOfChambers = 0;
  this->_numberOfTriggers = 0;
}

RPCChambersCluster::RPCChambersCluster (const int & numberOfChambers,const int & numberOfTriggerObjs,const string & RE4TypeOfChambers){
  //RPCChambersCluster(); // destructor is called if the default constructor is used as normal method
  this->_numberOfChambers = 0;
  this->_numberOfTriggers = 0;
  this->createNewClusterOfChambersWithRE4type(numberOfChambers,RE4TypeOfChambers);
  this->createNewClusterOfTriggerObjects(numberOfTriggerObjs);
  
}

RPCChambersCluster::~RPCChambersCluster () {
  //cout << " destructor called " << endl;
  this->deleteAllObjects();
  // clear other resources here
  
}

void RPCChambersCluster::deleteAllChambers () {
  if (this->getNumberOfChambers() > 0){
    for (unsigned i=0;i < this->getNumberOfChambers();i++) {
      delete this->_chambersPointer.at(i);
    }
  }
  this->_chambersPointer.clear();
  this->_numberOfChambers = 0;
}

void RPCChambersCluster::deleteAllTriggerObjects(){
  if (this->getNumberOfTriggerObjects() > 0){
    RPCLinkBoard * pointer;
    for (int i=0; i < this->getNumberOfTriggerObjects();i++){
      delete this->_triggerObjectsPointer.at(i);
    }
  }
  this->_triggerObjectsPointer.clear();
  this->_numberOfTriggers = 0;
}

void RPCChambersCluster::deleteAllObjects(){
  this->deleteAllChambers();
  this->deleteAllTriggerObjects();
}

RPCLinkBoard * RPCChambersCluster::getChamberNumber(const int & chamberNumber){
  return dynamic_cast<RPCLinkBoard*>(this->_chambersPointer.at(chamberNumber-1));
}

RPCLinkBoard * RPCChambersCluster::getTriggerObjectNumber(const int & triggerObjectNumber){
  return  dynamic_cast<RPCLinkBoard*>(this->_triggerObjectsPointer.at(triggerObjectNumber-1));
}

/** analisys methods goes here, define some concrete logic that extract data from the events */

void RPCChambersCluster::findTrackInCurrentEvent(TFile * fileToSave){
  
  // Save file is used to save all the reconstructed graphs in order to inspect them
  
  RPCLinkBoard * currentChamberObj;
  RPCLinkBoard * triggerObj;
  RPCLinkBoardChannel * currentChannelObj;
  int numberOfChambers = this->getNumberOfChambers();
  triggerObj = this->getTriggerObjectNumber(1);
  TGraphErrors * graphToFit;
  currentChamberObj = this->getChamberNumber(1);
  
  // use the first hit of the scintilators as time reference 
  int timeReference = 0;
  int timeWindow = 200;
  
  for (int i = 0 ; i < 32 ; i++){
    if(triggerObj->getChannel(i+1)->hasHit()){
      timeReference = triggerObj->getChannel(i+1)->getHits().at(0);
      break;
    }
  }
  
  cout << " time reference : " << timeReference << endl;
  
  for (int totalChambers = 0 ; totalChambers < numberOfChambers ; totalChambers++){
    
    currentChamberObj = this->getChamberNumber(totalChambers+1);
    
    for (int j=0 ; j < 96 ;j++){

      currentChamberObj = this->getChamberNumber(totalChambers+1);
      currentChannelObj = currentChamberObj->getChannel(j+1);
      if (currentChannelObj->hasHit()){
	
	cout << " chamber " << totalChambers+1 << " channel " << currentChannelObj->getOnlineNumber() << " time " << currentChannelObj->getHits().at(0);
	
      }
    }
    cout << endl;
    currentChamberObj->findAllClustersForTiggerTimeReferenceAndTimeWindow(timeReference,timeWindow);
    
  }
  cout << endl;
  
}

void RPCChambersCluster::recursiveLoopOnAllChambers(int chambersTotal){
  
}
