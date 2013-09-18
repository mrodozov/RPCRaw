/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoard.h"

using namespace std;

/** ihneritance methods goes here to make the objects of this class works fine */

// strips methods

void RPCLinkBoard::allocStrips(){
  // this is a 
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

