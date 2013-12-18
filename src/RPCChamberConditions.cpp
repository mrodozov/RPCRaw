#include "../interface/RPCChamberConditions.h"

using namespace std;

RPCChamberConditionsBase::~RPCChamberConditionsBase(){
  
}

void RPCChamberConditions::setHVmon(const int & hvmonvalue){
  this->_hvMon = hvmonvalue;
}

const int & RPCChamberConditions::getHVmon(){
  return this->_hvMon;
}

void RPCChamberConditions::setHVset(const int & hvSetvalue) {
  this->_hvSet = hvSetvalue;
}

const int & RPCChamberConditions::getHVset() {
  return this->_hvSet;
}

void RPCChamberConditions::setChamberName(const string & name) {
  this->_chamberName = name;
}

const string & RPCChamberConditions::getChamberName() {
  return this->_chamberName;
}

void RPCChamberConditions::setShelfNumber (const int & shelfNumber) {
  this->_shelfNumber = shelfNumber;
}

const int & RPCChamberConditions::getShelfNumber () {
  return this->_shelfNumber;
}

void RPCChamberConditions::setGapLabels(const vector<string> & labels) {
  this->gapsLabel = labels;
}

const vector<string> & RPCChamberConditions::getGapLabels() {
  return this->gapsLabel;
}

void RPCChamberConditions::setGapsCurrent(const vector<double> & currents) {
  this->gapsCurrent = currents;
}

const vector<double> & RPCChamberConditions::getGapsCurrent() {
  return this->gapsCurrent;
}

void RPCChamberConditions::setGapsVmon(const vector<double> & gapsVmon) {
  this->gapsVmon = gapsVmon;
}

const vector<double> & RPCChamberConditions::getGapsVmon() {
  return this->gapsVmon;
}

void RPCChamberConditions::setGapsVset(const vector<double> & gapsVset) {
  this->gapsVset = gapsVset;
}

const vector<double> & RPCChamberConditions::getGapsVset() {
  return this->gapsVset;
}

void RPCChamberConditions::setFEBtresholds(const vector<int> & FEBtresholds) {
  this->_FEBtresholds = FEBtresholds;
}

const vector<int> & RPCChamberConditions::getFEBTresholds() {
  return this->_FEBtresholds;
}

/** additional optional methods */

const double & RPCChamberConditions::getHVmonForGap(const int & gapNumber){
  assert(this->getGapsVmon().size() <= gapNumber);
  return this->getGapsVmon().at(gapNumber-1);
}

