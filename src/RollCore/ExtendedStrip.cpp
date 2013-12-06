/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#include <iostream>
#include "../../interface/RollCore/ExtendedStrip.h"

using namespace std;

ExStrip::~ExStrip(){
  //cout << "ExStrip destructor " << endl;
}

ExStrip::ExStrip():Strip(){
  
  this->isToMask=false;
  this->isToUnmask=false;
  this->setRate(0);
  this->setmaxRate(0);
  this->setTimeAsNoisy(0);
  
}

ExStrip::ExStrip(const string& W_D, const int& W_D_N, const int& sec, const string& R_ID, const string& R_ID_N, const int& num): Strip(W_D, W_D_N, sec, R_ID, R_ID_N, num){
  
  this->isToMask=false;
  this->isToUnmask=false;
  this->setRate(0);
  this->setmaxRate(0);
  this->setTimeAsNoisy(0);
  
}

void ExStrip::ClearConditions(){
  
  Strip::ClearConditions();
  this->isToUnmask = false;
  this->isToMask = false;
  this->rate = 0;
  this->timeWhileNoisy = 0;
  this->maxRate = 0;
    
}

const double & ExStrip::getRate(){
  
  return this->rate;
  
}

void ExStrip::setRate(const double & rate_){
  this->rate = rate_;
}

const double& ExStrip::getMaxRate(){
  return this->maxRate;
}

const double& ExStrip::getTimeAsNoisy(){
  return this->timeWhileNoisy;
}

const bool& ExStrip::isAboutToBeMasked(){
  return this->isToMask;
}

const bool& ExStrip::isAboutToBeUnmasked(){
  return this->isToUnmask;
}

void ExStrip::setmaxRate(const double& max_Rate){
  this->maxRate = max_Rate;
}

void ExStrip::setTimeAsNoisy(const double& time_as_noisy){
  this->timeWhileNoisy = time_as_noisy;
}

void ExStrip::setToBeMasked(){
  this->isToMask = true;
}

void ExStrip::setToBeUnmasked(){
  this->isToUnmask = true;
}

void ExStrip::unsetToBeMasked(){
  this->isToMask = false;
}

void ExStrip::unsetToBeUnmasked(){
  this->isToUnmask = false;
}

void ExStrip::initWithValues(const string& W_D, const int& W_D_N, const int& sec, const string& R_ID, const string& R_ID_N, const int& num)
{
  
  Strip::initWithValues(W_D, W_D_N, sec, R_ID, R_ID_N, num);
  this->isToMask=false;
  this->isToUnmask=false;
  this->setRate(0);
  this->setmaxRate(0);
  this->setTimeAsNoisy(0);
  
}
