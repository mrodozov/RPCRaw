/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 06.08.13
*/

#ifndef USER_APPLICATIONS_H
#define USER_APPLICATIONS_H
#include <iomanip>
#include "interface/RPCChambersCluster.h"
#include "interface/RPCRawConverter.h"
#include "interface/RPCConfigObject.h"

void currentTests(int _argc,char * arguments[]);// substitution of the main function. use this for tests

void testDBconnection(int _argc,char * arguments[]); // make the test for the db connection

void clusterObjectTester (int _argc,char * _argv[]);

void firstCompleteTestApplication(int _argc,char * _argv[]);

void timeEvolutionStudy(int _argc,char * _argv[]);

void localEfficiencyStudy(int _argc,char * _argv[]);

void converterTests(int _argc,char * _argv[]);

void configObjectsTest(int _argc,char * _argv[]);

void getGlobalPlotsForRunStack(int _argc,char * _argv[]); // mostly for CERN files

void getDistributionOfEventsByClustersInRefChambers(int _argc,char * _argv[]);

void fitSigmoidHistogram(); // fit existing sigmoid 

#endif