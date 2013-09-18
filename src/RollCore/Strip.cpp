/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <new>
#include "../../interface/RollCore/Strip.h"

using namespace std;

StripInterface::~StripInterface(){ 
  //cout << "StripIface destructor" << endl;
}

const bool & Strip::getIsMasked(){
  return isMasked;
}

const bool & Strip::getIsDead(){
  return isDead;
}

const bool & Strip::getIsDisconnected(){
  return isUnpluged;
}

const int & Strip::getOnlineNumber(){
  return number;
}

const int & Strip::getOfflineNumber(){
  return this->offlineNumber;
}

  // mutators
void Strip::Mask(){
  isMasked = true;
}
  
void Strip::Kill(){
  isDead = true;
}

void Strip::Disconnect(){
  isUnpluged = true;
}
 
void Strip::unsetMasked(){
  isMasked = false;
}

void Strip::unsetDead(){
  isDead = false;
}

void Strip::SetNotDisconnected(){
  isUnpluged = false;
}

void Strip::setOfflineID(const std::string & newID){
    offlineID  = newID;
}

void Strip::setOfflineNumber(const int & newNumber){
  
  this->offlineNumber = newNumber;
    // newNumber = 0; // assignment of read-only element is not allowed, this ensures that this reference newMember would not be changed
}

void Strip::setOnlineNumber(const int & newNumber){
  
  this->number = newNumber;
  
}

void Strip::ClearConditions(){
  
  this->unsetDead();
  this->unsetMasked();
  
}


void Strip::printOutStatus(){
  
  cout << wheel_disk << " " << wheel_disk_num  <<" "<< sector <<" "<< roll_id <<" "<<roll_id_num<<" "<< number <<" "<< isMasked <<" "<< isDead <<" "<< isUnpluged << endl;
  
}


const std::string & Strip::getOfflineID(){
  return offlineID;
}

Strip::Strip(){
  this->offlineID = "";
  this->offlineNumber = 0;
  //initStripValues();
}//in case that i need array of strips


void Strip::initWithValues(const  std::string & W_D,const int & W_D_N,const int & sec,const std::string & R_ID,const std::string & R_ID_N,const int & num){
  
  wheel_disk = W_D;
  wheel_disk_num = W_D_N; 
  sector = sec;
  roll_id= R_ID;
  roll_id_num = R_ID_N;
  number = num;
  isDead = 0;
  isMasked = 0;
  isUnpluged = 0; 
  this->offlineID = "";
  this->offlineNumber = 0;
  //initStripValues();
  
}


Strip::Strip(const std::string & W_D,const int & W_D_N,const int & sec,const std::string & R_ID,const std::string & R_ID_N,const int & num){
  
  initWithValues( W_D, W_D_N, sec, R_ID, R_ID_N, num);
  
}

Strip::~Strip(){
//   cout << "Strip destructor" << endl;
  //delete offlineNumber;
  //delete [] offlineID; // -> the error is somewhere in the class Roll , single Strip object doesn't complain about this delete
}