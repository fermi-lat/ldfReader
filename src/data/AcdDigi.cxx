#ifndef ldfReader_AcdDigi_CXX
#define ldfReader_AcdDigi_CXX

#include "ldfReader/data/AcdDigi.h"

namespace ldfReader {

    AcdDigi::AcdDigi() { 
      clear();
    }

    AcdDigi::AcdDigi(const char *name, int tileNumber, int id) {
        init(name, tileNumber, id);
    }

    AcdDigi::~AcdDigi() { clear(); }

    void AcdDigi::clear() {
        m_readout.clear();
        m_tileName = "";
        m_tileId = -1;
    }
    
    void AcdDigi::init(const char *name, int number, int id) {
        m_tileName = name;
        m_tileNumber = number;
        m_tileId = id;
    }

    void AcdDigi::print() const {
        printf("AcdDigi\n");
        printf("(tileName, tileNumber, tileId): (  %s, %d, %d )\n", m_tileName, m_tileNumber, m_tileId);
        printf("num readouts: %d\n", m_readout.size());
        unsigned int i;
        for (i = 0; i < m_readout.size(); i++) {
            m_readout[i].print();
        }

    }

} // end namespace
#endif
