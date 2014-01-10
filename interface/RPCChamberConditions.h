/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 18.12.13
*/

#ifndef RPC_CHAMBER_COND_H
#define RPC_CHAMBER_COND_H

#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;

/** Necessary methods to be implemented in the class realization 
  don't ask me why we need asbstract classes ADN/OR interfaces for this simple project. 
  This is right way if we are about to reuse functionality  */

class RPCChamberConditionsBase {
  
protected:
  
  string _chamberName;
  int _hvSet;
  int _hvMon;
  int _shelfNumber;
  bool _isReference;
  vector<string> gapsLabel;
  vector<double> gapsCurrent;
  vector<double> gapsVmon;
  vector<double> gapsVset;
  vector<int> _FEBtresholds;
  
public:
  
  virtual void setHVmon(const int & hvmonvalue) = 0;
  virtual const int & getHVmon() = 0;
  virtual void setHVset(const int & hvSetvalue) = 0;
  virtual const int & getHVset() = 0;
  virtual void setChamberName(const string & name) = 0 ;
  virtual const string & getChamberName() = 0 ;
  virtual void setShelfNumber (const int & shelfNumber) = 0 ;
  virtual const int & getShelfNumber () = 0 ;
  virtual void setGapLabels(const vector<string> & labels) = 0 ;
  virtual const vector<string> & getGapLabels() = 0;
  virtual void setGapsCurrent(const vector<double> & currents) = 0 ;
  virtual const vector<double> & getGapsCurrent() = 0;
  virtual void setGapsVmon(const vector<double> & gapsVmon) = 0 ;
  virtual const vector<double> & getGapsVmon() = 0 ;
  virtual void setGapsVset(const vector<double> & gapsVset) = 0 ;
  virtual const vector<double> & getGapsVset() = 0 ;
  virtual void setFEBTresholds(const vector<int> & FEBtresholds) = 0 ;
  virtual const vector<int> & getFEBTresholds() = 0 ;
  virtual void setIsReference(const bool & isReference)=0;
  virtual const bool & getIsReference()=0;
  
  RPCChamberConditionsBase();
  virtual ~RPCChamberConditionsBase();
  
};

class RPCChamberConditions : public RPCChamberConditionsBase { 
  
  // operating variables, HV, current, thresholds
  
protected:
  
public:
  
  void setHVmon(const int & hvmonvalue) ;
  const int & getHVmon() ;
  void setHVset(const int & hvmSetalue) ;
  const int & getHVset() ;
  void setChamberName(const string & name) ;
  const string & getChamberName() ;
  void setShelfNumber (const int & shelfNumber) ;
  const int & getShelfNumber () ;
  void setGapLabels(const vector<string> & labels);
  const vector<string> & getGapLabels();
  void setGapsCurrent(const vector<double> & currents);
  const vector<double> & getGapsCurrent();
  void setGapsVmon(const vector<double> & gapsVmon);
  const vector<double> & getGapsVmon() ;
  void setGapsVset(const vector<double> & gapsVset) ;
  const vector<double> & getGapsVset() ;
  void setFEBTresholds(const vector<int> & FEBtresholds) ;
  const vector<int> & getFEBTresholds() ;
  void setIsReference(const bool & isReference);
  const bool & getIsReference();
  /** additional methods */
  
  const double & getHVmonForGap(const int & gapNumber);
  const double & getHVsetForGap(const int & gapNumber);
  const double & getCurrentForGap(const int & gapNumber);
  const string & getLabelForGap(const int & gapNumber);
  const int & getFEBTresholdForBoard(const int & febNumber);
  
  RPCChamberConditions();
  ~RPCChamberConditions();
  
};


#endif