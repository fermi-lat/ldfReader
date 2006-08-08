#ifndef EBFDATAGRAMPARSER_CXX
#define EBFDATAGRAMPARSER_CXX 1

#include "../../src/iterators/EbfDatagramParser.h"

namespace ldfReader {

    int EbfDatagramParser::process(LATdatagram* datagram)
    {
        m_lci.iterate(datagram);

        return m_lci.status();
    }


}
#endif
