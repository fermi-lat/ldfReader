#ifndef ldfReader_CalDigi_CXX
#define ldfReader_CalDigi_CXX

/** @file CalDigi.cxx
@brief Implementation of the CalDigi class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/CalDigi.cxx,v 1.2 2003/08/19 04:59:38 heather Exp $
*/


#include "ldfReader/data/CalDigi.h"

namespace ldfReader {

    CalDigi::CalDigi() {
        clear();
    }

    CalDigi::~CalDigi() {
        m_readout.clear();

    }

    void CalDigi::init(unsigned short layer, unsigned short col, CalDigi::CalTrigMode mode) {
        m_layerNum = layer;
        m_columnNum = col;
        m_mode = mode;
    }


    void CalDigi::clear() {
        m_readout.clear();
        m_mode = CalDigi::BESTRANGE;
        m_columnNum = 0;
        m_layerNum = 0;
    }
}
#endif
