// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfDebug.cxx,v 1.1 2003/05/20 16:10:08 jrb Exp $
#include "EbfDebug.h"

bool EbfDebug::setDebug(bool debugOn) {
    bool old = m_debug;
    m_debug = debugOn;
    return old;
}
// Do this one inline
// bool EbfDebuf::getDebug() {return m_debug;}

bool EbfDebug::m_debug;
