/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
 * 
 * This class has to be factory class for RPCLinkBoard objects. The analysis goes trough its methods
 * Nice idea is to implement counters, then to write the results for each event in the chambers 
 * in the cluster with incrementing the counters. 
*/

#ifndef RPC_CHAMBERS_CLUSTER_H
#define RPC_CHAMBERS_CLUSTER_H

#include "RPCLinkBoard.h"

class RPCChambersCluster {
  
  map<unsigned,RollInterface*> _chambersPointer ;
  map<unsigned,RollInterface*> _triggerObjectsPointer ;
  unsigned _numberOfChambers;
  unsigned _numberOfTriggers;
  
protected:
  
public:
  
  /**  Essential methods */
  
  RPCChambersCluster ();
  RPCChambersCluster (const int & numberOfChambers,const int & numberOfTriggerObjs,const string & RE4TypeOfChambers);
  virtual ~RPCChambersCluster();
  
  void createNewClusterOfChambersWithRE4type(const int & nChambers,const string & RE4type); // factory method for chambers needed
  void createNewClusterOfTriggerObjects(const int & nTriggerObjects); // factory method for trigger objects 
  void setDataSourceForNchambers(const int & nChambers,const vector<vector<vector< unsigned> > > & dataSource);
  void setDataSourceForNtriggerObjects(const int & nTriggerObject,const vector<vector<vector< unsigned> > > & dataSource);
  
  const unsigned & getNumberOfChambers() const { return this->_numberOfChambers;} // return the number of current chamber objects. set it from createNewClusterOfChambers
  const unsigned & getNumberOfTriggerObjects() const { return this->_numberOfTriggers;} // set it from createNewClusterOfTriggerObjects 
  
  virtual RPCLinkBoard * getChamberNumber(const int & chamberNumber);
  virtual RPCLinkBoard * getTriggerObjectNumber(const int & triggerObjectNumber);
  
  void deleteAllChambers();
  void deleteAllTriggerObjects();
  void deleteAllObjects();
  
  /** here , place some analysis methods that loop on all the chambers and retrieve some information about the tracks */
  
  void findTrackInCurrentEvent(TFile * fileToSave); // put each 
  void recursiveLoopOnAllChambers(bool searchInReferenceChambers);
  
  
};

#endif