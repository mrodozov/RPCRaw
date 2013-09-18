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


int main(int argc,char * argv[]){
  
  //currentTests(argc,argv);
  //testDBconnection(argc,argv);
  //clusterObjectTester(argc,argv);
  firstCompleteTestApplication(argc,argv);
  
  return 0;
}
