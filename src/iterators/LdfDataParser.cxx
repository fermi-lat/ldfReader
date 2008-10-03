#ifndef LDFDATAPARSER_CXX
#define LDFDATAPARSER_CXX 1

#include "LDFiteratorBase.h"
#include "EbfDataParser.h"
#include "LdfDataParser.h"

/** @class LdfDataParser
@brief Provides callbacks for each component.
$Header$
*/
namespace ldfReader {


//int LdfDataParser::EBF(const EBFevent *start, const EBFevent *end) {
//  return LDFiteratorBase(start, end);
//
//}

unsigned int LdfDataParser::iterate2(const unsigned char* buf, unsigned int len, 
                                    bool swap) {

    return LDFiteratorBase::iterate(buf,len,swap);
}

void LdfDataParser::summary(ASCsummary&) {

}
void LdfDataParser::cno(ASCcno&) {
}

void LdfDataParser::tile(unsigned /*channel*/, const ASCtile&) {
}


int LdfDataParser::handleError(const LATcontribution* c,
                               unsigned code,
                               unsigned p1,
                               unsigned p2) const
{
  int rc = LDFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

int LdfDataParser::handleError(const LATdatagram *datagram,
                               unsigned code,
                               unsigned p1,
                               unsigned p2) const
{
    int rc = LDFiteratorBase::handleError(datagram, code, p1, p2);
   _handleErrorCommon();
   return rc;
}

int LdfDataParser::handleError(const ASCcontribution* c, unsigned code,
                               unsigned p1, unsigned p2) const
{
    int rc = LDFiteratorBase::handleError(c, code, p1, p2);
   _handleErrorCommon();
   return rc;
}

int LdfDataParser::handleError(const ASCtileContribution* c, unsigned code,
                               unsigned p1, unsigned p2) const
{
    int rc = LDFiteratorBase::handleError(c, code, p1, p2);
   _handleErrorCommon();
   return rc;
}

} // end ldfReader namespace

#endif
