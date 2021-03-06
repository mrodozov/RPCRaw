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

const int & RPCLinkBoardChannel::getAllTracks () const {  return this->allTracks; }

const int & RPCLinkBoardChannel::getEfficientTracks () const { return this->efficientTracks;}

bool RPCLinkBoardChannel::hasHit(){
  return !this->_vectorOfhits->empty();
} 

bool RPCLinkBoardChannel::hasMultipleHits (){
  return (this->hasHit() && this->_vectorOfhits->size() > 1);
}

void RPCLinkBoardChannel::writeMultiHitDifferences(const int & timeWindow){
  int timeOfFirstHit = 0;
  if (this->hasMultipleHits()){
    //cout << "multi hits " << endl;
    // discard hits with values less then 16000 for CERN case, find the first with higher value
    
    timeOfFirstHit = this->getHits().at(0);
    int properStartTime = 0;
    
    for (int i = 0 ; i < this->getHits().size() - 1 && timeOfFirstHit > 16000; i++) {
      if (timeWindow == 250 && timeOfFirstHit < 16000) {
	timeOfFirstHit = this->getHits().at(i);
	if (timeOfFirstHit < 16000) continue;
      }
      
      if ( this->getHits().at(i+1) + timeWindow > timeOfFirstHit ) break;
      
      this->timeEvolutionDifferences.push_back( this->getHits().at(i+1) - this->getHits().at(i) );
      //cout << this->getHits().at(i+1) << " " << this->getHits().at(i) << endl;
    }
    //cout << endl;
  }
}

RPCLinkBoardChannel::RPCLinkBoardChannel(){
  this->setEfficiency(0); // well, should have default value
  this->resetEfficiencyCounters();
}

RPCLinkBoardChannel::~RPCLinkBoardChannel(){
  
}

void RPCLinkBoardChannel::incrementEfficiencyCounters(const bool & hitIsFound){
  if(hitIsFound) this->efficientTracks++;
  this->allTracks++;
}

void RPCLinkBoardChannel::resetTimeEvoEntriesVector(){
  this->timeEvolutionDifferences.clear();
}

void RPCLinkBoardChannel::resetAllCounters(){
  this->resetTimeEvoEntriesVector();
  this->resetEfficiencyCounters();
  this->resetNumberOfCounts();
  this->resetNumberOfNoisyCounts();
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

void RPCLinkBoardChannel::incrementNumberOfCountsOutOfReferenceWindow(const int & reference,const int & window){
  int minimum = reference - window;
  int maximum = reference + window;
  for (int i = 0 ; i < this->getHits().size() ; i++){
    if ( this->getHits().at(i) <  minimum  ){
      this->numberOfNoisyCounts ++;
    }
  }
}
