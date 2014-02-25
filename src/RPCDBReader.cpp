/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 10.08.13
*/

#include <iostream>
#include <sstream>
#include <assert.h>
#include "../interface/RPCDBReader.h"
#include <occi.h>

using namespace std;
using namespace oracle::occi;

void RPCDBReader::openNewConnection(){
  this->_environment = Environment::createEnvironment();
  
  try {
    this->_connection = this->_environment->createConnection(kRPCDB_username,kRPCDB_password,kRPCDB_connectionString);
  }
  catch(oracle::occi::SQLException & sqlexcptn){
    cout << " connection failed with message: " << endl;
    cerr << sqlexcptn.getMessage() << endl;
  }
}

void RPCDBReader::closeCurrentConnection(){
  // terminate the connection and the environment
  this->_environment->terminateConnection(this->_connection);
  Environment::terminateEnvironment(this->_environment);
}

void RPCDBReader::executeQueryWithSQLstatement(const string & SQLStatement){
  
  Statement * stmnt;
  try{
    stmnt = this->getCurrentConnection()->createStatement(SQLStatement.c_str());
    this->_resultSet = stmnt->executeQuery();
  }
  catch(oracle::occi::SQLException & sqlexcptn){
    cout << " general statement query failed with message:" << endl;
    cerr <<  sqlexcptn.getMessage() << endl;
  }
}

ResultSet * RPCDBReader::getCurrentResultSet () {
  return this->_resultSet;
}

void RPCDBReader::getChambersDataForRunAndSite(const unsigned & run,const string & site){
  
  string queryString ; // make some magic here ;) 
  //queryString = "select r.CHAMBER_NAME, r.CHAMBER_TYPE, c.ETA_PARTITION, c.FLAG, c.LAYER, c.HV, c.CURRENT_B, c.CURRENT_TN, c.CURRENT_TW  from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and ";
  queryString = "select distinct r.CHAMBER_NAME, r.CHAMBER_TYPE, c.LAYER, c.HV, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.CURRENT_TN, c.CURRENT_TW, c.CURRENT_B ,c.VTH1_A, c.VTH2_A,c.VTH3_A, c.VTH4_A,c.VTH1_B, c.VTH2_B,c.VTH3_B, c.VTH4_B,c.VTH1_C, c.VTH2_C,c.VTH3_C, c.VTH4_C , c.STATION from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and ";
  queryString = RPCDBReader::concatenateQueryStringWithRunAndSite(queryString,run,site);
  queryString += " order by c.STATION";
  cout << queryString << endl;
  this->executeQueryWithSQLstatement(queryString.c_str());
  
  //this->getCurrentConnection()->terminateStatement(stmnt);the result set depends on the statement ? // this one causes SQLException
  
}

void RPCDBReader::getEnvironmentDataForRunAndSite(const unsigned & run,const string & site){
  
  string queryString ; // make some magic here ;) 
  queryString = "select DISTINCT TEMPERATURE, PRESSURE, HUMIDITY from CMS_RPC_CONSTR.RE4_COSMIC_TABLE where ";  
  queryString = RPCDBReader::concatenateQueryStringWithRunAndSite(queryString,run,site);
  this->executeQueryWithSQLstatement(queryString.c_str());
  
}

string RPCDBReader::concatenateQueryStringWithRunAndSite(const string & queryString, const unsigned & run,const string & site){
  
  string returnValue,runAsString,mutableSite;
  stringstream ss;
  ss << run;
  runAsString = ss.str();
  ss.clear();
  runAsString = " RUN = '" + runAsString + "'";
  mutableSite = " and SITE = '" + site + "'";
  return queryString+runAsString+mutableSite;
  
}

void RPCDBReader::getChambersDataForRunRangeAndSite(const unsigned & runstart,const unsigned & runend,const string & site){
  
  string _runstart, _runend;
  stringstream ss;
  ss << runstart;
  _runstart = ss.str();
  ss.clear();
  ss << runend;
  _runend = ss.str();
  ss.clear();
  string queryString = 
  "select distinct r.CHAMBER_NAME, r.CHAMBER_TYPE, c.LAYER, c.HV, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.CURRENT_TN, c.CURRENT_TW,c.CURRENT_B ,c.STATION from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and ";
  queryString = queryString + " c.RUN >= '" + _runstart + "' or c.RUN <= '" + _runend + "' and c.SITE = '" + site + "'";
  queryString += " order by c.STATION";
  this->executeQueryWithSQLstatement(queryString.c_str());
  
}

void RPCDBReader::printResultWithNcolumns(const int & Ncolumns){
  
  while(this->getCurrentResultSet()->next()){
    for (int i= 0 ; i < Ncolumns;i++){
      cout << this->getCurrentResultSet()->getString(i+1) << " ";
    }
    cout << endl;
  }
  
}

Connection * RPCDBReader::getCurrentConnection(){
  return this->_connection;
}
