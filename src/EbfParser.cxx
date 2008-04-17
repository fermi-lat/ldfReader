#ifndef ldfReader_EbfParser_CXX
#define ldfReader_EbfParser_CXX

#include "EbfDebug.h"

/** @file EbfParser.cxx
@brief Implementation of the EbfParser base class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfParser.cxx,v 1.5 2006/08/01 15:52:15 heather Exp $
*/

#include "ldfReader/data/LatData.h"
#include "ldfReader/EbfParser.h"
namespace ldfReader {

bool EbfParser::setDebug(bool on) {
    return EbfDebug::setDebug(on);
}

int EbfParser::setAcdRemap(const std::string& filename) {

    if (filename != "") {
       return (ldfReader::LatData::instance()->setAcdRemap(filename));
    }
    return -1;
}


void EbfParser::setIgnoreSegFault() {
   ldfReader::LatData::instance()->setIgnoreSegFault(true);

}

void EbfParser::setOldStyleRunId(bool flag) {
   ldfReader::LatData::instance()->setOldStyleRunId(flag);
}

} //namespace
#endif
