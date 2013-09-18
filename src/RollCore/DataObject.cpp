/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../../interface/RollCore/DataObject.h"
#include <cstdlib>
#include <stdlib.h>

using namespace std;

DataObject::DataObject(std::string fileName, int columns){
  this->getDataFromFile(fileName,columns);
}

DataObject::DataObject(std::string fileName){
  ifstream IFS;
  int columns=0;
  stringstream buff;
  string LINE,token;
  IFS.open(fileName.c_str());
  if (!IFS.good() || !IFS.is_open()) { cout << "File " << fileName << " not open, exit! " << endl ; exit(-1); }
  getline (IFS,LINE);
  buff.str(LINE);
  while(buff >> token){
    columns++;
  }
  IFS.clear();
  IFS.close();
  this->getDataFromFile(fileName,columns);
}

DataObject::DataObject(){
  
}

void DataObject::getDataFromFile(string fileName,int columns){
  
  
  ifstream IFS;
  string key,token;
  stringstream buffer;
  IFS.open(fileName.c_str());
  string LINE;
  int argumentCounter = 0;
  int matrixIndex = 1;
  
  while(getline(IFS,LINE)){
    string * aRow;
    buffer.str(LINE);
    if(columns == 1){
      aRow = new string;
    }
    else{
      aRow = new string[columns-1];
    }
    
    argumentCounter = 0;
    while(buffer >> token && argumentCounter < columns ){
      if(columns == 1){
	key = token;
	*aRow = token;
      }
      else{
	if(argumentCounter == 0){
	  key = token;
	}
	else {
	  aRow[argumentCounter-1] = token;
	}
	argumentCounter++;
      }
    }
    
    map<string,string*> aBuffer ;
    this->KeysValuesMap[key] = aRow;
    aBuffer[key] = aRow;
    this->ElementsAsMatrixMap[matrixIndex] = aBuffer;
    matrixIndex++;
    buffer.clear();
  }
  IFS.clear();
  IFS.close();
  if (columns > 1){
    this->transformElementsMapToSingleKeysMap();
  }
}


void DataObject::getDataFromDB(){
  
}


string DataObject::getElementFromPropertyContainer(int rowIndex,int columnIndex) const{
  string returnValue;
  
  if(columnIndex == 1){
    returnValue = this->ElementsAsMatrixMap.find(rowIndex)->second.begin()->first;
  }
  else{
    returnValue = this->ElementsAsMatrixMap.find(rowIndex)->second.begin()->second[columnIndex-2];
  }
  
  return returnValue;
  
}

string DataObject::getElementByKey(string Key, int Element) const{
  
  return this->KeysValuesMap.find(Key)->second[Element-1];
    
}

int DataObject::getElementByKeyAsInt(string Key, int Element) const{
  
  return atoi(this->KeysValuesMap.find(Key)->second[Element-1].c_str());
  
}

double DataObject::getElementByKeyAsDouble  (string Key, int Element) const {
  
  return atof(this->KeysValuesMap.find(Key)->second[Element-1].c_str());
  
  
}

string  DataObject::getElement (int Row,int Column) const{
  return this->getElementFromPropertyContainer(Row,Column);
}

int DataObject::getElementAsInt(int Row,int Column) const {
  return atoi(getElementFromPropertyContainer(Row,Column).c_str());
}

double DataObject::getElementAsDouble(int Row,int Column) const {
  return atof(getElementFromPropertyContainer(Row,Column).c_str());
}

int DataObject::getLenght() const{
  return this->ElementsAsMatrixMap.size();
}

vector< string > DataObject::getVectorOfValuesForKey(string Key){
  if(this->ifKeyExist(Key)){
    return this->SingleKeyMultiValuesMap.find(Key)->second;
  }
}


map< string, vector< string > > DataObject::getMapOfVectors() const {
  
  return this->SingleKeyMultiValuesMap;
  
}


bool DataObject::ifKeyExist(string keyToCheck) const {
  bool retval;
  if(this->KeysValuesMap.find(keyToCheck) != this->KeysValuesMap.end()){
    retval = true;
  }
  else {retval=false;}
  
  return retval;
}


void DataObject::transformElementsMapToSingleKeysMap(){
  
  for (int i = 1; i <= this->getLenght() ; i++){
    
    if (this->SingleKeyMultiValuesMap.find(this->getElement(i,1)) != this->SingleKeyMultiValuesMap.end()){
      this->SingleKeyMultiValuesMap.find(this->getElement(i,1))->second.push_back(this->getElement(i,2));
    }
    else{
      vector<string> first_element;
      SingleKeyMultiValuesMap[this->getElement(i,1)] = first_element;
      SingleKeyMultiValuesMap.find(this->getElement(i,1))->second.push_back(this->getElement(i,2));
    }
  }
  //cout << " executed !" << endl; // debug
}



DataObject::~DataObject(){
  this->KeysValuesMap.clear();
  this->ElementsAsMatrixMap.clear();
  this->SingleKeyMultiValuesMap.clear();
}

