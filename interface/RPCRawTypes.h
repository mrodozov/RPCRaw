/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#ifndef RPC_RAW_TYPES_H
#define RPC_RAW_TYPES_H

/** 
 * define constants and enumerators 
 */
#include <string>
#include "RPCGhentRawStructure.h"

using namespace std;

enum ESiteFileType {
  kIsCERNrawFile = 1, kIsBARCrawFile , kIsGENTrawFile , kSiteIsNotDefined
};

// branch types enumerator

enum EBranchType {
  kIsCERNchamberChannelBranch = 1, kIsCERNtriggerChannelBranch , kIsCERNnumberOfChannelsBranch , kIsCERNHitsPerChannelBranch
  
};

// chamber types - RE4-3 or RE4-2
// decide the ring - is it like RE1 or its like RE2 or RE3. Put RE3 for now 

const std::string kRPC_RE_4_2_chamber="RE-3_2_01";
const std::string kRPC_RE_4_3_chamber="RE-3_3_01";

// error domains
const std::string kRPCRawConverterErrorDomain="com.rpcrawconverter.app";
const std::string kRPCCommonFormatErrorDomain="com.rpccommonformat.app";

// CERN 
const std::string kRPCisCERNFileNumberOfChannelsBranch="_NUMBER_OF_CHANNELS";
const std::string kRPCisCERNFileHitsPerChannelBranch="_HITS_PER_CHANNEL";
const std::string kRPCisCERNFileChannelBranch="_LE_CH";
// GHENT
const std::string kRPCisGHENTEventBranch="iEvent";
// BARC 
const std::string kRPCisBARCfirstBranch="";


#endif
