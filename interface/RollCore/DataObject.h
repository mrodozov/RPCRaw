/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class DataObject{
  
  map<string,string*> KeysValuesMap;
  map<int,map<string,string*> > ElementsAsMatrixMap;
  map<string,vector<string> > SingleKeyMultiValuesMap;
  void transformElementsMapToSingleKeysMap();
  
  
 public:
  
  std::string getElementFromPropertyContainer(int Row,int Column) const;
  std::string  getElement(int Row,int Column) const;
  int  getElementAsInt (int Row,int Column) const;
  double  getElementAsDouble(int Row,int Column) const;
  string  getElementByKey(string Key,int Element) const;
  
  int getElementByKeyAsInt(string Key,int Element) const;
  
  double  getElementByKeyAsDouble(string Key,int Element) const;
  void getDataFromFile(std::string,int Columns);
  int getLenght() const;
  vector<string> getVectorOfValuesForKey(string Key);
  map<string,vector<string> > getMapOfVectors() const;
  //void transformElementsMapToSingleKeysMap();
  bool ifKeyExist(string keyToCheck) const;
  
  virtual void getDataFromDB();
  
  DataObject(string fileName,int columns); // this constructor will be used both because of history issues and because it could restrict the number of columns from file to be used
  DataObject(string fileName); // this constructor have to find out how many rows the file has
  DataObject();
  ~DataObject();
  
};

#endif