/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 06.08.13
*/

#include "UserApplications.h"
#include <occi.h>
#include <json/json.h>

using namespace std;
using namespace oracle::occi;

void currentTests(int _argc,char * arguments[]){
  
  string ghentfilename = arguments[1];
  TFile * ghentFile = new TFile(ghentfilename.c_str(),"READ","in");
  
  RPCRawConverter * converter = new RPCRawConverter();
  converter->setCurrentFileType(converter->getFileTypeForFile(ghentFile));
  converter->setCurrentFile(ghentFile);
  converter->initialyzeStructure();
  converter->setGhentTDCtoRPCmap(arguments[2]);
  
  RPCLinkBoard * aBoard = new RPCLinkBoard(kRPC_RE_4_2_chamber);
  RPCLinkBoard * firstChamber, * secondChamber;
  firstChamber = new RPCLinkBoard(kRPC_RE_4_2_chamber);
  secondChamber = new RPCLinkBoard(kRPC_RE_4_2_chamber);
  firstChamber->allocAndInit();
  secondChamber->allocAndInit();
  
  aBoard->allocAndInit();
  aBoard->setStripsHitsDataFromSource(converter->getTriggerVector(1));
  
  firstChamber->setStripsHitsDataFromSource(converter->getChamberVector(1));
  secondChamber->setStripsHitsDataFromSource(converter->getChamberVector(2));
  
  converter->nextEvent();
  converter->nextEvent();
  
  for (int i = 0 ; i < 32 ; i++){
    if (aBoard->getStrip(i+1)->hasHit()){
      cout << i << " " << aBoard->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  for (int i = 0 ; i < 96 ; i++){
    
    if (firstChamber->getStrip(i+1)->hasHit()){
      cout << i << " first: " << firstChamber->getStrip(i+1)->getHits().at(0) << endl;
    }    
    if (secondChamber->getStrip(i+1)->hasHit()){
      cout << i << " second: " << secondChamber->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  
  converter->nextEvent();
  for (int i = 0 ; i < 32 ; i++){
    if (aBoard->getStrip(i+1)->hasHit()){
      cout << i << " " << aBoard->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  for (int i = 0 ; i < 96 ; i++){
    
    if (firstChamber->getStrip(i+1)->hasHit()){
      cout << i << " first: " << firstChamber->getStrip(i+1)->getHits().at(0) << endl;
    }    
    if (secondChamber->getStrip(i+1)->hasHit()){
      cout << i << " second: " << secondChamber->getStrip(i+1)->getHits().at(0) << endl;
    }
  }
  //while (converter->nextEvent()) { cout << converter->getEventNumber() << endl; }
  
}

void testDBconnection(int _argc, char* arguments[]){
  
  /** data base object tests */
  
  RPCDBReader * reader = new RPCDBReader;
  
  reader->openNewConnection();
  reader->closeCurrentConnection(); // just test does it work
  reader->openNewConnection();
  
  reader->getChambersDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(8);
  
  reader->getChambersDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(8);
  
  reader->getEnvironmentDataForRunAndSite(1163,"Ghent");
  
  reader->printResultWithNcolumns(3);
  
  reader->getEnvironmentDataForRunAndSite(2499,"CERN");
  
  reader->printResultWithNcolumns(3);
  
  reader->closeCurrentConnection();
  
    
    
}

void clusterObjectTester (int _argc,char * _argv[]){
  
  /* memory test
  
  for (int i = 0 ; i < 100000000 ; i++){
    RPCLinkBoard * aBoard = new RPCLinkBoard(kRPC_RE_4_3_chamber);
    aBoard->allocAndInit();
    cout << i << endl;
    delete aBoard;
    
  }
  // memory leak test, its ok not leaking. also checked with valgrind
  */
  
  RPCChambersCluster * aCluster = new RPCChambersCluster;
  aCluster->createNewClusterOfChambersWithRE4type(5,kRPC_RE_4_2_chamber);
  aCluster->deleteAllChambers();
  aCluster->createNewClusterOfChambersWithRE4type(10,kRPC_RE_4_3_chamber);
  // now try to create before erase the previous
  aCluster->createNewClusterOfChambersWithRE4type(3,kRPC_RE_4_3_chamber);
  // now print the ammount
  cout << aCluster->getNumberOfChambers() << endl;
  // delete
  aCluster->deleteAllObjects();
  cout << aCluster->getNumberOfChambers() << endl;
  
}

void firstCompleteTestApplication(int arg_c,char * arg_v[]){
  
  string filename=arg_v[1],ghentMap = arg_v[2];
  TFile * cernfile = new TFile(filename.c_str());
  RPCRawConverter * converter = new RPCRawConverter(cernfile);
  converter->setGhentTDCtoRPCmap(ghentMap);
  int numberOfChambersNeeded = converter->getNumberOfChamberObjects();
  int numberOfTriggerObjsNeeded = converter->getNumberOfTriggerObjects();
  RPCChambersCluster * aCluster = new RPCChambersCluster(numberOfChambersNeeded,numberOfTriggerObjsNeeded,kRPC_RE_4_2_chamber);
  aCluster->setDataSourceForNchambers(numberOfChambersNeeded,converter->getChambersData());
  aCluster->setDataSourceForNtriggerObjects(numberOfTriggerObjsNeeded,converter->getTriggersData());
  RPCLinkBoard * singleLBforTest;
  RPCLinkBoardChannel * singleChannel;
  RPCLinkBoard * triggerObject;
  triggerObject = aCluster->getTriggerObjectNumber(1);
  TGraphErrors * trackGraph;
  
  TFile * aFile; // just dont do nothing now
  
  //for( int i = 0 ; i < 1000 ; i++){  
  while(converter->nextEvent()){
  //converter->nextEvent();
    cout << " event: " << converter->getEventNumber() << ", chambers data: ";
    aCluster->findTrackInCurrentEvent(aFile);
    
  }
  
  aCluster->deleteAllObjects();
  
  cernfile->Close("R");
  cernfile->Delete();
  
  delete aCluster;
  delete converter;
  
  
}

int testRecursion(int n, int m){
  int returnValue;
  if ((n == 1) || (m == 0) || (n == m))
  returnValue = 1;
  else
  returnValue = testRecursion(n-1,m) + testRecursion(n-1,m-1);
  return returnValue;
  
}

