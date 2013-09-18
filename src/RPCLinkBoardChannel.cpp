/** author : Mircho Rodozov, mrodozov@cern.ch 
 * created on : 05.08.13
*/

#include "../interface/RPCLinkBoardChannel.h"

using namespace std;

bool RPCLinkBoardChannel::hasMultipleHits (){
  return (this->hasHit() && this->_vectorOfhits->size() > 1);
}