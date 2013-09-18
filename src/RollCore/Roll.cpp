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
#include "../../interface/RollCore/DataObject.h"
#include "../../interface/RollCore/Strip.h"
#include "../../interface/RollCore/Roll.h"

using namespace std;

RollInterface::~RollInterface(){
  
  //cout << "RollInterface destructor" << endl;
  
}

RollInterface::RollInterface(){
  
  this->strips = NULL; // prevent undefined behaviour if strips are not allocated
//   cout << "RI constructor" << endl;
  
}

void Roll::allocStrips(){
  
  this->strips = new Strip[96];
  
}

void Roll::initStrips(){
  
  for(int i=1; i <= 96 ; i++){
    this->getStrip(i)->initWithValues(this->getWheel_Disk(),this->getWheel_Disk_Number(),this->getSector(),this->getRollID(),this->getOptionalRollID(),i);
  }
  this->setDisconectedStrips();
  this->Clone();
  
}


Strip * Roll::getStrip(int element){
  
  return & getStripCollection()[element-1]; // as the 
  
}

Strip * Roll::getStripCollection(){
  
  return dynamic_cast<Strip*>(strips);
  
}

Roll::Roll(const string& W_D_, const int& W_D_N_, const int& sec_, const string& roll_id_, const string& roll_id_num_){
  
  this->initWithValues(W_D_,W_D_N_,sec_,roll_id_,roll_id_num_);
  this->setNumberOfClones(0);
  this->setNumberOfUnpluggedStrips(0);
  //clone
}

Roll::Roll(const string & RollOnlineName){
  
  this->ParseStringToIdentifiers(RollOnlineName,this->WheelOrDisk,this->WheelOrDiskNumber,this->Sector,this->ID,this->ID_N);
  //cout <<  "Roll string constructor" << endl;
  this->setNumberOfClones(0);
  this->setNumberOfUnpluggedStrips(0);
  //set disconnected
  // clone
  
}


Roll::Roll(){
  
  //cout << "from Roll def constructor" << endl;
  this->initWithValues("",0,0,"","");
  this->setNumberOfClones(0);
  this->setNumberOfUnpluggedStrips(0);
  //cout << "Roll constructor" << endl;
  
}

Roll::~Roll(){
  
  //cout << "Roll destructor" << endl;  
  delete [] this->getStripCollection();
  delete [] this->RollIDofClone;
  delete [] this->RawIDofClone;
  delete [] this->RollPartOfClone;
  this->strips = NULL;
  
}

void Roll::initClonesData(const int& numberOfClones){
  
  RollIDofClone = new string[numberOfClones];
  RawIDofClone = new int[numberOfClones];
  RollPartOfClone = new string[numberOfClones];
  
  for (int i=0;i < numberOfClonedRolls ;i++){
    RollIDofClone[i] = "";
    RawIDofClone[i] = 0;
    RollPartOfClone[i] = "";
  }
  
}

void Roll::initWithValues(const string& W_D, const int& W_D_N, const int& sec, const string& roll_id, const string& roll_id_num){
  
  WheelOrDisk= W_D ;
  WheelOrDiskNumber = W_D_N ; 
  Sector=sec;
  ID=roll_id;
  ID_N=roll_id_num;
  numberOfClonedRolls=0;
  
}

void Roll::initWithValues(const string& RollNameString){
  
  ParseStringToIdentifiers(RollNameString,this->WheelOrDisk,this->WheelOrDiskNumber,this->Sector,this->ID,this->ID_N);
  numberOfClonedRolls=0;
  
}


void Roll::ParseStringToIdentifiers(const string& fullRollOnlineString, string& WheelOrD, int& WheelOrDiskN, int& Sect, string& ID_, string& IDN_){
  int Sector,WRN; //(WRN means Wheel or Ring Number )
  string firstPart,secondPart,thirdPart,WheelRing,ID,Opt_ID,LINE;
  LINE = fullRollOnlineString;
  stringstream ss;
   firstPart = LINE.substr(0,(LINE.find("_")));
    //cout << firstPart << endl;
    secondPart = LINE.substr(firstPart.size()+1, LINE.find_last_of("_") - LINE.find_first_of("_") -1 );
    //cout << secondPart << endl;
    thirdPart = LINE.substr(LINE.find_last_of("_")+1,LINE.size()-LINE.find_last_of("_"));
    //cout << firstPart << " " << secondPart << " " << thirdPart << endl;
    // now find the + and -
    
    if(firstPart.substr(0,1)=="W"){
      
      WheelRing = "W";
      
      if ( thirdPart.find("+") != string::npos || thirdPart.find("-") != string::npos){
	if (thirdPart.size() == 3){
	  ss << thirdPart.substr(0,1);
	  ss >> Sector;
	  //cout << thirdPart.substr(1,2) << endl;
	  Opt_ID = thirdPart.substr(1,2);
	  //cout <<thirdPart<<" "<< Sector <<" "<<Opt_ID <<" "<< endl;
	  ss.clear();
	}
	else{
	  //cout << thirdPart.substr(2,2) << endl;
	  ss << thirdPart.substr(0,2);
	  ss >> Sector;
	  //cout << thirdPart.substr(1,2) << endl;
	  Opt_ID = thirdPart.substr(2,2);
	  //cout << thirdPart <<" "<< Sector <<" "<<Opt_ID <<" "<< endl;
	  ss.clear();
	}
      }
      else{
	if (thirdPart.size() == 2){
	  ss << thirdPart.substr(0,1);
	  ss >> Sector;
	  //cout << thirdPart.substr(1,2) << endl;
	  Opt_ID = thirdPart.substr(1,1);
	  //cout <<thirdPart<<" "<< Sector <<" "<<Opt_ID <<" "<< endl;
	  ss.clear();
	}
	else{
	  //cout << thirdPart.substr(2,2) << endl;
	  ss << thirdPart.substr(0,2);
	  ss >> Sector;
	  //cout << thirdPart.substr(1,2) << endl;
	  Opt_ID = thirdPart.substr(2,1);
	  //cout << thirdPart <<" "<< Sector <<" "<<Opt_ID <<" "<< endl;
	  ss.clear();
	}
      }
      // set the other properties
      ss << firstPart.substr(1,firstPart.size()-1);
      ss >> WRN;
      ss.clear();
      ID = secondPart;
      // now test it
      //cout << LINE <<" "<<WheelRing <<" "<<WRN<<" "<<ID<<" "<<Sector<<" "<<Opt_ID<<endl;
    }
    else{// its a endcap roll 
      WheelRing = "RE";
      ss << firstPart.substr(2,2);
      ss >> WRN;
      ss.clear();
      ss << secondPart;
      ss >> Sector;
      ss.clear();
      //cout << WRN <<" "<< secondPart << endl;
      ID = thirdPart;
      Opt_ID= "";
      //cout << LINE <<" "<<WheelRing <<" "<<WRN<<" "<<ID<<" "<<Sector<<" "<<Opt_ID<<endl;

    }
    //cout << LINE <<" "<<WheelRing <<" "<<WRN<<" "<<ID<<" "<<Sector<<" "<<Opt_ID<<endl; // debug (angry) :))))
    WheelOrD = WheelRing ; WheelOrDiskN = WRN ;Sect = Sector ; ID_ = ID ; IDN_ = Opt_ID;
}

void Roll::setDisconectedStrips(){
   int * arrayWith_Unpluged ;
  this->setNumberOfUnpluggedStrips(0);
  
  if(ID == "RB1in" || ID == "RB2in"){
    setNumberOfUnpluggedStrips(6);
    int arrayWithUnpluged[] = {0,16,32,48,64,80} ;
    arrayWith_Unpluged = arrayWithUnpluged;
  }
  if (ID == "RB1out"){
    setNumberOfUnpluggedStrips(12);
    int arrayWithUnpluged[] = {0,8,16,24,32,40,48,56,80,88,64,72};
    arrayWith_Unpluged = arrayWithUnpluged;
  }
  
  if (ID == "RB2out"){
    setNumberOfUnpluggedStrips(12);
    int arrayWithUnpluged[] = {0,1,16,17,32,33,48,49,64,65,80,81};
    arrayWith_Unpluged = arrayWithUnpluged;
  }
  if(ID == "RB3"){
    setNumberOfUnpluggedStrips(12);
    int arrayWithUnpluged[] = {0,15,16,31,32,47,48,63,64,79,80,95};
    arrayWith_Unpluged = arrayWithUnpluged;
  }
  if( ID == "RB4")
    
  {
    if (  ( (WheelOrDiskNumber == 1 || WheelOrDiskNumber == 2) && Sector == 8) || ((WheelOrDiskNumber == 0 || WheelOrDiskNumber == -1 || WheelOrDiskNumber == -2) && Sector == 12 ) ) 
    {
      setNumberOfUnpluggedStrips(12);
      int arrayWithUnpluged[] = {48,49,62,63,64,65,78,79,80,81,94,95};
      arrayWith_Unpluged = arrayWithUnpluged;
    }
    
    else if (  ( (WheelOrDiskNumber == 1 || WheelOrDiskNumber == 2) && Sector == 12) || ((WheelOrDiskNumber == 0 || WheelOrDiskNumber == -1 || WheelOrDiskNumber == -2) && Sector == 8 ) ) 
    {
      setNumberOfUnpluggedStrips(12);
      int arrayWithUnpluged[] = {0,1,14,15,16,17,30,31,32,33,46,47};
      arrayWith_Unpluged = arrayWithUnpluged;
    }
    
    else if (Sector == 9 || Sector == 11 )
    {
      setNumberOfUnpluggedStrips(48);
      int arrayWithUnpluged[]={48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
      arrayWith_Unpluged = arrayWithUnpluged;      
    }
    
    else if (Sector == 10)
    {
      setNumberOfUnpluggedStrips(36);
      int arrayWithUnpluged[]={0,1,14,15,16,17,30,31,32,33,46,47,48,49,62,63,64,65,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
      arrayWith_Unpluged=arrayWithUnpluged;
    }
    else if (Sector == 4)
    {
      setNumberOfUnpluggedStrips(24);
      int arrayWithUnpluged[]={0,1,14,15,16,17,30,31,32,33,46,47,48,49,62,63,64,65,78,79,80,81,94,95};
      arrayWith_Unpluged=arrayWithUnpluged;
    }
    else
    {
      setNumberOfUnpluggedStrips(0);
    }
  }
  //else{
  //  setNumberOfUnpluggedStrips=0;
  //}
  /*else{
    setNumberOfUnpluggedStrips=0;
    int arrayOfUnpluged[0];
    arrayWith_Unpluged=arrayOfUnpluged;
  } */
  if(getUnpluged() != 0)
  {
  for(int i=0;i < getUnpluged();i++)
  {
      int j = arrayWith_Unpluged[i];
      this->getStrip(j+1)->Disconnect();
  }
    
  }
}



void Roll::Clone(){
  if(WheelOrDisk == "W"){
      if(ID == "RB1in" || ID =="RB1out" || ID =="RB2in" || ID =="RB2out"){
	numberOfClonedRolls=1;
	this->initClonesData(numberOfClonedRolls);

	if(ID_N == "F"){
	  RollIDofClone[0]="Forward";
	}
	else if(ID_N == "B"){
	  RollIDofClone[0]="Backward";
	}
	else{
	  RollIDofClone[0]="Middle";
	}
	RollPartOfClone[0] = "";
	int counter=1;
	for(int i=0 ; i < 96 ;i++){ /** rewrite it in order to meet the model :) */
	  
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	
      }
      
      if (ID == "RB3"){  
	numberOfClonedRolls = 2;
	this->initClonesData(numberOfClonedRolls);
	// set the if conditions here
	if(getWheel_Disk_Number() > 0){
	RollPartOfClone[0] = "-";
	RollPartOfClone[1] = "+";
	}
	if(getWheel_Disk_Number() < 0){
	RollPartOfClone[0] = "+";
	RollPartOfClone[1] = "-";
	}
	if(getWheel_Disk_Number() == 0){
	if(getSector() == 1 || getSector() == 4 || getSector() == 5 ||getSector() == 8 ||getSector() == 9 || getSector() == 12 ){
	  RollPartOfClone[0] = "+";
	  RollPartOfClone[1] = "-";
	}
	else{
	  RollPartOfClone[0] = "-";
	  RollPartOfClone[1] = "+";
	}
	}
	
	if(ID_N == "F"){
	  RollIDofClone[0] = "Forward";
	  RollIDofClone[1] = "Forward";
	}
	else{
	  RollIDofClone[0] = "Backward";
	  RollIDofClone[1] = "Backward";
	}
	int counter = 1;
	for(int i=0;i < 48;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	counter = 1;
	for(int i=48 ; i < 96 ;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[1]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
      }

      if (ID == "RB4"){
	
	if(Sector != 4 && Sector != 9 && Sector != 10 && Sector != 11 ){
	  numberOfClonedRolls = 2;
	  this->initClonesData(numberOfClonedRolls);
	  if(getWheel_Disk_Number() > 0){
	    RollPartOfClone[0] = "-";
	    RollPartOfClone[1] = "+";
	  }
	  if(getWheel_Disk_Number() < 0){
	    RollPartOfClone[0] = "+";
	    RollPartOfClone[1] = "-";
	  }
	  if(getWheel_Disk_Number() == 0){
	    if(getSector() == 1 || getSector() == 5 || getSector() == 8 || getSector() == 12 ){
	      RollPartOfClone[0] = "+";
	      RollPartOfClone[1] = "-";
	    }
	    else{
	      RollPartOfClone[0] = "-";
	      RollPartOfClone[1] = "+";
	    }
	  }
	  
	  if (ID_N == "+F" || ID_N == "-F"){
	    RollIDofClone[0] = "Forward";
	    RollIDofClone[1] = "Forward";
	  }
	  else{
	    RollIDofClone[0] = "Backward";
	    RollIDofClone[1] = "Backward";
	  }
	int counter = 1;
	for(int i=0;i < 48;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	counter = 1;
	for(int i=48 ; i < 96 ;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[1]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}

	  /**
	  for(int i=0 ; i < 48 ; i++){
	    getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	    getStrip(i+1)->setOfflineNumber(i+1);
	  }
	  for(int i=48 ; i < 96 ;i++){
	    getStrip(i+1)->setOfflineID(RollIDofClone[1]);
	    getStrip(i+1)->setOfflineNumber(i-47);
	  }
	  */
	}
	
	if (Sector == 9 || Sector == 10 || Sector == 11){
	  numberOfClonedRolls = 1;
	  this->initClonesData(numberOfClonedRolls);
 	 if (ID_N == "F" || ID_N == "+F" || ID_N == "-F"){
	    RollIDofClone[0] = "Forward";
	  }
	  else{
	    RollIDofClone[0] = "Backward";
	  }

	  if (Sector == 9 || Sector == 11){
	    RollPartOfClone[0] = "";
	  }
	  else{
	    if (ID_N == "-F" || ID_N == "-B"){
	      RollPartOfClone[0] = "-";
	    }
	    else{
	      RollPartOfClone[0] = "+";
	    }
	  }
	  int counter = 1;
	  for(int i=0 ; i < 96 ;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	  
	 // for(int i=0 ; i < 96 ; i++){
	 //   getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	 //   getStrip(i+1)->setOfflineNumber(i+1);
	 // }
	}
	
	if (Sector == 4){
	  numberOfClonedRolls = 2;
	  this->initClonesData(numberOfClonedRolls);
	  
	  if(ID_N == "-F" || ID_N == "+F"){
	    RollIDofClone[0] = "Forward";
	    RollIDofClone[1] = "Forward";
	  }
	  else{
	    RollIDofClone[0] = "Backward";
	    RollIDofClone[1] = "Backward";
	  }
	  /** just a comment here , -- is like it is , - only means -+ , ++ is like it is + only means +- 
	   * keep that in mind , if you change something (for example this ID's to -- - + ++)
	  */
	  if(ID_N == "-F" || ID_N == "-B"){
	    if (getWheel_Disk_Number() >= 0){
	      RollPartOfClone[0] = "--";
	      RollPartOfClone[1] = "-+";
	    }
	    else{
	    RollPartOfClone[0] = "-+";
	    RollPartOfClone[1] = "--";
	    }
	  }
	  else{
	    if(getWheel_Disk_Number() >= 0){
	    RollPartOfClone[0] = "+-"; // or ONLY + , according to the new ID's and Anton
	    RollPartOfClone[1] = "++";
	    }
	    else{
	    RollPartOfClone[0] = "++";
	    RollPartOfClone[1] = "+-";
	    }
	  }
	  int counter = 1;
	for(int i=0;i < 48;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[0]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	counter = 1;
	for(int i=48 ; i < 96 ;i++){
	  getStrip(i+1)->setOfflineID(RollIDofClone[1]);
	  if (getStrip(i+1)->getIsDisconnected() != 1){
	    getStrip(i+1)->setOfflineNumber(counter);
	    counter ++;
	  }
	}
	}
      }
    }
    
    
    // implementation of the endcap 
    if(WheelOrDisk == "RE"){
      numberOfClonedRolls = 3;
      this->initClonesData(numberOfClonedRolls);
      RollIDofClone[0] = "A";
      RollIDofClone[1] = "B";
      RollIDofClone[2] = "C";
      if (getWheel_Disk_Number() == -1 || getWheel_Disk_Number() == 1)
      {/**this algorithm is about to be very lame,but it should be implemented fast so ... no other options :)
      *it is obvious that it should use mathematics rule in order to shortening the code ... ! BLA ;(
      */
      int Offline = 16;
      for (int i = 0 ; i < 16 ; i++){
	  getStrip(i+1)->setOfflineID("A");
	  getStrip(i+1)->setOfflineNumber(Offline);
	  Offline--;
      }
      Offline = 32;
      for(int i = 16;i < 32;i++){
	  getStrip(i+1)->setOfflineID("A");
	  getStrip(i+1)->setOfflineNumber(Offline);
	  Offline--;
      }
      for (int i = 32 ; i < 64 ; i++){
	
	getStrip(i+1)->setOfflineID("B");
	getStrip(i+1)->setOfflineNumber(i-31);

      }
      for (int i = 64 ; i < 96 ; i++){
	
	getStrip(i+1)->setOfflineID("C");
	getStrip(i+1)->setOfflineNumber(i-63);

      }
      
      }
      else {
	int Offline = 16;
	for (int i = 0 ; i < 16 ; i++){
	  getStrip(i+1)->setOfflineID("A");
	  getStrip(i+33)->setOfflineID("B");
	  getStrip(i+1)->setOfflineNumber(Offline);
	  getStrip(i+33)->setOfflineNumber(Offline);
	  Offline--;
	}
	Offline = 32;
	for(int i = 16;i < 32;i++){
	  getStrip(i+1)->setOfflineID("A");
	  getStrip(i+33)->setOfflineID("B");
	  getStrip(i+1)->setOfflineNumber(Offline);
	  getStrip(i+33)->setOfflineNumber(Offline);
	  Offline--;
	}
	for (int i = 64 ; i < 96 ; i++){
	getStrip(i+1)->setOfflineID("C");
	getStrip(i+1)->setOfflineNumber(i-63);
	}
      }
    }
    
}

// end of important methods


const int & Roll::getWheel_Disk_Number(){
  return WheelOrDiskNumber;
}

void Roll::setWheelDiskNumber(const int& number){
  this->WheelOrDiskNumber = number;
}

const int & Roll::getSector(){
  
  return this->Sector;
  
}

void Roll::setSector(const int& sector){
  
  this->Sector = sector;
  
}

const int & Roll::getRawIDofClone (const int & clone){
  
  return RawIDofClone[clone-1];
  
}

string Roll::returnFieldAsString(const int & classField){
  string mediator;
  stringstream ss;
  ss << classField;
  ss >> mediator;
  ss.clear();
  return mediator;
}

const string& Roll::getWheel_Disk(){
  return this->WheelOrDisk;
}

void Roll::setWheelDisk(const string& WheelDisk_){
  this->WheelOrDisk = WheelDisk_;
}

const string& Roll::getRollID(){
  return this->ID;
}

void Roll::setRoll_ID_(const string& ID_){
  this->ID = ID_;
}

const string& Roll::getOptionalRollID(){
  return this->ID_N;
}

void Roll::setRollOptionalID(const string& OptionalID){
  this->ID_N = OptionalID;
}


int Roll::getMasked(){
  int  masked = 0;
  for (int i = 0; i < 96 ; i++){
    if(getStrip(i+1)->getIsMasked()==1){
      masked++;
    }
  }
  return masked;
}

int Roll::getDead(){
  int  dead = 0;
  
  for (int i = 0; i < 96 ; i++){
    if(getStrip(i+1)->getIsDead()==1){
      dead++;
    }
  }
  return dead;
  
}

int Roll::getUnpluged(){
   int  disconnected = 0;
  
  for (int i = 0; i < 96 ; i++){
    if(getStrip(i+1)->getIsDisconnected()==1){
      disconnected++;
    }
  }
  return disconnected;
  
}

void Roll::setNumberOfUnpluggedStrips(const int& unplugged){
  
  this->numberOfUnpluged = unplugged;
  
}

int Roll::getMaskedFromClone(const int & cloneNumber){
  
  int tmp = 96/getClones();
  int maskedInClone = 0;
  for (int i = (cloneNumber-1)*tmp ; i < (cloneNumber)*tmp ; i++ ) { 
    if(getStrip(i+1)->getIsMasked() == 1) {
      maskedInClone ++;
    }
  }
  return maskedInClone;
 
}

int Roll::getDeadFromClone(const int & cloneNumber){
  int tmp = 96/getClones();
  int deadInClone = 0;
  for (int i = (cloneNumber-1)*tmp ; i < (cloneNumber)*tmp ; i++ ) { 
    if(getStrip(i+1)->getIsDead() == 1) {
      deadInClone ++;
    }
  }
  return deadInClone;
  
}

int Roll::getUnplugedFromClone(const int & cloneNumber){
  int tmp = 96/getClones();
  int disconnected = 0;
  for (int i = (cloneNumber-1)*tmp ; i < (cloneNumber)*tmp ; i++ ) { 
    if(getStrip(i+1)->getIsDisconnected() == 1) {
      disconnected ++;
    }
  }
  return disconnected;
}

void Roll:: printOutStatus(){
  cout << WheelOrDisk <<" "<<WheelOrDiskNumber<<" "<<Sector<<" "<<ID;
  if (WheelOrDisk == "W"){cout <<" "<<ID_N;}
  cout <<" "<<getMasked()<<" "<<getDead()<<" "<<getUnpluged()<<endl;
}

void Roll::printStatusOfClones(){
  for (int i = 0 ; i < numberOfClonedRolls ; i++){
    cout << WheelOrDisk <<" "<<WheelOrDiskNumber<<" "<<Sector<<" "<<ID;
    cout <<" "<<RollIDofClone[i];
    cout <<" "<<getMaskedFromClone(i+1)<<" "<<getDeadFromClone(i+1)<<" "<<getUnplugedFromClone(i+1)<<endl;
    
  }
}

int * Roll::getMaskedByNumbers(){
  int counterOfMasked = 0;
  int * arrayOfMaskedStrips;
  arrayOfMaskedStrips = new int[getMasked()];
  for (int i=0 ; i < 96 ; i++){
    if(getStrip(i+1)->getIsMasked() > 0){
      arrayOfMaskedStrips[counterOfMasked] = i+1;
      counterOfMasked++;
    }
  }
  return arrayOfMaskedStrips;
}

int * Roll::getDeadByNumbers(){
  int counterOfDead = 0;
  int * arrayOfDeadStrips;
  arrayOfDeadStrips = new int[getDead()];
  for (int i=0 ; i < 96 ; i++){
    if(getStrip(i+1)->getIsDead() > 0){
      arrayOfDeadStrips[counterOfDead] = i+1;
      counterOfDead++;
    }
  }
  return arrayOfDeadStrips;
  
}

// this two doesnt work for now

int * Roll::getMaskedByNumbersFromClone(const int & cloneNumber){
  
  
  int * arrayOfMaskedStrips ,counterOfMasked = 0;
  arrayOfMaskedStrips = new int [getMaskedFromClone(cloneNumber)];

  for (int i = (cloneNumber-1)*(96/getClones()) ; i < cloneNumber*(96/getClones()) ; i++){
    if(getStrip(i+1)->getIsMasked() > 0){
      arrayOfMaskedStrips[counterOfMasked] = getStrip(i+1)->getOnlineNumber();
      counterOfMasked++;
    }
  }
  return arrayOfMaskedStrips;
}

int * Roll::getDeadByNumbersFromClone(const int & cloneNumber){
  int * arrayOfDeadStrips ,counterOfDead = 0;
  arrayOfDeadStrips = new int [getDeadFromClone(cloneNumber)];
  
  for (int i = (cloneNumber-1)*(96/getClones()) ; i < cloneNumber*(96/getClones()) ; i++){
    if(getStrip(i+1)->getIsMasked() > 0){
      arrayOfDeadStrips[counterOfDead] = getStrip(i+1)->getOnlineNumber();
      counterOfDead++;
    }
  }
  return arrayOfDeadStrips;
}

void Roll::setNumberOfClones(const int& number){
  this->numberOfClonedRolls = number;
}

const int& Roll::getClones(){
  return this->numberOfClonedRolls;
}

const int& Roll::getNumberOfClones(){
  
  return this->numberOfClonedRolls;
  
}

void Roll::setRawIDofClone(const int & cloneNum,const int & newRawID){
    RawIDofClone[cloneNum] = newRawID;
}

bool Roll::isInBarrel(){
  if(this->getWheel_Disk()=="W"){
    return true;
  }
  else{
    return false;
  }
}

bool Roll::isInEndcap(){
  if(this->getWheel_Disk()=="RE"){
    return true;
  }
  else{
    return false;
  }
}

const string& Roll::getRollPartOfClone(const int& cloneNum){return this->RollPartOfClone[cloneNum-1]; }

const string& Roll::getRollOfflineIDofClone(const int& cloneNum){return this->RollIDofClone[cloneNum-1];}

// the heavy implementation methods comes here

string Roll::getRollIDofClone(const int& cloneNum){
  
  string offlineRollID,whiteSpace,aditionalZero,WheelOrRing;
  if ( WheelOrDiskNumber > -1){
    whiteSpace="+";
  }
  else{
    whiteSpace ="";
  }
  if( Sector < 10 && WheelOrDisk == "W"){
    aditionalZero="0";
  }
  else{
    aditionalZero="";
  }
  // end of aditional signs
  if(WheelOrDisk == "W"){
    WheelOrRing = "_S";
  }
  else{
    WheelOrRing="_R";
  }
  
  if (WheelOrDisk == "W"){
      offlineRollID = WheelOrDisk+whiteSpace+returnFieldAsString(WheelOrDiskNumber)+"_"+ID+RollPartOfClone[cloneNum-1]+WheelOrRing+aditionalZero+returnFieldAsString(Sector)+"_"+RollIDofClone[cloneNum-1];
    
  }
  else{
     offlineRollID = WheelOrDisk+whiteSpace+returnFieldAsString(WheelOrDiskNumber)+WheelOrRing+returnFieldAsString(Sector)+"_CH"+ID+"_"+RollIDofClone[cloneNum-1];
  }
  return offlineRollID;
  
}



string Roll::getRollIDofClone_withEtaPartSeparated(const int& cloneNum){
  
  string changed_Part_of_Clone="",changed_ID,aditionalPlus="",zeroLessEndcapID;
  if(getWheel_Disk_Number() > 0){
      aditionalPlus = "+";
    }
  if (isInBarrel()){
    if(getSector() == 4 && getRollID() == "RB4" && getRollPartOfClone(cloneNum).substr(0,1) != getRollPartOfClone(cloneNum).substr(1,1)){
      changed_Part_of_Clone = getRollPartOfClone(cloneNum).substr(0,1);
    }
    else{
      changed_Part_of_Clone=getRollPartOfClone(cloneNum);
    }
    if(getOptionalRollID() == "C"){
      changed_ID = "Central";
    }
    else{
      changed_ID = RollIDofClone[cloneNum-1];
    }
    
    return getWheel_Disk()+aditionalPlus+returnFieldAsString(getWheel_Disk_Number())+"/"+getRollID()+"/"+returnFieldAsString(getSector())+changed_Part_of_Clone+" "+changed_ID;  
  }
  else{
    zeroLessEndcapID = getRollID();
    if(zeroLessEndcapID.substr(0,1)=="0"){
      zeroLessEndcapID=zeroLessEndcapID.substr(1,1);
    }
    return getWheel_Disk()+aditionalPlus+returnFieldAsString(getWheel_Disk_Number())+"/"+ returnFieldAsString(getSector())+"/"+zeroLessEndcapID+" "+getRollOfflineIDofClone(cloneNum);
  }
  
}

string Roll::getRollIDofCloneWithNewIdentifiers(const int& cloneNum){
  
  string newOfflineRollID;
  string offlineRollID,whiteSpace,aditionalZero,WheelOrRing;
  /** here they are , the two changed identifiers */
  string changed_Opt_ID,changed_ID/** i.e. Middle instead of Middle*/;
  if(getWheel_Disk() == "W"){
    if ( WheelOrDiskNumber > -1){
      whiteSpace="+";
    }
    else{
      whiteSpace ="";
    }
    
     WheelOrRing = "_S";
     
    if (getSector() < 10){
      aditionalZero = "0";
    }
    else{
      aditionalZero = "";
    }
    
    if(getOptionalRollID() == "C"){
      changed_ID = "Middle";
    }
    
    else{
      changed_ID = RollIDofClone[cloneNum-1];
    }
    
    if(getSector() == 4 && getRollID() == "RB4" && getRollPartOfClone(cloneNum).substr(0,1) != getRollPartOfClone(cloneNum).substr(1,1)){
      changed_Opt_ID = getRollPartOfClone(cloneNum).substr(0,1);
    }
    else{
      changed_Opt_ID=getRollPartOfClone(cloneNum);
    }
    
    newOfflineRollID = WheelOrDisk+whiteSpace+returnFieldAsString(WheelOrDiskNumber)+"_"+ID+changed_Opt_ID+WheelOrRing+aditionalZero+returnFieldAsString(getSector())+"_"+changed_ID;
    
  }
  
  else{
   newOfflineRollID =  getRollIDofClone(cloneNum);
  }
  
  return newOfflineRollID;
  
}


string Roll::getFullOnlineRollID(){
  
  string plus="";
  if (this->getWheel_Disk_Number() > 0){
    plus="+";
  }
  if(this->getWheel_Disk() == "W"){
    return this->getWheel_Disk()+plus+this->returnFieldAsString(this->getWheel_Disk_Number())+"_"+this->getRollID()+"_"+this->returnFieldAsString(this->getSector())+this->getOptionalRollID();
  }
  else{
    return this->getWheel_Disk()+plus+this->returnFieldAsString(this->getWheel_Disk_Number())+"_"+this->returnFieldAsString(this->getSector())+"_"+this->getRollID();
  }
  //return fullName;
  
}

string Roll::getRollIDofCloneWithConstructionDBidentifiers(const int& cloneNum){
  string newOfflineRollID;
  string offlineRollID,whiteSpace,aditionalZero,WheelOrRing;
  /** here they are , the two changed identifiers - according to ConstructionDB */
  string changed_Opt_ID,changed_ID/** i.e. Central instead of Middle*/;
  if(getWheel_Disk() == "W"){
    if ( WheelOrDiskNumber > 0){
      whiteSpace="+";
    }
    else{
      whiteSpace ="";
    }
    
    WheelOrRing = "_S";
    
    if (getSector() < 10){
      aditionalZero = "0";
    }
    else{
      aditionalZero = "";
    }
    
    if(getOptionalRollID() == "C"){
      changed_ID = "Central";
    }
    
    else{
      changed_ID = RollIDofClone[cloneNum-1];
    }
    
    if(getSector() == 4 && getRollID() == "RB4" && getRollPartOfClone(cloneNum).substr(0,1) != getRollPartOfClone(cloneNum).substr(1,1)){
      changed_Opt_ID = getRollPartOfClone(cloneNum).substr(0,1);
    }
    else{
      changed_Opt_ID=getRollPartOfClone(cloneNum);
    }
    
    newOfflineRollID = WheelOrDisk+whiteSpace+returnFieldAsString(WheelOrDiskNumber)+"_"+ID+changed_Opt_ID+WheelOrRing+aditionalZero+returnFieldAsString(getSector())+"_"+changed_ID;
    
  }
  
  else{
   newOfflineRollID =  getRollIDofClone(cloneNum);
  }
  
  return newOfflineRollID;
  
}

void Roll::setRollRawIDfromSource(const DataObject& source){
  
  for(int i = 1 ; i <= this->getClones() ; i++){
    //cout << this->getRollIDofCloneWithNewIdentifiers(i) << endl;
    
    this->setRawIDofClone(i,source.getElementByKeyAsInt(this->getRollIDofCloneWithNewIdentifiers(i),1));
    
  }
  
}

void Roll::setMaskedStripsFromSource(const DataObject& source){
  
  string someBuff = this->getFullOnlineRollID();
  
  if(source.ifKeyExist(someBuff)){
    
    /** 
     * This ClearConditions should be used only explicitely ! it the previos method is setDeadStripsFromSource
     * or similar it clears the conditions previously set ! 
     * 
     */
    //for(int i = 1; i <= 96 ; i++){
    //  this->getStrip(i)->ClearConditions();
    //}
    string curr_num;
    vector<string> thevec = source.getMapOfVectors().find(this->getFullOnlineRollID())->second;
    //cout << "the size is " << thevec.size()  << endl;
    for(int i = 0 ; i < thevec.size() ; i++){
      curr_num = thevec[i];
      this->getStrip(atoi(curr_num.c_str()))->Mask();
    }
  }
  
}

void Roll::setDeadStripsFromSource(const DataObject& source){
  
  if(source.ifKeyExist(this->getFullOnlineRollID())){
    string curr_num;
    vector<string> thevec = source.getMapOfVectors().find(this->getFullOnlineRollID())->second;
    //cout << "the size is " << thevec.size()  << endl;
    for(int i = 0 ; i < thevec.size() ; i++){	
      curr_num = thevec[i];
      this->getStrip(atoi(curr_num.c_str()))->Kill();
    }
  }

}




void Roll::WriteResults( std::ofstream & outFileStream){
  
  for (int i = 0 ; i < numberOfClonedRolls ; i++){

    if(getMaskedFromClone(i+1) !=0 || getDeadFromClone(i+1) != 0){
      outFileStream << getRollIDofCloneWithNewIdentifiers(i+1) << " " << getRawIDofClone(i+1);
      
      // tricky , but should work
      int couNT = 96/numberOfClonedRolls;// -> 32 , 48  // :-)
      
      outFileStream <<" "<< 96/getClones() - getUnplugedFromClone(i+1); // number of all connected strips
      outFileStream <<" "<< getMaskedFromClone(i+1);// change with getMaskedFromClone
      outFileStream <<" "<< getDeadFromClone(i+1);//change with getDeadFromClone
      //outFileStream <<" "<< getUnplugedFromClone(i+1);// change with getMaskedFromClone
      for (int j= i*couNT ; j < (i+1)*couNT ; j++ ){
	if(getStrip(j+1)->getIsMasked() > 0 ){
	  outFileStream <<" "<< getStrip(j+1)->getOfflineNumber(); 
	}
      }
      outFileStream << " -1" ; 
      //for (int k = 0 ; k < 96 - getMaskedFromClone(i+1) ; k++){
	//outFileStream<<" 0";
      //}      
      for (int j= i*couNT ; j < (i+1)*couNT ; j++ ){
	if(getStrip(j+1)->getIsDead() > 0){
	  outFileStream <<" "<< getStrip(j+1)->getOfflineNumber();
	  
	}
      }
      
      outFileStream << " -1";
      
      /*
      for (int k = 0 ; k < 96 - getDeadFromClone(i) ; k++){
	outFileStream<<" 0";
      }
      */
      outFileStream << "\n";
    }
    outFileStream.clear();
  }
}

void Roll::Reset(){
  // not implemented yet but have to reset all the fields
}

