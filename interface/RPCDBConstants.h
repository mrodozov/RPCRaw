/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 10.08.13
*/

/** data base strings needed for the connection */

#ifndef DB_CONSTANTS_H
#define DB_CONSTANTS_H

#include <string.h>

const std::string kRPCDB_username="CMS_RPC_CONSTR"; // username
const std::string kRPCDB_password="RPCConDB_2012"; // password
const std::string kRPCDB_connectionString="CMSR"; // the connection string describes the database connection details in tnsnames.ora in /etc

#endif