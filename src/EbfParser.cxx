#ifndef ldfReader_EbfParser_CXX
#define ldfReader_EbfParser_CXX

#include "EbfDebug.h"

/** @file EbfParser.cxx
@brief Implementation of the EbfParser base class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/LdfParser.cxx,v 1.23 2005/04/27 06:01:09 heather Exp $
*/

#include "ldfReader/EbfParser.h"
namespace ldfReader {

bool EbfParser::setDebug(bool on) {
    return EbfDebug::setDebug(on);
}

}


#endif
