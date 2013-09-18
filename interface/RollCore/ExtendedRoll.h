/** Author: Mircho Rodozov mrodozov@cern.ch
 * Date 08.01.2013
 * Info added at SVN revision 76
 * 
*/
#ifndef EXTENDED_ROLL_H
#define EXTENDED_ROLL_H

#include <string>
#include "DataObject.h"
#include "Roll.h"
#include "Chip.h"

class ExRoll : public Roll {
  
  map<int,ChipBaseClass*> Chips; // used by getChipByNumber method to point to chips member, where the virtual getChipByNumber specifies the actual returned type
  int NumberOfChipsAssigned; // hold how many chips are assigned 
  
  double rollAvgRate;
  double stripsAreaOfClone;
  double * stripArea;
  string towerName;
  string LBstring;
  
  // fields added by Michele Gabusi
  
  string rootFile;
  string TH1Path;
  TFile* refFile;
  
protected:
  
  void incrementNumberOfChips(); // Chip class method, increments the NumberOfChipsAssigned member with one
  void insertNewChipToChipsMap(const int & num,ChipBaseClass * abstractObject); // no need to override this one
  map<int,ChipBaseClass*> getChipsMap(){return this->Chips;}
  
public:
  
  // Methods  Necessary to implement
  
  virtual void allocStrips(); // override
  virtual void initStrips(); // use Roll::initStrips and add whats needed 
  virtual ExStrip * getStrip(int stripNum); // override 
  virtual ExStrip * getStripCollection(); // override
  
  // end of methods to override as minimum
  
  // good to override
  
  ExRoll(const string & RollOnlineName); // Constructors, usually is using its base , no need to override
  ExRoll(); // Base constructor
  ~ExRoll(); //
  
  virtual void initWithValues(const std::string & W_D,const int & W_D_N,const int & sec,const std::string & roll_id,const std::string & roll_id_num);
  virtual void initWithValues(const string & RollNameString);
  
  // other methods
  
  // Chip Class methods. Override what to be returned if the Chip class is extended in extension of ExRoll class (interface)
  
  virtual Chip * getChipByNumber(const int & ChipNumber); // specifies the concrete return type of the Chip objects to be returned. override this
  virtual void insertNewChipWithNumberAndID(const int & num,const int & ID); // insert chip 
  void assignChipsFromSource(map<string,map<int,int> > ChipsMap); 
  const int & getNumberOfChipsAssigned();
  
  // end of Chip methods
  
  // custom ExRoll methods and overridden Roll methods (Like Reset() and ClearStripsConditions() )
  
  void setRollRate(const double & rollRate);
  const double & getRollRate();
  double getSumOfStripRates();
  int getStripsToBeMaskedFromClone(const int & CloneNum);
  int getStripsToBeUnmaskedFromClone(const int & CloneNum);
  double getRollRateFromClone(const int & CloneNum);
  double getRollRatePSCFromClone(const int & CloneNum);
  const string & getLBstring();
  void setLBstring(const string & theLBString);
  void setStripsToBeMaskedFromSource(const DataObject & MaskedStrips);
  void setStripsToBeUnmaskedFromSource(const DataObject & StripToUnmask);
  void setStripsToBeUnmasked_withMaxRate_FromSource(const DataObject & StripsToUnmask_WithMaxRate);
  void setStripsToBeMasked_withTheTime_andMaxRate_FromSource(const DataObject & StripsMask_withTheTime_andMaxRate);
  void setDeadStripsWithMaxRate(const DataObject & DeadStripsWithMaxRate);
  void setStripsRateFromSource(const DataObject & StripsRates);
  const double & getStripsAreaFromClone(const int & CloneNum); // returns the area of one strip for specified Roll within a Chamber (or for one 'partition')
  void setStripsAreaForClone(const double & StripArea,const int & CloneNum); // set the strips for given clone (partition)
  void setStripsRateFromRootFileSource(const std::string & rootFileName); // would not be implemented, not used anywhere // used, it causes problems in Apllications.cpp
  void setStripsRateFromRootFileSource(const TFile & RatesObject); // obsolete and deprecated, this method search for single object in the entire file, then it cloeses the file. In the better implementation, the TH1F object is passed directly by reference as argument 
  void setTowerNameFromMap(map<string,string> towerMap); //
  void setLBnameFromMap(map<string,string> RollName_LBname_Map);
  
  // virtual ExRoll * cloneObject() const { return new ExRoll(*this); } // return copy of the existing object, doesnt work this way !
  
  /** @brief  if single strips rate values are greater then the mean of the distribution of all strips with given percent this strips rate is set to 0  */
  void removeNoisyStripsForCloneWithPercentValue(const int & cloneNum,const int & percents); 
  /** @brief wrapper for removing the noisy strips for all clones */
  void removeNoisyStripsForAllClonesWithPercentValue(const int & percent);
  /** @brief return the channel number (or the online number) of the first strip object in this clone*/
  int getFirstStripNumberOfClone(const int & cloneNum);
  /** @brief return the channel number (or the online number) of the last strip object in this clone*/
  int getLastStripNumberOfClone(const int & clone);
  string getPreviousNeighbourName();
  string getNextNeighbourName();  
  
  /*
  double getAverageRatePSCforAllClonesUsingRefitedHistos(); // obsolete, deprecated , not used in Applications
  double getAverageRatePSCusingRefitedHistoforClone(const int &); // obsolete , deprecated , not used in Applications
  double getFitValueFromCuttedRateHistoForClone(const int& cloneNum,const int & cutValue);// obsolete , deprecated, not used in Applications
  double getAvgRatePSCusingCutValueAndSingleFit(const int &);// obsolete , deprecated, not used in Applications
  */
  
  double getAvgRatePSCWithoutCorrections();
  double getAvgRatePSCWithoutCorrectionsForClone(const int & cloneNum );
  double getFullAreaForClone(const int & cloneNum); // the surface of this partition (Clone, Roll, - all the same)
  double getChamberFullArea();
  int getTotalChannelsOfOneClone(); /** does not require arguments . return the total number of strips in single clone */  
  string getShortFullIDofClone(const int & cloneNum); // needed for 2D square-like 
  void setTowerName(const string & tower_Name);
  const string & getTowerName(); // get the name of the trigger logic tower to which this chamber belong to  
  int getStripsWithZeroRate();
  double getMaxRateValue();
  void setStripsRatesFromTH1FObject(TH1F * HistoObject);
  void setStripsAreaFromSource_cmsswResource(const DataObject &);
  void setRollRawIDs_UsingNewIDsFile(const DataObject &);
  string getOpositeRollIDEcapOnly();
  // TH1F * getRateHistoForClone(const int &); possibly overridden by Michele
  TH1F * getRefitedRateHistoForClone(const int &);
  TH1F * getCuttedAndFitedRateHistoForClone(const int &,const int &);
  void WriteResultForDB(std::ofstream &);
  void WriteDetailedResultsForDB(std::ofstream &,std::ofstream&);
  void WriteGeomTable(std::ofstream &);
  void WriteShortDetailedResultsForDB(std::ofstream &);
  
  //void WriteExtendedGeomTable (std::ofstream &,int & ); // not used , deprecated and obsolete, would not be used
  // reset states methods
  virtual void ClearStripsConditions(); // reset the strip fields, set them to rate zero and reset condition states (i.e. no masked dead to mask or to unmask)
  virtual void Reset(); // Reset the state of the object, clear everything  override to extend default behavior. 
  
  // experimental methods, may try to implement them
  
  double getRateCallBackMethod(int,double (*getRateMethod)(int)); // not implemented but try to use callbacks
  
  /** Methods added by Michele Gabusi */
  /** Author: Michele Gabusi  mgabusi@cern.ch
  * Date 08.01.2013
  * Info added at SVN revision 76
  * 
  */
  
  string getRootFileName() {return rootFile;};
  string getTH1Path() {return TH1Path;};
  TH1F* getRateHistoForClone(const int &);
  TH1F* getRateHistoForClone(TH1F* totHisto, const int & cloneNum);
  void setRootFileName(string name) {rootFile = name;};
  void setTH1Path(string name) {TH1Path = name;};
  void setStripsAreaFromMap(map<string, double>& areaMap);
  void setStripsRateFromTH1Source(TH1F* h1);
  double getRollArea();
  void AddToRate(int rate);
  TH1F* buildHistoFromRoll();
  int getEmptyStrip();
  double CalculateRateFromTH1(bool correct, int clone=-9); // to be removed somehow or redeclared - it requires that ExtendedRoll depends on NoiseAnalyzer
  
};

#endif