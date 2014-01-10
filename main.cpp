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
#include <thread>
#include "UserApplications.h" // it includes all headers anyway, even occi
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

typedef basic_ptree< std::string, std::string > ptree;

int main(int argc,char ** argv){
  
  //currentTests(argc,argv);
  //testDBconnection(argc,argv);
  //clusterObjectTester(argc,argv);
  //firstCompleteTestApplication(argc,argv);
  //std::thread first (timeEvolutionStudy,argc,argv);
  //thread first ( localEfficiencyStudy,argc,argv);
  //thread second (testDBconnection,argc,argv);
  //converterTests(argc,argv);
  //configObjectsTest(argc,argv);
  
  //first.join();  
  
  
  
  return 0;
}
