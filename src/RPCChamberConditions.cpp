#include "../interface/RPCChamberConditions.h"

using namespace std;

RPCChamberConditionsBase::~RPCChamberConditionsBase(){
  
}

RPCChamberConditionsBase::RPCChamberConditionsBase (){
  
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

void RPCChamberConditions::setFEBTresholds(const vector<int> & FEBtresholds) {
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

const double & RPCChamberConditions::getHVsetForGap(const int & gapNumber){
  assert(this->getGapsVset().size() <= gapNumber);
  return this->getGapsVset().at(gapNumber-1);
}

const double & RPCChamberConditions::getCurrentForGap(const int & gapNumber){
  assert(this->getGapsCurrent().size() <= gapNumber);
  return this->getGapsCurrent().at(gapNumber-1);
}

const string & RPCChamberConditions::getLabelForGap(const int & gapNumber){
  assert(this->getGapLabels().size() <= gapNumber);
  return this->getGapLabels().at(gapNumber-1);
}

const int & RPCChamberConditions::getFEBTresholdForBoard(const int & febNumber){
  assert(this->_FEBtresholds.size() <= febNumber);
  return this->_FEBtresholds.at(febNumber-1);
}

RPCChamberConditions::RPCChamberConditions(){
  
}

RPCChamberConditions::~RPCChamberConditions(){
  
  this->gapsCurrent.clear();
  this->gapsLabel.clear();
  this->gapsVmon.clear();
  this->gapsVset.clear();
  this->_FEBtresholds.clear();
  
}

