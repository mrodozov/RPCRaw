/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#include <fstream>
#include <sstream>
#include "../../interface/RollCore/ExtendedStrip.h"
#include "../../interface/RollCore/ExtendedRoll.h"
#include "../../interface/RollCore/Chip.h"
#include "../../interface/ROOT.h"

using namespace std;

ChipBaseClass::ChipBaseClass(){
  // empty base class constructor
}

ChipBaseClass::~ChipBaseClass(){
  // empty base class destructor
}

Chip::Chip(){  //cout << "BLABLA" << endl;
  
}

Chip::~Chip(){
  
}

ExStrip* Chip::getPointerToStripPointers(){
  
  return dynamic_cast<ExStrip*>(this->stripPointers);
  
}

void Chip::setStripsPointersToPointToObjects(StripInterface * Allocated_ExStrip_Objects){
  
  this->stripPointers = Allocated_ExStrip_Objects;
  
}

void Chip::assignStripsFromRoll(RollInterface* aRoll){
  
  ExStrip * pointerToStrips[8];
  
  for(int i=0;i < 8;i++){
     pointerToStrips[i] =  dynamic_cast<ExRoll*>(aRoll)->getStrip(this->getChipNumber()*8+(i+1));    
  }
  
  this->setStripsPointersToPointToObjects(*pointerToStrips);
  
}

const int & Chip::getChipNumber(){
  return this->chipNumber;
}

void Chip::setChipNumber(const int & theNumber){
  this->chipNumber=theNumber;
}

ExStrip* Chip::getChipStrip(const int & stripNumber){
    
  return & this->getPointerToStripPointers()[stripNumber-1];
  
}

void Chip::setChipID(int ID){
  this->chipID = ID;
}


const int & Chip::getChipID(){
  return this->chipID;
}


double Chip::getMeanValueOfStrips(){
  double value=0;
  double maxValue=0;
  //TH1F * Histo = new TH1F("name","title",1000,0,);
  
  for(int i = 1 ; i <= 8 ; i++){
    if(this->getChipStrip(i)->getRate() > maxValue){
      maxValue = this->getChipStrip(i)->getRate();
    }
  }
  
  TH1F * Histo = new TH1F("name","title",1000,0,maxValue);
  for(int i = 1 ; i <= 8;i++){
    if( ! this->getChipStrip(i)->getIsDisconnected()){
      Histo->Fill(this->getChipStrip(i)->getRate());
    }
  }
  
  value = Histo->GetMean();
  delete Histo;
  if (maxValue == 0){
    cout << "maxValue is zero ! " << endl;
    value = maxValue;
  }
  
  return value;
}

double Chip::getAverageValueOfStrips(){
  double sum=0;
  int counter=0;
  double result;
  for (int i = 1 ; i <= 8 ; i++){
    if( ! this->getChipStrip(i)->getIsDisconnected() && this->getChipStrip(i)->getRate() != 0){
      sum += this->getChipStrip(i)->getRate();
      counter++;
    }
  }
  
  if(counter > 0){
    result = (sum/counter);
  }
  else{
    result = 0;
  }
  
  return result;
  
}

double Chip::getMaxRateValueOfChipStrips(){
  double maxValue=0;
  for(int i=1 ; i <= 8 ; i++){
    if(this->getChipStrip(i)->getRate() > maxValue){
      maxValue = this->getChipStrip(i)->getRate();
    }
  }
  return maxValue;
}


TH1F* Chip::getHistoOfChipStrips(const string & runString){
  stringstream SS;
  string ID_as_String;
  SS << this->getChipID();
  SS >> ID_as_String;
  SS.clear();
  
  TH1F * StripsRates = new TH1F ((runString+"_"+ID_as_String+"_"+"stripRates").c_str(),("Strip rates for Chip "+ID_as_String).c_str(),8,0,8);
  for(int i = 1 ; i <= 8 ; i ++){
    StripsRates->SetBinContent(i,this->getChipStrip(i)->getRate());
  }
  StripsRates->SetLineColor(kCyan);
  
  return StripsRates;
  
}

TH1F* Chip::getDistributionOfChipsStripRateValues(const string & runString){
  
  stringstream SS;
  string ID_as_String;
  SS << this->getChipID();
  SS >> ID_as_String;
  SS.clear();
  
  TH1F * StripsDistribution = new TH1F ((runString+"_"+ID_as_String+"_strip_distr").c_str(),("Distribution of chip strips chip ID "+ID_as_String).c_str(),100,0,this->getMaxRateValueOfChipStrips()+5);
  for(int i = 1 ; i <= 8 ; i++){
    StripsDistribution->Fill(this->getChipStrip(i)->getRate());
  }
  StripsDistribution->SetLineColor(kRed);
  
  return StripsDistribution;
  
}