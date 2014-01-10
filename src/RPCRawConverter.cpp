/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
 */

#include <sstream>
#include <fstream>
#include <vector>
#include <exception>
#include <assert.h>
#include "../interface/ROOT.h"
#include "../interface/RPCRawConverter.h"
#include "../interface/RPCRawTypes.h"

using namespace std;

RPCRawConverter::RPCRawConverter(){
  // start with this constructor when converting more than one file (and change the files throught a loop)
  
  this->setCurrentFileType(kSiteIsNotDefined);
  
}

RPCRawConverter::~RPCRawConverter(){
  
}

RPCRawConverter::RPCRawConverter(TFile * inputFile){
  
  this->setCurrentFileType(this->getFileTypeForFile(inputFile));
  this->setCurrentFile(inputFile);
  this->initialyzeStructure();
  
}

/** */

ESiteFileType RPCRawConverter::getCurrentFileType(){
  return _currentFileType;
}

void RPCRawConverter::setCurrentFileType(ESiteFileType type){
  this->_currentFileType = type;
}

void RPCRawConverter::convertToCommonFormat(){
  
  //
  this->setCurrentTree(this->getTreeFromFileWithType(this->getCurrentFileType()));
  // the tree is set only once per file. Then, one should use only the getData methods to select the next event
  // get the first event initially with this methods
  switch (_currentFileType){
    
    case kIsCERNrawFile:
      // handle CERN file
    {
      this->getDataFromCERNinputFile();
      break;
    }
    case kIsBARCrawFile:
      // handle BARC file
    {
      this->getDataFromBARCinputFile();
      break;
    }
    case kIsGENTrawFile:
      // handle GENT file
    {
      this->getDataFromGHENTinputFile();
      break;
    }
    case kSiteIsNotDefined:  
      
      break;
    default:
    {
      // handle error, print description and exit
      cout << "site not recognized, domain getCurrentFileType" << endl;
      assert(0);
    }
  }
}

ESiteFileType RPCRawConverter::getFileTypeForFile(TFile * file){
  
  ESiteFileType retVal = kSiteIsNotDefined;
  
  if (!file->IsOpen()){
    cout << " file not opened " << endl;
  }
  
  // decide the file type for the other sites on the top of their structure.
  TIter nextkey( file->GetListOfKeys() );
  TKey *key;
  TObject *obj;
  TObjArray * listOfBranches;
  TBranch * aBranch;
  TTree * innerThree;
  string nameOfBranch; // use this to compare with branch names within the file specific for the site, and return site type
  
  while ( ( key = (TKey*)nextkey()) != NULL ) {
    obj = key->ReadObj();
    innerThree = dynamic_cast<TTree*>(file->Get(obj->GetName()));
    if (innerThree->InheritsFrom(TTree::Class_Name())) {
      
      listOfBranches = innerThree->GetListOfBranches();
      // search for specific branch name
      
	aBranch = dynamic_cast<TBranch*>(listOfBranches->At(0));
	nameOfBranch = aBranch->GetName();
	//cout << nameOfBranch << endl;
	
	if (nameOfBranch.find(kRPCisCERNFileNumberOfChannelsBranch) != string::npos){
	  retVal = kIsCERNrawFile;
	}
	else if (nameOfBranch.find(kRPCisGHENTEventBranch) != string::npos){
	  retVal = kIsGENTrawFile;
	}
	
	else {
	  // assert its a barc file then in the assertion, with condition value to match !=string::npos
	  assert(nameOfBranch.find(kRPCisBARCfirstBranch) != string::npos);
	  retVal = kIsBARCrawFile;
	}
      
    }
    
    else {
      // throw some nasty exception and set the return value to error
      // maybe use some error (TError perhaps) in a waterfall passed by reference .  
    }
    // break the while loop we read only the first branch name
    break;
  }
  
  return retVal;
  
}

string RPCRawConverter::getNameOfCurrentFile(){  
  return this->getCurrentFile()->GetName();
}

int RPCRawConverter::getDecimalFromHex(int hexValue){
  int retVal;
  stringstream ss;
  ss << std::hex << hexValue;
  ss >> retVal;
  ss.clear();
  return retVal;
  
}

void RPCRawConverter::getDataFromCERNinputFile(){
  
  TBranch * aBranch = 0;
  TObjArray* objectArray = 0 ;
  int numberOfBranches = 0;
  string nameOfBranch;
  vector< unsigned > * channelsVector = 0;
  int trigger_channels_counter = 0,chamber_channels_counter=0,chambersCounter=0,trigerObjectsCounter=0;
  numberOfBranches = getCurrentTree()->GetListOfBranches()->GetEntries();
  objectArray = getCurrentTree()->GetListOfBranches(); //
  vector< vector<unsigned int> > triggerChannelVectors,chamberChannelVectors,partitionA,partitionsBC;
  //cout << this->_eventCounter << " " ;
  
  for (int i = 0 ; i < numberOfBranches ; i++){
    
    //aBranch = dynamic_cast<TBranch *>(objectArray->At(i));
    nameOfBranch = objectArray->At(i)->GetName();
    aBranch = this->getCurrentTree()->GetBranch(nameOfBranch.c_str());
    //cout << nameOfBranch << " " << this->_eventCounter << endl;
    aBranch->GetEntry(this->_eventCounter);
    
    switch (this->getBranchTypeFromBranchName(nameOfBranch)){
      
      case kIsCERNtriggerChannelBranch:
      {
	this->_currentTTree->SetBranchAddress(nameOfBranch.c_str(),&channelsVector,&aBranch);
	triggerChannelVectors.push_back(*channelsVector);
	channelsVector->clear();
	
	if(trigger_channels_counter == 31){
	  if(!this->structureIsInitialized()){
	    this->_triggerObjectRecords.push_back(triggerChannelVectors);
	  }
	  else{
	    this->_triggerObjectRecords.at(trigerObjectsCounter) = triggerChannelVectors;
	  }
	  
	  trigerObjectsCounter++;
	  triggerChannelVectors.clear();
	}
	
	trigger_channels_counter++;
	
	break;
      }
      
      case kIsCERNchamberChannelBranch:
      {
	
	this->getCurrentTree()->SetBranchAddress(nameOfBranch.c_str(),&channelsVector,&aBranch);
	chamberChannelVectors.push_back(*channelsVector);
	int channelNumberInCurrentChamber = chamber_channels_counter - (96*chambersCounter);
	
	if(channelNumberInCurrentChamber < 64){
	  partitionsBC.push_back(*channelsVector);
	}
	else{
	  partitionA.push_back(*channelsVector);
	}
	
	channelsVector->clear();
	
	if (chamber_channels_counter != 0 && ((chamber_channels_counter+1 ) % 96) == 0){
 	  if(!this->structureIsInitialized()){
 	    // push back initial elements
	    this->_chamberObjectsRecords.push_back(chamberChannelVectors);
 	  }
 	  
 	  else {
	    // the elements should excist, don't clear but just change them
	    // construct the vector to be used with swapped partitions
	    
	    chamberChannelVectors.assign(partitionA.begin(),partitionA.end());    
	    for (unsigned vectoriter = 0;vectoriter < partitionsBC.size();vectoriter++){
	      chamberChannelVectors.push_back(partitionsBC.at(vectoriter));
	    }
	    
	    this->_chamberObjectsRecords.at(chambersCounter) = chamberChannelVectors;
 	  }
	  chambersCounter++; // number of chambers, if needed somewhere
	  partitionA.clear();
	  partitionsBC.clear();
	  chamberChannelVectors.clear();
	  
	}
	chamber_channels_counter++ ;
	
	break;
      }
      
      case kIsCERNHitsPerChannelBranch:
      {
	// handle if needed
	break;
      }
      
      case kIsCERNnumberOfChannelsBranch:
      {
	// handle if needed
	break;
      }
      
      default:
      {
	// no default for this case, break and pass proper error
	break;
      }
    } 
    
  }
  
}

void RPCRawConverter::getDataFromBARCinputFile(){
  
}

void RPCRawConverter::getDataFromGHENTinputFile(){
  
  RPCGhent::RAWData data;
  int errorCode = 0;
  
  errorCode = this->getCurrentTree()->SetBranchAddress("iEvent",&data.iEvent);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintNTop",&data.ScintNTop);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintTopHits",data.ScintTopHits);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintNBot",&data.ScintNBot);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintBotHits",data.ScintBotHits);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintTopQDC",data.ScintTopQDC);
  errorCode += this->getCurrentTree()->SetBranchAddress("ScintBotQDC",data.ScintBotQDC);
  errorCode += this->getCurrentTree()->SetBranchAddress("TDCTime",data.TDCTime);
  errorCode += this->getCurrentTree()->SetBranchAddress("TDCCh",data.TDCCh);
  errorCode += this->getCurrentTree()->SetBranchAddress("TDCNHits",&data.TDCNHits);
  
  assert(errorCode==0); // if even single branch is missing or is with other name, this would fail
  
  this->getCurrentTree()->GetEntry(this->getEventNumber());
  
  for (int i = 0 ; i < 5 ; i++){
    
    vector< vector< unsigned> > emptyVectorChamber;
    
    if(!this->_strucureInitialized){
      this->_chamberObjectsRecords.push_back(emptyVectorChamber);
      if ( i == 0 ){ this->_triggerObjectRecords.push_back(emptyVectorChamber); }
    }
    
    for (int j = 0 ; j < 96 ; j++){
      
      vector< unsigned> emptyChannelVector;
      
      if(!this->_strucureInitialized){
	this->_chamberObjectsRecords.at(i).push_back(emptyChannelVector);
	if ( i == 0 && j < 32 ) { this->_triggerObjectRecords.at(i).push_back(emptyChannelVector); }
      }
      else {
	//cout << i << " " << j << endl;
	this->_chamberObjectsRecords.at(i).at(j) = emptyChannelVector;
	if ( i == 0 && j < 32 ) { this->_triggerObjectRecords.at(i).at(j) = emptyChannelVector; }
	//cout << "end " << endl;
      }
    }
  }
  
  // break here if this is initialyzing, we don't need furher execution in this case  
  //}
  
  int channel=0;
  
  if(this->structureIsInitialized()){
    
    for (int i = 0 ; i < data.TDCNHits ; i++){
      
      if ( data.TDCCh[i] < 32 ){
	this->_triggerObjectRecords.at(0).at( data.TDCCh[i] ).push_back(data.TDCTime[i]);
      }
      
      else {

	int TdcChannelToSearch = data.TDCCh[i];
	this->getGhentTDCtoRPCmap().find(TdcChannelToSearch); // TODO - find why there is no value for this key when it should be. here is the bug
	channel = this->getGhentTDCtoRPCmap().find(TdcChannelToSearch)->second;
	this->_chamberObjectsRecords.at((int)(channel/100)).at((int)(channel%100)).push_back(data.TDCTime[i]);
	
      }
    }
    
  /*  
  // debug lines
    cout << "Event: " << this-> getEventNumber() << " iEvent: " << data.iEvent << endl 
    << " TDChits: " << data.TDCNHits << " records: " ;
    
    for (int i = 0 ; i < data.TDCNHits ; i++){
      cout << data.TDCCh[i] << " " << data.TDCTime[i] << " ";
      
    } */
  
  }
  //cout << endl;
  
  
}

unsigned int RPCRawConverter::getNumberOfChamberObjects(){
  return this->getChambersData().size();
}

unsigned int RPCRawConverter::getNumberOfTriggerObjects(){
  return this->getTriggersData().size();
}

bool RPCRawConverter::nextEvent(){
  if(this->_eventCounter < this->getCurrentTree()->GetEntries()){
    switch(_currentFileType){
      case kIsCERNrawFile:
      {
	this->getDataFromCERNinputFile();
	break;
      }
      case kIsBARCrawFile:
      {
	this->getDataFromBARCinputFile();
	break;
      }
      case kIsGENTrawFile:
      {
	this->getDataFromGHENTinputFile();
	break;
      }
      default:
      {
	// there is no default, its a error
	assert(0);
	break;
      }
    }
    
    this->_eventCounter++;
    //cout << "event " << this->getEventNumber() << endl; // print for debug
    return true;
  }
  else{
    // stop the while loop if the last event is reached
    // cout << "last event reached, exit. " << endl;
    return false;
  }
}

void RPCRawConverter::clearCurrentDataStructures () {
  
  this->clearThreeDimensionalVector(this->_triggerObjectRecords);
  this->clearThreeDimensionalVector(this->_chamberObjectsRecords);
  // clear other internal structures if any, later on
}

void RPCRawConverter::setCurrentTree ( TTree* aTree ) {
  this->_currentTTree = aTree;
}

TTree * RPCRawConverter::getCurrentTree () {
  return this->_currentTTree;
}

TTree * RPCRawConverter::getTreeFromFileWithType(ESiteFileType fileType){
  
  switch (fileType){
    
    case kIsCERNrawFile:
      // handle CERN file
    {
      
      TIter nextkey( this->getCurrentFile()->GetListOfKeys() );
      TKey *key;
      TObject *obj;
      
      while ( ( key = (TKey*)nextkey() ) != NULL ) {
	obj = key->ReadObj();
	this->setCurrentTree(dynamic_cast<TTree*>(getCurrentFile()->Get(obj->GetName())));
	if (!this->_currentTTree->InheritsFrom(TTree::Class_Name())){
	  // handle error and return, 
	  cout << " Object in the given file is not a TTree ! " << endl;
	  
	  return nullptr;
	}
	// break the while loop, we need only the first record. 
	// Two identical trees exists in the file, we need only the first one
	
	break;
      }
      
      // switch case brake
      break;
    }
    
    case kIsBARCrawFile:
      // handle BARC file
    {
      
      break;
    }
    case kIsGENTrawFile:
      // handle GENT file
    {
      TIter nextkey( this->getCurrentFile()->GetListOfKeys() );
      TKey *key;
      TObject *obj;
      
      while ( ( key = (TKey*)nextkey() ) != NULL ) {
	obj = key->ReadObj();
	this->setCurrentTree(dynamic_cast<TTree*>(getCurrentFile()->Get(obj->GetName())));
	if (!this->_currentTTree->InheritsFrom(TTree::Class_Name())){
	  // handle error and return, 
	  cout << " Object in the given file is not a TTree ! " << endl;
	  
	  return nullptr;
	}
	
      }
      
      break;
    }
    
    case kSiteIsNotDefined:
      break;
    default:
    {
      // handle error, print description and exit
      cout << "not defined file type ! domain getTreeFromFileWithType " << endl;
      assert(0);
    }
    
    }
    return this->_currentTTree;
}

TFile * RPCRawConverter::getCurrentFile(){
  return this->_currentFile;
}

void RPCRawConverter::setCurrentFile(TFile * file){
  this->_currentFile = file;
}

int RPCRawConverter::getEventNumber(){
  return this->_eventCounter;
}

EBranchType RPCRawConverter::getBranchTypeFromBranchName(string branchName){
  EBranchType retval;
  if (branchName.find(kRPCisCERNFileChannelBranch) != string::npos ) {
  
    /**
     * two cases - it is trigger channel branch or chamber channel branch. If it contains 'Module2_LE_CH' and the last 
     * characters are numbers between 0 and 31 - its trigger channel branch. In any other case its chamber channel branch
     */
    
    int channelNumber=0;
    string buffer="Module2_LE_CH";
    channelNumber = atoi(branchName.substr(buffer.size(),branchName.size()-buffer.size()).c_str());
    if (branchName.find(buffer) != string::npos && channelNumber < 32){
      
      //cout << channelNumber << endl;
      retval = kIsCERNtriggerChannelBranch;
      
    }
    else{ retval = kIsCERNchamberChannelBranch; }
    
  }
  
  if (branchName.find(kRPCisCERNFileHitsPerChannelBranch) != string::npos) retval = kIsCERNHitsPerChannelBranch;
  if (branchName.find(kRPCisCERNFileNumberOfChannelsBranch) != string::npos) retval = kIsCERNnumberOfChannelsBranch;
  
  return retval;
}

void RPCRawConverter::clearThreeDimensionalVector(vector<vector<vector<unsigned int> > > & vectorToClear){
  
  for ( unsigned int i = 0 ; i < vectorToClear.size() ; i++){  
    for ( unsigned int j = 0 ; j < vectorToClear[i].size() ; j++ ){
      vectorToClear[i][j].clear();      
    }
    vectorToClear[i].clear();
  }
  vectorToClear.clear();
}

int RPCRawConverter::getTotalEvents(){
  return this->getCurrentTree()->GetEntries();
}

void RPCRawConverter::setEventNumber(int event){
  
  if(event <= this->getTotalEvents()){
    this->_eventCounter = event;
  }
  else{
    cout << "warning, event number given exceeds the total number of events, set to the last event" << endl;
    this->_eventCounter = this->getTotalEvents();
  }
  
}

void RPCRawConverter::initialyzeStructure(){
  this->_eventCounter = 0 ;
  this->_strucureInitialized = false;
  this->convertToCommonFormat();
  this->_strucureInitialized = true;
  this->setEventNumber(0);
}

const map<int , int> & RPCRawConverter::getGhentTDCtoRPCmap () const { 
  
  if (this->_ghentTDCChannelsToChambersChannelsMap.empty()){
     cout << " First use setGhentTDCtoRPCmap with file name (search in the resources/GHENT) to set the Ghent map " << endl;
     assert(!this->_ghentTDCChannelsToChambersChannelsMap.empty());     
  }
  return this->_ghentTDCChannelsToChambersChannelsMap;
  
}

void RPCRawConverter::setGhentTDCtoRPCmap(const string & fileName){
  //
  
  int RPCCh =0, TDCCh =0;
  ifstream mappingfile;
  mappingfile.open(fileName.c_str());
  stringstream _ss;
  string buffer;
  
  while(getline(mappingfile,buffer)) {
    _ss.str(buffer);
      _ss >> RPCCh >> TDCCh;
      this->_ghentTDCChannelsToChambersChannelsMap[TDCCh] = RPCCh;
      _ss.clear();
  }
  
  mappingfile.close();
}
