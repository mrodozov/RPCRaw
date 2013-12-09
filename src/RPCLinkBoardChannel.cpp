/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoardChannel.h"
#include <iostream>

using namespace std;

void RPCLinkBoardChannel::setHits (const std::vector<unsigned> & vectorOfHits){
  this->_vectorOfhits = &vectorOfHits;  
} // set vector of hits, with receiving reference to the data object trought the parent Chamber object

const std::vector<unsigned> & RPCLinkBoardChannel::getHits () const { 
  return *this->_vectorOfhits;
} // get the vector of hits

void RPCLinkBoardChannel::setEfficiency (const double & channelEfficiency) { this->_channelEfficiency = channelEfficiency; }

const int & RPCLinkBoardChannel::getAllTracks () const { 
  return this->allTracks;
}

const int & RPCLinkBoardChannel::getEfficientTracks () const { return this->efficientTracks;}

bool RPCLinkBoardChannel::hasHit(){
  return !this->_vectorOfhits->empty();
} 

bool RPCLinkBoardChannel::hasMultipleHits (){
  return (this->hasHit() && this->_vectorOfhits->size() > 1);
}

RPCLinkBoardChannel::RPCLinkBoardChannel(){
  this->setEfficiency(0); // well, should have default value
  this->resetEfficiencyCounters();
}

RPCLinkBoardChannel::~RPCLinkBoardChannel(){
  
}

void RPCLinkBoardChannel::incrementEfficiencyCounters(const bool & hitIsFound){
  if(hitIsFound){
    this->efficientTracks++;
  }
  this->allTracks++;
}

void RPCLinkBoardChannel::resetEfficiencyCounters(){
  this->allTracks=0;
  this->efficientTracks=0;
}

double RPCLinkBoardChannel::getEfficiency () {
  
  //cout << "channel " << this->getOnlineNumber() << " efficientTracks/allTracks " << this->efficientTracks << " " << this->allTracks << endl;
  if (this->allTracks > 0){
    return (double)((double)this->efficientTracks/(double)this->allTracks)*100;
  }
  
  else return -1; // if there was no track crossing the channel, its not inefficient
}