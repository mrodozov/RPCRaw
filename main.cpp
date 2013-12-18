/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 10.08.13
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <thread>
//#include <boost/thread.hpp> // install on MacOSX, it exists on Linux
#include "UserApplications.h" // it includes all headers anyway, even occi

using namespace std;


int main(int argc,char ** argv){
  
  //currentTests(argc,argv);
  //testDBconnection(argc,argv);
  //clusterObjectTester(argc,argv);
  //firstCompleteTestApplication(argc,argv);
  //timeEvolutionStudy(argc,argv);
  //localEfficiencyStudy(argc,argv);
  //converterTests(argc,argv);
  
  RPCLinkBoard * aBoard = new RPCLinkBoard(kRPC_RE_4_2_chamber);
  aBoard->allocAndInit();
  vector<double> vmons;
  
  vmons.push_back(0.5);
  vmons.push_back(0.6);
  vmons.push_back(0.7);
  
  
  RPCChamberConditions * conditions = new RPCChamberConditions();
  aBoard->setCurrentRunDetails(conditions);  
  aBoard->getBasicChamberConditions()->setGapsVmon(vmons);
  
  cout << aBoard->getBasicChamberConditions()->getGapsVmon().at(2) << endl;
  cout << aBoard->getExtendedChamberConditions()->getHVmonForGap(3) << endl;
  
  delete aBoard;
  
  return 0;
}
