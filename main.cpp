/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 10.08.13
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include "UserApplications.h" // it includes all headers anyway, even occi


int main(int argc,char ** argv){
  
  //currentTests(argc,argv);
  //testDBconnection(argc,argv);
  //clusterObjectTester(argc,argv);
  //firstCompleteTestApplication(argc,argv);
  //timeEvolutionStudy(argc,argv);
  
  localEfficiencyStudy(argc,argv);
  
  //thread second (testDBconnection,argc,argv);
  //converterTests(argc,argv);
  //configObjectsTest(argc,argv);  
  //getGlobalPlotsForRunStack(argc,argv);  
  //getDistributionOfEventsByClustersInRefChambers(argc,argv);
    
  return 0;
}
