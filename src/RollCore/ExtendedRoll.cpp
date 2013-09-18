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
#include <cstdlib>
#include "../../interface/RollCore/ExtendedRoll.h"
#include "../../interface/RollCore/Chip.h"
#include "../../interface/ROOT.h"

using namespace std;

void ExRoll::allocStrips(){
  
  this->strips = new ExStrip[96];
  
}

void ExRoll::initStrips(){
  
  Roll::initStrips();
  this->stripArea = new double[this->getClones()];
  // init other values of the strips
}

ExStrip * ExRoll::getStrip(int element){
  
  return & getStripCollection()[element-1];
  
}

ExStrip * ExRoll::getStripCollection(){
  
  return dynamic_cast<ExStrip*>(this->strips) ;
  
}

ExRoll::ExRoll(const string& RollOnlineName): Roll(RollOnlineName){
  
  //cout << "ExRoll string constructor" << endl;  
  this->NumberOfChipsAssigned = 0;
  this->towerName = "Not Assigned !";
  
}

ExRoll::ExRoll() : Roll(){
  
  //cout << "ExRoll constructor" << endl;
  this->towerName = "Not Assigned !";
  this->NumberOfChipsAssigned = 0;
    
}

ExRoll::~ExRoll(){
  
  delete [] this->getStripCollection();
  delete [] this->stripArea;
  this->strips = NULL;//causes seg fault because delete called also from the base constructor, after it is called from the derived
  //cout << "ExRoll destructor" << endl; // debug line
  
}

// not necc. to override

void ExRoll::initWithValues(const string& W_D, const int& W_D_N, const int& sec, const string& roll_id, const string& roll_id_num){
  
  Roll::initWithValues(W_D, W_D_N, sec, roll_id, roll_id_num);
  
}

void ExRoll::initWithValues(const string& RollNameString){
  
  Roll::initWithValues(RollNameString);
  
}

Chip * ExRoll::getChipByNumber(const int& ChipNumber){
  
  return dynamic_cast<Chip*>(this->Chips.find(ChipNumber)->second);
  
}

const int & ExRoll::getNumberOfChipsAssigned(){
  
  return this->NumberOfChipsAssigned;
  
}

void ExRoll::incrementNumberOfChips(){
  
  this->NumberOfChipsAssigned++;
  
}

void ExRoll::insertNewChipToChipsMap(const int& num, ChipBaseClass * abstractObject){
  this->Chips[this->getNumberOfChipsAssigned()-1] = abstractObject;
}

void ExRoll::insertNewChipWithNumberAndID(const int & num,const int & ID){
  
  this->incrementNumberOfChips(); // they should be zero assigned by the constructor
  Chip * aChip = new Chip; // this to be changed onl when this method is overloaded by the next class if needed
  aChip->setChipNumber(num);
  aChip->setChipID(ID);
  aChip->assignStripsFromRoll(this);
  //this->Chips[getNumberOfChipsAssigned()-1] = aChip; // make this not directly - ahaaa ! not to use the pointer directly and to remain private for the class. its not directly
  this->insertNewChipToChipsMap(num,aChip);
}

void ExRoll::assignChipsFromSource(map< string, std::map< int, int > > ChipsMap){
  for (int i=1; i <= this->getClones() ; i++){
    for(map<int,int>::iterator it = ChipsMap.find(this->getRollIDofClone_withEtaPartSeparated(i))->second.begin() ;
	it != ChipsMap.find(this->getRollIDofClone_withEtaPartSeparated(i))->second.end() ; it++){
      this->insertNewChipWithNumberAndID(it->first,it->second);
    }
  }
}

void ExRoll::setRollRate(const double& rollRate){ this->rollAvgRate = rollRate; }

const double& ExRoll::getRollRate(){return this->rollAvgRate;}

double ExRoll::getSumOfStripRates(){
  double sumOfStripRates=0;
  for (int i = 0 ; i < 96 ; i++){
    sumOfStripRates += this->getStrip(i+1)->getRate();
  }
  return sumOfStripRates;
}

int ExRoll::getStripsToBeMaskedFromClone(const int & cloneNum){
  int ToBeMaskedInThisClone=0;
  for (int i=(cloneNum-1)*(96/getClones()); i < cloneNum*(96/getClones()) ; i++ ){
    if(getStrip(i+1)->isAboutToBeMasked()){
	ToBeMaskedInThisClone ++;
    }
  }
  return ToBeMaskedInThisClone;
}

int ExRoll::getStripsToBeUnmaskedFromClone(const int & cloneNum){
  int ToBeUnmaskedInThisClone=0;
  for (int i=(cloneNum-1)*(96/getClones()); i < cloneNum*(96/getClones()) ; i++ ){
    if(getStrip(i+1)->isAboutToBeUnmasked()){
	ToBeUnmaskedInThisClone ++;
    }
  }
  return ToBeUnmaskedInThisClone;
}

double ExRoll::getRollRateFromClone(const int & cloneNum){
  double rateOfClone=0;
  for (int i=this->getFirstStripNumberOfClone(cloneNum); i <= this->getLastStripNumberOfClone(cloneNum) ; i++ ){
    rateOfClone += getStrip(i)->getRate();
  }
  return rateOfClone;
}

double ExRoll::getRollRatePSCFromClone(const int & cloneNum){
  double ratePCS;
  int workingStripsInThisClone = (96/getClones())-getMaskedFromClone(cloneNum)-getUnplugedFromClone(cloneNum);
  ratePCS = this->getRollRateFromClone(cloneNum)/(workingStripsInThisClone*this->getStripsAreaFromClone(cloneNum));
  if (this->getRollRateFromClone(cloneNum) == 0 || workingStripsInThisClone == 0){
    ratePCS = 0;
  }
  return ratePCS;
}

const string& ExRoll::getLBstring(){
  return this->LBstring;
}

void ExRoll::setLBstring(const string& theLBString){
  this->LBstring = theLBString;
}

void ExRoll::setStripsToBeMaskedFromSource(const DataObject & objectWithStripsToBeMasked){
  
  if(objectWithStripsToBeMasked.ifKeyExist(this->getFullOnlineRollID())){
    string curr_num;
    vector<string> thevec = objectWithStripsToBeMasked.getMapOfVectors().find(this->getFullOnlineRollID())->second;
    //cout << "the size is " << thevec.size()  << endl;
    for(int i = 0 ; i < thevec.size() ; i++){	
      curr_num = thevec[i];
      this->getStrip(atoi(curr_num.c_str()))->setToBeMasked();
    }
  }
}

void ExRoll::setStripsToBeUnmaskedFromSource(const DataObject & objectWithStripsToBeUnmasked){
  
  if(objectWithStripsToBeUnmasked.ifKeyExist(this->getFullOnlineRollID())){
    string curr_num;
    vector<string> thevec = objectWithStripsToBeUnmasked.getMapOfVectors().find(this->getFullOnlineRollID())->second;
    //cout << "the size is " << thevec.size()  << endl;
    for(int i = 0 ; i < thevec.size() ; i++){	
      curr_num = thevec[i];
      this->getStrip(atoi(curr_num.c_str()))->setToBeUnmasked();
    }
  }  
}

void ExRoll::setStripsToBeUnmasked_withMaxRate_FromSource(const DataObject& ObjectWithTU_with_maxrate){
  for (int i = 0 ; i < ObjectWithTU_with_maxrate.getLenght() ; i++){
    if(this->getFullOnlineRollID() == ObjectWithTU_with_maxrate.getElement(i+1,1)){
      this->getStrip(ObjectWithTU_with_maxrate.getElementAsInt(i+1,2))->setmaxRate(ObjectWithTU_with_maxrate.getElementAsDouble(i+1,3));
      this->getStrip(ObjectWithTU_with_maxrate.getElementAsInt(i+1,2))->setToBeUnmasked();
    }
  }
}

void ExRoll::setDeadStripsWithMaxRate(const DataObject& objWithDeadAndMaxRate){
  for(int i = 1;i <= objWithDeadAndMaxRate.getLenght();i++){
    if(this->getFullOnlineRollID() == objWithDeadAndMaxRate.getElement(i,1)){
      this->getStrip(objWithDeadAndMaxRate.getElementAsInt(i,2))->setmaxRate(objWithDeadAndMaxRate.getElementAsDouble(i,3));
      this->getStrip(objWithDeadAndMaxRate.getElementAsInt(i,2))->Kill();
    }
  }
}

void ExRoll::setStripsToBeMasked_withTheTime_andMaxRate_FromSource(const DataObject& ObjectWithTM_time_maxrate){
  for(int i = 0 ; i < ObjectWithTM_time_maxrate.getLenght() ; i++){
    if(this->getFullOnlineRollID() == ObjectWithTM_time_maxrate.getElement(i+1,1)){
      this->getStrip(ObjectWithTM_time_maxrate.getElementAsInt(i+1,2))->setmaxRate(ObjectWithTM_time_maxrate.getElementAsDouble(i+1,4));
      this->getStrip(ObjectWithTM_time_maxrate.getElementAsInt(i+1,2))->setTimeAsNoisy(ObjectWithTM_time_maxrate.getElementAsDouble(i+1,3));
      this->getStrip(ObjectWithTM_time_maxrate.getElementAsInt(i+1,2))->setToBeMasked();
    }
  }
  
}

void ExRoll::setStripsRateFromSource(const DataObject & objectWithStripsRates){
  int didFoundRollName = 0;
  for(int i=0;i < objectWithStripsRates.getLenght();i++){
    if(objectWithStripsRates.getElementFromPropertyContainer(i+1,1) == getFullOnlineRollID()){
      didFoundRollName = 1;
      double stripRate;
      stringstream ss;
      for (int j=1;j < 97;j++){
	ss << objectWithStripsRates.getElementFromPropertyContainer(i+1,j+1);
	ss >> stripRate;
	ss.clear();
	getStrip(j)->setRate(stripRate);
	rollAvgRate += stripRate;
      }
    }
    
  }
  if(didFoundRollName == 0){
    rollAvgRate = -99;
  }
}

void ExRoll::setStripsRateFromRootFileSource(const string & fileName){
  TH1F *h1;
  TFile * file = new TFile(fileName.c_str(),"READ","in");
  //TChain *globChain = 0;
  TIter nextkey( file->GetListOfKeys() ); // its a ROOT iterator object it seems
  TKey *key;
  while  (key = (TKey*)nextkey()) { 
    // assign key to the 
    h1 = (TH1F*)key->ReadObj(); 
    if(getFullOnlineRollID() == h1->GetName()){ // absolutely beautiful statement - nothing else necessary
      
      int * i = new int;
      *i = 0;
      for (*i = 1; *i < 97 ; *i=*i+1){
        getStrip(*i)->setRate(h1->GetBinContent(*i)); // aaaaand thats it !
        rollAvgRate += h1->GetBinContent(*i);
      }
      delete i;
    }
     //else {rollAvgRate = -99;}
    delete h1;
  }
  file->Delete();
  file->Close("R");
  //delete file;
}

void ExRoll::setStripsRateFromRootFileSource(const  TFile & rootFile){
TH1F *h1;
  TChain *globChain = 0;
  TIter nextkey( rootFile.GetListOfKeys() ); 
  TKey *key;
  while  (key = (TKey*)nextkey()) {
    TObject *obj1 = key->ReadObj();
    h1 = (TH1F*)obj1; // casting the object as TH1F
     if(getFullOnlineRollID() == h1->GetName()){ // absolutely beautiful statement - nothing else necessary
       for (int i = 1; i < 97 ; i++){
	 getStrip(i)->setRate(h1->GetBinContent(i)); // aaaaand thats it !
      }
    }
  }
}

void ExRoll::setTowerNameFromMap(map< string, string > towerMap){
  this->setTowerName(towerMap.find(this->getFullOnlineRollID())->second);
}

void ExRoll::setLBnameFromMap(map< string, string > RollName_LBname_Map){
  this->LBstring = RollName_LBname_Map.find(this->getRollIDofClone_withEtaPartSeparated(1))->second;
}

void ExRoll::removeNoisyStripsForCloneWithPercentValue(const int & clone,const int & percents){
  TH1F * histCorrection = new TH1F(this->getRollIDofClone(clone).c_str(),this->getRollIDofClone(clone).c_str(),this->getMaxRateValue()+5,0,this->getMaxRateValue()+5);
  for(int j=(clone-1)*(96/this->getClones()); j < (clone)*(96/this->getClones());j++){
    if(this->getStrip(j+1)->getRate() > 0){
    histCorrection->Fill(this->getStrip(j+1)->getRate());
    }
  }
  for(int j=(clone-1)*(96/this->getClones()); j < (clone)*(96/this->getClones());j++){
    if(this->getStrip(j+1)->getRate() > (histCorrection->GetMean() + (percents/100)*histCorrection->GetMean())){
      this->getStrip(j+1)->setRate(0); // dispose of the strips with values > mean of all strips rate values + percent*mean_value
    }
    
  }
  delete histCorrection;
  
}

void ExRoll::removeNoisyStripsForAllClonesWithPercentValue(const int & percent){
  for(int i=1;i <= this->getClones();i++){
    this->removeNoisyStripsForCloneWithPercentValue(i,percent);
  }
  // correction for all clones
}

int ExRoll::getFirstStripNumberOfClone(const int & cloneNumber){
  return ((cloneNumber-1)*(96/this->getClones()) + 1);
}

int ExRoll::getLastStripNumberOfClone(const int & cloneNumber){
  return (cloneNumber*(96/this->getClones()));
}

string ExRoll::getPreviousNeighbourName(){
  string returnValue,plus="";
  if (getWheel_Disk_Number() > 0){
    plus="+";
  }
  stringstream SS;
  
  if(this->isInEndcap()){
    string prev;
    if (atoi(this->getRollID().c_str()) == 36){
      prev = "35";
    }
    else if(atoi(this->getRollID().c_str()) == 1){
      prev = "36";
    }
    else {
      SS << (atoi(this->getRollID().c_str()) - 1);
      SS >> prev;
      SS.clear();
      if(prev.length() == 1){
	prev = "0"+prev;
      } 
    }
    
    returnValue = getWheel_Disk()+plus+returnFieldAsString(getWheel_Disk_Number())+"_"+returnFieldAsString(getSector())+"_"+prev;
  
  }
    // if in barrel
    if (this->isInBarrel()){
      int prev;
	if(this->getSector() == 12){
	  prev = 11;
	}
	else if(this->getSector() == 1){
	  prev = 12;
	}
	else{
	  prev = this->getSector() - 1;
	}
	returnValue = getWheel_Disk()+plus+returnFieldAsString(getWheel_Disk_Number())+"_"+getRollID()+"_"+returnFieldAsString(prev)+getOptionalRollID();
  }
  
  return returnValue;
  
}

string ExRoll::getNextNeighbourName(){
  string returnValue,plus="";
  if (getWheel_Disk_Number() > 0){
    plus="+";
  }
  stringstream SS;
  
  if(this->isInEndcap()){
    string next;
    if (atoi(this->getRollID().c_str()) == 36){
      next = "01";
    }
    else if(atoi(this->getRollID().c_str()) == 1){
      next = "02";
    }
    else {
      SS << (atoi(this->getRollID().c_str()) + 1);
      SS >> next;
      SS.clear();
      if(next.length() == 1){
	next = "0"+next;
      } 
    }
    
    returnValue = getWheel_Disk()+plus+returnFieldAsString(getWheel_Disk_Number())+"_"+returnFieldAsString(getSector())+"_"+next;
  
  }
    // if in barrel
    if (this->isInBarrel()){
      int next;
	if(this->getSector() == 12){
	  next = 1;
	}
	else if(this->getSector() == 1){
	  next = 2;
	}
	else{
	  next = this->getSector() + 1;
	}
	returnValue = getWheel_Disk()+plus+returnFieldAsString(getWheel_Disk_Number())+"_"+getRollID()+"_"+returnFieldAsString(next)+getOptionalRollID();
  }
  
  return returnValue;
  
}

double ExRoll::getAvgRatePSCWithoutCorrections(){
  double number = 96 - getStripsWithZeroRate();
  if (number == 0){
    return 0;
  }
  else{
  return (this->getSumOfStripRates()/((number/96)*this->getChamberFullArea()) );}
  // here the var number represents a value from 0 to 1 which normalize the area amount with respect to the total number of working strips (non zero rate)
}

double ExRoll::getAvgRatePSCWithoutCorrectionsForClone(const int & clone){
  int numberOfStripsWithZeroRateForHere = this->getTotalChannelsOfOneClone();
  int allZeroRates=0;
  for(int i = this->getFirstStripNumberOfClone(clone); i <= this->getLastStripNumberOfClone(clone);i++){
    if(this->getStrip(i)->getRate() == 0){
      numberOfStripsWithZeroRateForHere --;
      allZeroRates++;
    }
  }
  
  double retVal = (this->getRollRateFromClone(clone)/((this->getTotalChannelsOfOneClone()-allZeroRates)*this->getStripsAreaFromClone(clone)));
  
  if(this->getRollRateFromClone(clone) == 0){
    retVal = 0;
  }
  
  return retVal;
  
}

double ExRoll::getFullAreaForClone(const int & cloneNum){
  return this->getStripsAreaFromClone(cloneNum)*(96/this->getClones());
}

double ExRoll::getChamberFullArea(){
  double fullArea = 0 ;
  for (int i=0 ; i < this->getClones() ; i++){
    fullArea +=this->getFullAreaForClone(i+1);
  }
  return fullArea;
}

int ExRoll::getTotalChannelsOfOneClone(){
  return 96/this->getClones();
}

string ExRoll::getShortFullIDofClone(const int & cloneNum){
  string firstPart,secondPart,finalPart;
  
  if(this->isInBarrel()){
    
    firstPart = this->getRollIDofCloneWithNewIdentifiers(cloneNum);
    firstPart = firstPart.substr(firstPart.find("RB"));
    firstPart = firstPart.substr(0,firstPart.find("_")+1);
    secondPart = this->getRollOfflineIDofClone(cloneNum).substr(0,1);
    // one private case - if sector 9 or 11 , after RB4 put a - sign
    if(firstPart=="RB4_"){firstPart="RB4-_";}
    finalPart = firstPart+secondPart;
  }
  else{
    finalPart = "R" + this->returnFieldAsString(abs(this->getSector())) + "_" + this->getRollOfflineIDofClone(cloneNum);
  }
  return finalPart;
  
}

void ExRoll::setTowerName(const string& tower_Name){
  this->towerName = tower_Name;
}

const string& ExRoll::getTowerName(){
  return this->towerName;
}

int ExRoll::getStripsWithZeroRate(){
  int number=0;
  for (int i = 0 ; i < 96 ; i ++){
    if (this->getStrip(i+1)->getRate() == 0){
      number++;
    }
  }
  return number;
}

double ExRoll::getMaxRateValue(){
  double maxRate=0;
  for(int i=1;i <= 96l;i++){
    if(this->getStrip(i)->getRate() > maxRate){
      maxRate = this->getStrip(i)->getRate();
    }
  }
  return maxRate;
}

void ExRoll::setStripsRatesFromTH1FObject(TH1F * RateHistogram){
  for (int i=0; i < 96; i++){ // dangerous, but 
    this->getStrip(i+1)->setRate(RateHistogram->GetBinContent(i+1));
  }
}

void ExRoll::setStripsAreaFromSource_cmsswResource(const DataObject& objectWithAreas){
  for (int i = 1 ; i < getClones() + 1 ; i++){
    this->setStripsAreaForClone(objectWithAreas.getElementByKeyAsDouble(this->getRollIDofCloneWithNewIdentifiers(i),1),i);    
  }
}

void ExRoll::setRollRawIDs_UsingNewIDsFile(const DataObject& fileWithNewRawIDs){
  for (int i = 1; i <= this->getClones();i++){
    this->setRawIDofClone(i-1,fileWithNewRawIDs.getElementByKeyAsInt(this->getRollIDofCloneWithNewIdentifiers(i),1));
  }
}

string ExRoll::getOpositeRollIDEcapOnly(){
  string resultString;
  string buffer =  this->getFullOnlineRollID();
  if(this->isInEndcap()){
    if(this->getWheel_Disk_Number() < 0 );
    //resultString = buffer.substr(0,3);
    resultString += "RE+"+buffer.substr(3,buffer.length());
    if(getWheel_Disk_Number() > 0){
    resultString = "RE-"+buffer.substr(3,buffer.length());
    //resultString += "RE-"+resultString;
    }
  }
  else{
    resultString = this->getFullOnlineRollID();
  }
  return resultString;
}



TH1F * ExRoll::getRefitedRateHistoForClone(const int & cloneNum){
  int NonZeroRateStrips = 96/getClones();
  for(int i=(cloneNum-1)*(96/getClones());i< (cloneNum)*(96/getClones());i++){
    //get the number of strips with rate > 0 : not masked or dead
    if(getStrip(i+1)->getRate() == 0){
      NonZeroRateStrips --;
    }
  }
  
  int bin = 0;
  TH1F * regularHisto = new TH1F("h1","Some Name for check",NonZeroRateStrips,0,NonZeroRateStrips);
  for (int j = (cloneNum-1)*(96/getClones());j < (cloneNum)*(96/getClones());j++){
    Double_t rate_t = getStrip(j+1)->getRate();
    if(rate_t > 0){
    bin ++;
    regularHisto->SetBinContent(bin,rate_t);
    }
  }
  // just check for now 
  TF1 * func = new TF1("f1","pol0",0,NonZeroRateStrips);
  
  regularHisto->Fit(func);
  double fitResult = func->GetParameter(0);
  delete regularHisto;
  // and start again
  
  bin = 0;
  TH1F * refitedHisto = new TH1F("h1",(getRollIDofCloneWithNewIdentifiers(cloneNum)).c_str(),NonZeroRateStrips,0,NonZeroRateStrips);
  TF1 * func2 = new TF1("f2","pol0",0,NonZeroRateStrips);
  for (int j = (cloneNum-1)*(96/getClones());j < (cloneNum)*(96/getClones());j++){
    Double_t rate_t = getStrip(j+1)->getRate();
    if(rate_t > 0){
      bin ++;
      if(rate_t > fitResult){
	rate_t = fitResult;
      }
      refitedHisto->SetBinContent(bin,rate_t);
    }
  }
  //refitedHisto->Fit(func2);
  //cout << func2->GetParameter(0) << endl;
  delete func,func2;
  return refitedHisto;
}

TH1F* ExRoll::getCuttedAndFitedRateHistoForClone(const int& cloneNum,const int & cutValue){
  int NonZeroRateStrips = 96/getClones();
  for(int i=(cloneNum-1)*(96/getClones());i< (cloneNum)*(96/getClones());i++){
    //get the number of strips with rate > 0 : not masked or dead
    if(getStrip(i+1)->getRate() == 0){
      NonZeroRateStrips --;
    }
  }
  int bin = 0;
  TH1F * regularHisto = new TH1F("regularHisto","Cutted rate histo for non empty bins",NonZeroRateStrips,0,NonZeroRateStrips);
  for (int j = (cloneNum-1)*(96/getClones());j < (cloneNum)*(96/getClones());j++){
    Double_t rate_t = getStrip(j+1)->getRate();
    if(rate_t > 0){
    bin ++;
    if(cutValue < rate_t){
      regularHisto->SetBinContent(bin,cutValue);
    }
    else{regularHisto->SetBinContent(bin,rate_t);  }
    }
  }
  //regularHisto->SaveAs("cut.root");
  return regularHisto;
}

const double & ExRoll::getStripsAreaFromClone(const int & cloneNum){
  return stripArea[cloneNum-1];
}

void ExRoll::WriteResultForDB(std::ofstream& outputFileStream){
  for (int i=0;i < getClones() ; i++){
    if(getRollRate() == -99){
    outputFileStream << getRawIDofClone(i+1)  << " ";
    outputFileStream << getRollIDofCloneWithNewIdentifiers(i+1) << " ";
    outputFileStream << "-99 -99 -99 -99 -99 -99" << "\n";
    }
    // not sure about it for now
    else{
    outputFileStream << getRawIDofClone(i+1)  << " ";
    outputFileStream << getRollIDofCloneWithNewIdentifiers(i+1) << " ";
    outputFileStream << getDeadFromClone(i+1) << " ";
    outputFileStream << getMaskedFromClone(i+1) << " ";
    outputFileStream << getStripsToBeUnmaskedFromClone(i+1) << " ";
    outputFileStream << getStripsToBeMaskedFromClone(i+1) << " ";
    outputFileStream << getRollRateFromClone(i+1) <<" ";
    outputFileStream << getRollRatePSCFromClone(i+1);
    outputFileStream << "\n";
    }
    outputFileStream.clear();
    
  }
}

void ExRoll::WriteDetailedResultsForDB(ofstream& FileForDetailedResults,ofstream & ErrorFile){
  for(int i = 0;i < getClones() ; i++){
    for(int j = i*(96/getClones()) ; j < (i+1)*(96/getClones()) ; j++){
      
      // i = 0 -> j = 0*(32) to j = 1*(32) ; // i = 1 ; // i = 2 ;
      //FileForDetailedResults <<getFullOnlineRollID() << " "; // remove in case you want the Online Roll name
      FileForDetailedResults << this->getRawIDofClone(i+1) << " ";
      //FileForDetailedResults << this->getRollIDofCloneWithNewIdentifiers(i+1) << " ";
      FileForDetailedResults << this->getStrip(j+1)->getOnlineNumber() << " ";
      if (getStrip(j+1)->getIsDisconnected() == 1){
	FileForDetailedResults << "-1 ";
      }
      else{
      FileForDetailedResults << this->getStrip(j+1)->getOfflineNumber() << " ";
      }
      if (getRollRate() == -99){
	FileForDetailedResults << "-99 -99 -99 -99" << "\n";
      }
      else{
      
      FileForDetailedResults << this->getStrip(j+1)->getIsDead() << " ";
      FileForDetailedResults << this->getStrip(j+1)->getIsMasked() << " ";
      //FileForDetailedResults << this->getStrip(j+1)->isAboutToBeUnmasked() << " "; // remove in case you want the number of ToBeUnmasked
      //FileForDetailedResults << this->getStrip(j+1)->isAboutToBeMasked() << " "; //remove in case you want the number of ToBeMasked
      //FileForDetailedResults << this->getStrip(j+1)->getRate() << " "; //FileForDetailedResults <<endl;
      FileForDetailedResults << this->getStrip(j+1)->getRate()/getStripsAreaFromClone(i+1) <<" ";
      //FileForDetailedResults << this->getStripsAreaFromClone(i+1) << " "; // remove in case you want the areas of strips
      FileForDetailedResults << "\n";
      if(this->getStrip(j+1)->getRate() < 0){
	ErrorFile << this->getFullOnlineRollID() <<" ";
	ErrorFile << this->getStrip(j+1)->getOfflineNumber() <<" ";
	ErrorFile << this->getStrip(j+1)->getOnlineNumber() <<" ";
	ErrorFile << this->getRollIDofClone(i+1) <<" ";
	ErrorFile << this->getStrip(j+1)->getRate()/getStripsAreaFromClone(i+1) << "\n" ;
      }
      
      }
      ErrorFile.clear();
      FileForDetailedResults.clear(); 
    }
  }
}

void ExRoll::WriteGeomTable(std::ofstream& FileGeomTable){
   for(int i = 0;i < getClones() ; i++){
     FileGeomTable << getRawIDofClone(i+1) << " ";
     FileGeomTable << getFullOnlineRollID() << " ";
     FileGeomTable << getRollIDofCloneWithNewIdentifiers(i+1) << " "; 
     FileGeomTable << getRollIDofClone_withEtaPartSeparated(i+1) << " ";
     FileGeomTable << getStripsAreaFromClone(i+1) << " " << "\n";
     FileGeomTable.clear();
   }
}

void ExRoll::WriteShortDetailedResultsForDB(std::ofstream& FileShortResults){
  for(int i = 0;i < getClones() ; i++){
    for(int j = i*(96/getClones()) ; j < (i+1)*(96/getClones()) ; j++){
    FileShortResults << getRawIDofClone(i+1) << " ";
    FileShortResults << getStrip(j+1)->getOnlineNumber() << " ";
    if (getStrip(j+1)->getIsDisconnected() != 1){
      FileShortResults << getStrip(j+1)->getOfflineNumber() <<" ";
    }
    else{
      FileShortResults << "-1 ";
    }
    if (getRollRate() == -99){
      FileShortResults << "-99 -99 -99" << "\n";
    }
    else{
      FileShortResults << getStrip(j+1)->getIsDead() << " ";
      FileShortResults << getStrip(j+1)->getIsMasked() << " ";
      FileShortResults << getStrip(j+1)->getRate()/getStripsAreaFromClone(i+1) << " " << "\n";
    }
    FileShortResults.clear();
    }
  }
}

void ExRoll::setStripsAreaForClone(const double& StripArea, const int& CloneNum){
  this->stripArea[CloneNum-1] = StripArea;
}

// reset and clear conditions methods

void ExRoll::ClearStripsConditions(){
  
  for(int i = 1 ; i <= 96 ; i++){
    this->getStrip(i)->ClearConditions();
  }
}

void ExRoll::Reset(){
    Roll::Reset(); // reset the Roll fields
}

/** Author: Michele Gabusi  mgabusi@cern.ch
  * Date 08.01.2013
  * Info added at SVN revision 76
  * 
*/

// Methods added by Michele

void ExRoll::setStripsAreaFromMap(map<string, double>& areaMap){

  for(int i=0;i < getClones();i++){
   string idOfClone = getRollIDofCloneWithNewIdentifiers(i+1);
    stripArea[i]=areaMap[idOfClone];
    stripArea[i]=areaMap[idOfClone];
	//cout << i << ") DB " << getRollIDofCloneWithConstructionDBidentifiers(i+1) << " " << stripArea[i]<< endl;
	//cout << i << ") ID " << getRollIDofClone(i+1) << " " << stripArea[i]<< endl;
  }

}

void ExRoll::setStripsRateFromTH1Source(TH1F* h1){
  
  for (int i = 1; i <= h1->GetNbinsX() ; i++){
	double binContent = h1->GetBinContent(i);
         getStrip(i)->setRate(binContent); 

	 if (binContent == 0){
	   getStrip(i)->Disconnect();
	   }

         rollAvgRate += binContent;
       }         
}

double ExRoll::getRollArea(){

  //Roll Area                                                                                                                                                                
  double TotArea = 0;
  double nClones = getClones();

  for (int j=0; j< nClones; j++){
    double val = getStripsAreaFromClone(j) * (96 / nClones - ( getUnplugedFromClone(j+1) + getMaskedFromClone(j+1) ) );
    TotArea += val;
//    cout << "clone: (" << j << ") ---->  Area of clone: " << val << endl;
   }
  return TotArea;
}

//Build an histogram starting from strips content                                                                                                                         
TH1F * ExRoll::getRateHistoForClone(const int & cloneNum){

  int bin = 0;
  double sum = 0 ;
  TH1F * histo = new TH1F("h1",getRollIDofClone(cloneNum).c_str(),96/getClones(),0,96/getClones());
  for (int j = (cloneNum-1)*(96/getClones());j < (cloneNum)*(96/getClones());j++){
    bin ++;
    Double_t rate_t = getStrip(j+1)->getRate();
    cout << bin << " " << rate_t << endl; // debug out                                                                                                                    
    histo->SetBinContent(bin,rate_t);
    sum += rate_t;
  }

//  cout << "average rate for one strip in clone " << cloneNum << " is " << sum/(96/getClones()) << " sum is "<< sum << endl;                                             
//  TF1 * func = new TF1("f1","pol0",0,96/getClones());                                                                                                                   

 // histo->Fit(func);                                                                                                                                                     
  //cout << "parameter is " << func->GetParameter(0) << endl; // parameter obrained                                                                                       
//  delete func;                                                                                                                                                          
  return histo;

  //delete bin;                                                                                                                                                           
}

//Build an histogram starting from a passed TH1


TH1F* ExRoll::getRateHistoForClone(TH1F* totHisto, const int & cloneNum){

        TH1F * histo = new TH1F("h1",getRollIDofClone(cloneNum).c_str(),96/getClones(),0,96/getClones());
        int bin=1;
         for (int j = (cloneNum-1)*(96/getClones());j < (cloneNum)*(96/getClones());j++){
                histo->SetBinContent(bin, totHisto->GetBinContent(j+1));
                bin++;
        }
        return histo;
}

void ExRoll::AddToRate(int rate){
rollAvgRate+= rate;
}

TH1F* ExRoll::buildHistoFromRoll(){

  TH1F* totalHisto = new TH1F("StripRate", "Strip Rate", 96, 0, 96);

  for (int i = 0 ; i < 96 ; i ++){
        totalHisto->SetBinContent( i+1, getStrip(i+1)->getRate() );
  }
        return totalHisto;
}

int ExRoll::getEmptyStrip(){
  int empty = 0;
  double nClones = getClones();
  for (int j=0; j< nClones; j++)
   empty += getUnplugedFromClone(j+1) + getMaskedFromClone(j+1);
  return empty;
}

/*
double ExRoll::CalculateRateFromTH1(bool correct, int clone){

if (clone != -9){}

NoiseAnalyzer ana0;
this->refFile = TFile::Open(this->rootFile.c_str());

TH1F* refHisto = (TH1F*)( refFile->Get( TH1Path.c_str() ) );
TH1F* clHisto  = (TH1F*)( refHisto->Clone() );

TH1F* dist0 = ana0.Associate(this, refHisto, clHisto, correct);

refFile->Close();
double integral = 0;

for (int k=1; k<=96;k++)
  
  integral += getStrip(k)->getRate();
  return integral;

}
*/

// experimental methods - (or)