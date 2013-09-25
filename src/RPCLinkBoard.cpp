/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoard.h"

using namespace std;

/** ihneritance methods goes here to make the objects of this class works fine */

// strips methods

void RPCLinkBoard::allocStrips(){
  // this is the factory method used only once after the object is constructed. 
  this->strips = new RPCLinkBoardChannel[96];
  
}

void RPCLinkBoard::initStrips(){
  ExRoll::initStrips();
  // do additional inits
  this->setIsTriggerChamber(false);
  this->setStationNumber(-1); // which means its not set
}

RPCLinkBoardChannel * RPCLinkBoard::getStripCollection (){
  return dynamic_cast<RPCLinkBoardChannel*>(this->strips);
}

RPCLinkBoardChannel * RPCLinkBoard::getStrip(int element){
  return & getStripCollection()[element-1];
}

RPCLinkBoardChannel * RPCLinkBoard::getChannel(int channel){
  this->getStrip(channel);
}

// constructors and destructors

RPCLinkBoard::RPCLinkBoard () : ExRoll() {
  
}

RPCLinkBoard::RPCLinkBoard (const string& RollOnlineName) : ExRoll(RollOnlineName) {
  // set some 
  
}

RPCLinkBoard::~RPCLinkBoard (){
  
  delete [] this->getStripCollection();
  this->strips = NULL;
  //cout << " RPCLinkBoard destructor called " << endl;
  
}

// chip methods. chip objects are grouping the chamber channels (strips) into pack of 8, and also provide several common methods to the group

RPCLinkBoardChip * RPCLinkBoard::getChipByNumber(const int & ChipNumber){
  return dynamic_cast<RPCLinkBoardChip*>(this->getChipsMap().find(ChipNumber)->second);
}

void RPCLinkBoard::insertNewChipWithNumberAndID(const int & num,const int & ID){
  this->incrementNumberOfChips(); // they should be zero assigned by the constructor
  RPCLinkBoardChip * aChip = new RPCLinkBoardChip; // this to be changed onl when this method is overloaded by the next class if needed
  aChip->setChipNumber(num);
  aChip->setChipID(ID);
  aChip->assignStripsFromRoll(this);
  //this->Chips[getNumberOfChipsAssigned()-1] = aChip; // make this not directly - ahaaa ! not to use the pointer directly and to remain private for the class. its not directly
  this->insertNewChipToChipsMap(num,aChip);
} // insert chip

/**  place your costum methods here */

void RPCLinkBoard::setStripsHitsDataFromSource(const vector< vector<unsigned> > & vectorSource){
  this->_vectorToStripsHits = &vectorSource;
  // now set each strip vector
  for (int i = 0 ; i < vectorSource.size() ; i++){
    //cout << i+1 << endl;
    this->getStrip(i+1)->setHits(vectorSource.at(i));
  }
  //cout << "out" << endl;
  
}

const vector< vector<unsigned> > & RPCLinkBoard::getStripsHitsDataVector () const {
  return *this->_vectorToStripsHits;
}


void RPCLinkBoard::findAllClustersForTiggerTimeReferenceAndTimeWindow(int triggerTimeReference,int timeWindow){
  
  for (int i = 0; i < this->getClones() ; i ++ ){
    
    // getClones() gives the number of the so called Etha Partitions. In case of Endcap they are always 3 - A,B,C partitions
    
    for (int j = this->getFirstStripNumberOfClone(i+1) ; j <= this->getLastStripNumberOfClone(i+1) ; j++ ){
      
      // getFirst and getLast strip number of clone is returning the first and the last channel for given partition, so we could loop on the channels of a given partition
      
      if (
	this->getChannel(j)->hasHit() // see if the channel has at least one hit
	&& this->getChannel(j)->getHits().at(0) <= triggerTimeReference+timeWindow // 
	&& this->getChannel(j)->getHits().at(0) >= triggerTimeReference-timeWindow // 
	) {
	
	cout << "Cluster started from : ";
      
	// write only the cluster starting channel thats enough
	this->_clusterStartingNumbers.push_back(j);
	int k = j;
	// increment a counter starting from the first for all consecutive hits
	while (k <= this->getLastStripNumberOfClone(i+1) && this->getChannel(k)->hasHit()) {
	  cout << k << " ";
	  k++;
	}
	
	cout << endl;
	
	j = k; // skip all strips in the cluster and continue from the next one
	k = 0 ; // reset the internal counter
		
      } 
    }
  }
}
