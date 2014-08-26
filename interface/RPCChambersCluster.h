/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
 * 
 * This class has to be factory class for RPCLinkBoard objects. The analysis goes trough its methods
 * 
 * 
*/

#ifndef RPC_CHAMBERS_CLUSTER_H
#define RPC_CHAMBERS_CLUSTER_H

#include "RPCLinkBoard.h"
#include "RPCRawTypes.h"
#include "RPCConfigObject.h"

class RPCChambersCluster {
  
  map<unsigned,RollInterface*> _chambersPointer ;
  map<unsigned,RollInterface*> _triggerObjectsPointer ; // trigger object is actually scintilator, but for the sake of time we use the channel number and vector of hits from chamber class 
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
  
  bool isShowerEvent ();
  
  virtual RPCLinkBoard * getChamberNumber(const int & chamberNumber);
  virtual RPCLinkBoard * getTriggerObjectNumber(const int & triggerObjectNumber);
  
  void deleteAllChambers();
  void deleteAllTriggerObjects();
  void deleteAllObjects();
  
  /** here , place some analysis methods that loop on all the chambers and retrieve some information about the tracks */
  
  void variousStudyExperimentalFunction(TFile * fileToSave,TH1F * histo[10],const int & eventNum); // put each track on histogram along with all the rest of the hits
  
  // one does not simply tries to understand this method in a few minutes
  map<int,vector<double> > getReconstructedHits(vector<unsigned> vectorOfReferenceChambers,const int & timeWindow,const int & timeReference,bool & isVerticalTrack,map<int,double> & scintilatorsCoordinates,const bool & keepRecoTrack,TFile * fileToWriteRecoTracks,const int & eventNum,const double & correlationFactor = 0.98,const ESiteFileType & fileType = kIsCERNrawFile);
  
  /** this method goes trough all the reference chambers */
  
  int getTimeReferenceValueForSiteType(ESiteFileType siteType);
  int getTimeWindowForSiteType(ESiteFileType siteType);
  
  /** helper method to get the vector of partitions between two references, with reference vectors for 1. number of reference chambers 2. cluster numbers in the reference chambers   */
  vector<vector<int> > getPartitionsVectorForVectorOfReferenceChambers(const int & chamberNumber,const vector<int> & vectorOfReferenceChambers,const vector<int> & vectorOfClusterNumbersCombination);
  void configureChambersWithConfigObject(RPCAbstractRunConfig * runConfigObject);
  
  void getScintilatorsCoordinates();  
  
  
  
};

// in case one does want to clarify better the name

typedef RPCChambersCluster RPCCosmicStand;
typedef RPCChambersCluster RPCCosmicSetup;

#endif