/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 10.08.13
*/

#ifndef RPCDB_READER_H
#define RPCDB_READER_H

#include <string>
#include <occi.h>
#include "RPCDBConstants.h"

/** class to serve the DB connections, retrieve and hold run conditions data */

using namespace std;
using namespace oracle::occi;

class RPCDBReader {
  
  unsigned _currentRunNumber; // the runnumber at the moment
  string _currentSiteName;
  Connection * _connection;
  Environment * _environment;
  ResultSet * _resultSet;
  
protected:
  
public:
  
  void setRunNumber(unsigned number){ this->_currentRunNumber = number;}
  const unsigned & getRunNumber() const { return this->_currentRunNumber;}
  void setSiteName(string siteName) { this->_currentSiteName = siteName ;} 
  const string & getSiteName() const { return this->_currentSiteName;}
  
  void openNewConnection(); // set the environment and the connection
  void closeCurrentConnection(); // terminate the connection
  
  void executeQueryWithSQLstatement(const string & SQLStatement); // execute any general sql statement
  void getEnvironmentDataForRunAndSite(const unsigned & run,const string & site); // get only environmental variables
  void getChambersDataForRunAndSite(const unsigned & run,const string & site); // get all chambers data 
  
  /** for list of runs we would need the data for HV (at least) for the chambers under study to get the one that are used as trigger */
  void getChambersDataForRunRangeAndSite(const unsigned & runstart,const unsigned & runend,const string & site);
  
  ResultSet * getCurrentResultSet () ; // get the result
  Connection * getCurrentConnection();
  
  static string concatenateQueryStringWithRunAndSite(const string & queryString, const unsigned & run,const string & site);
  void printResultWithNcolumns(const int & Ncolumns);
  
};

#endif