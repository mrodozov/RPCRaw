/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_RAW_CONVERTER_H
#define RPC_RAW_CONVERTER_H

#include <assert.h>
#include <string>
#include <vector>
#include "../interface/ROOT.h"
#include "../interface/RPCRawTypes.h"

using namespace std;

class RPCRawConverter {
  
private:
  
  string _currentFileName; // 
  TFile * _currentFile; // pointer to open TFile, set to null when not operating
  TTree * _currentTTree; // pointer to the tree that holds the data. 
  int _eventCounter;
  ESiteFileType _currentFileType;
  bool _strucureInitialized;
  map<int,int> _ghentTDCChannelsToChambersChannelsMap;
  TBranch * _localBranch; // added because method local defined pointers gets out of scope before the tree get destroyed.
  
  vector< vector< vector<unsigned int> > > _chamberObjectsRecords;
  vector< vector< vector<unsigned int> > > _triggerObjectRecords;
  
protected:
  
  void getDataFromCERNinputFile(); // deals with the internal structure of the CERN data file
  void getDataFromBARCinputFile(); // deals with the internal structure of the BARC data file
  void getDataFromGHENTinputFile(); // deals with the internal structure of the GHENT data file
  // 
  void setCurrentTree(TTree * aTree);
  TTree * getTreeFromFileWithType(ESiteFileType fileType);
  void clearCurrentDataStructures(); // clear the vectors that holds the data ,on each event
  EBranchType getBranchTypeFromBranchName(string branchName); // return enumerator value for given branch name to be used in switch block
  bool structureIsInitialized(){return this->_strucureInitialized;}
  const map<int , int> & getGhentTDCtoRPCmap () const;// { return this->_ghentTDCChannelsToChambersChannelsMap; }
  
public:
  
  /** essential public methods */
  
  RPCRawConverter();
  RPCRawConverter(TFile * inputFile); // contructor with file already open
  ~RPCRawConverter();
  
  void convertToCommonFormat(); // executes internally 2 methods - the one that gets the events tree, and one that gets the next event
  ESiteFileType getFileTypeForFile(TFile * file); // 
  string getNameOfCurrentFile(); // return the name of the current file to be used  
  ESiteFileType getCurrentFileType(); // returns the type (CERN BARC or GHENT) of the file
  void setCurrentFileType(ESiteFileType type);
  unsigned int getNumberOfChamberObjects(); // returns the number of chamber objects we have data in this file
  unsigned int getNumberOfTriggerObjects(); // returns the number of trigger objects we have data in this file
  bool nextEvent(); // read the next event from the TTree and change the internal vector structure
  TTree * getCurrentTree() ; // returns pointer to the current tree
  TFile * getCurrentFile() ; // return pointer to the current file
  void setCurrentFile(TFile * file); // set the current file by pointer to open TFile. To be used with the default constructor
  int getEventNumber(); // return the current event number
  int getTotalEvents(); // wrapper for the events in the TTree
  void setEventNumber(int event); // carefull with this method, because it sets the number of event to be started from
  void initialyzeStructure(); // used if the object is created with the default constructor. initializes the format
  void setGhentTDCtoRPCmap(const string & fileName); // method used to set the Ghent TDC to RPC channels. pass the path to the file, the file is under resources.
  
  // TODO -> add method to detach the file and the tree, when the last event is reached
  // TODO -> maybe some of the counters need tunning
  // TODO -> implement the destructor, how to clear the fields and to detach from resources if any
  
  // data getters, full vectors, chamber by number and channel by channel number and chamber number
  
  const vector< vector< vector<unsigned int> > > & getChambersData() const {return this->_chamberObjectsRecords;} // get unmutable reference to chamber object records 
  const vector< vector< vector<unsigned int> > > & getTriggersData() const {return this->_triggerObjectRecords;} // return unmutable reference to trigger object records
  const vector< vector<unsigned int> > & getChamberVector( unsigned int chamberNumber) const { return this->_chamberObjectsRecords.at(chamberNumber-1);} 
  const vector< vector<unsigned int> > & getTriggerVector( unsigned int triggerNumber) const { return this->_triggerObjectRecords.at(triggerNumber-1);} 
  const vector<unsigned int> & getChannelHitsVectorFromChamber ( unsigned int chamberNumber, unsigned int channelNumber) const { return this->getChamberVector(chamberNumber).at(channelNumber-1);} 
  const vector<unsigned int> & getChannelHitsVectorFromTriggerObject ( unsigned int triggerObjectNumber, unsigned int channelNumber) const { return this->getChamberVector(triggerObjectNumber).at(channelNumber-1);} 
  
  /** additional vectors (number of chambers, hits per channels branches etc. ) */
  
  /** helper methods */
  
  int getDecimalFromHex(int hexValue); // returns the int value if hex is given
  void clearThreeDimensionalVector(vector<vector<vector<unsigned int> > > & vectorToClear);
  
  /* add some additional methods to handle errors maybe use TError */
  
};


#endif
