#ifndef AdfParser_CXX
#define AdfParser_CXX 1

/** @file AdfParser.cxx
@brief Implementation of the AdfParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AdfParser.cxx,v 1.5 2009/08/31 18:17:57 jrb Exp $
*/

#include <stdio.h>
#include <assert.h>

// ldfReader includes
//#include "AdfParser.h"
#include "ldfReader/iterators/AdfParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"


namespace ldfReader {

int AdfParser::parseHeader(const unsigned char *buf ) {
    // Purpose and Method:  Unpack Ancillary File Header data from BT 2006
    // Version 200
     unsigned int *h = &((unsigned int*)buf)[0];
     unsigned int ver =(h[0] >> 20) & 0xfff;
     unsigned int runNum = h[2];
     if (EbfDebug::getDebug()) {
         printf("runNum 0x%08X %u\n", runNum, runNum);
         printf("ver: 0x%08X %u, len: 0x%08X %u\n", ver, ver, h[1], h[1]);
     }
    
    ldfReader::LatData* curLatData = ldfReader::LatData::instance();
    curLatData->setRunId(runNum);
    curLatData->setAdfHdrTlr(true);

    return 0;
}

int AdfParser::parseEvent(const unsigned char* buf ) {
    // Purpse and Method:  Unpack Ancillary Event data from BT 2006
    // Version 300
    ldfReader::LatData* curLatData = ldfReader::LatData::instance();
    AdfData* adf = curLatData->getAdfPtr();

    adf->setExist();

    unsigned int *h = &((unsigned int*)buf)[0];
    unsigned int ver =(h[0] >> 20) & 0xfff;
    unsigned int evtLen = h[1];
    adf->initLength(evtLen);
    unsigned int evtNum = h[2] & 0xfffffff;
    adf->initEventNum(evtNum);
    // HMK Unused? unsigned int spillNum = (h[3] >> 12) & 0xffff;
    // HMK Unused? unsigned int spillSize = h[3] & 0xfff;
    adf->initBuffer(buf,evtLen);
    if (EbfDebug::getDebug()) {
        printf("ver: 0x%08X %u, len: 0x%08X %u\n", ver, ver, evtLen, evtLen);
        printf("evtNum : %u\n", evtNum);
    }
    return 0;
}

int AdfParser::parseTrailer(const unsigned char* /*buf*/) {

    ldfReader::LatData* curLatData = ldfReader::LatData::instance();
    curLatData->setAdfHdrTlr(true);

    return 0;
}

}  //end namespace ldfReader

#endif
