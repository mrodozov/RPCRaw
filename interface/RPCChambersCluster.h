/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
 * 
 * This class has to be factory class for RPCLinkBoard objects. The analysis goes trough its methods
 * Nice idea is to implement counters, then to write the results for each event in the chambers 
 * in the cluster with incrementing the counters. 
 * 
 * Another nice idea is to separate the converter class to analyze each event on different thread.
 * If the converter has queue of analyzed events the Cosmics Stand could create new chambers for each 
 * event when it received an notification that new event is avalilable , make the analysis for this event.
 * As many threads as we have we could create chamber stack for them and each chamber stack would analyze
 * the events converted by each thread. Each stack with analyzed results would be put in a queue to wait
 * become first in the queue to write the results in the histograms.
 * 
*/

#ifndef RPC_CHAMBERS_CLUSTER_H
#define RPC_CHAMBERS_CLUSTER_H

#include "RPCLinkBoard.h"
#include "RPCRawTypes.h"

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
  
  void variousStudyExperimentalFunction(TFile * fileToSave,TH1F * histo[10],const int & eventNum); // put each track on histogram along with all the rest of the hits
  map<int,vector<double> > getReconstructedHits(vector<int> vectorOfReferenceChambers,const int & timeWindow,const int & timeReference,const int &Chi2Goodnes = 20,const ESiteFileType & fileType = kIsCERNrawFile);
  
  
  /** this method goes trough all the reference chambers */
  
  
  int getTimeReferenceValueForSiteType(ESiteFileType siteType);
  int getTimeWindowForSiteType(ESiteFileType siteType);
  
};

// in case one does want to clarify better the name

typedef RPCChambersCluster RPCCosmicStand;
typedef RPCChambersCluster RPCCosmicSetup;

#endif