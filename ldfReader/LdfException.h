#ifndef LdfException_H
#define LdfException_H 1

/** @file LdfException.h
@brief A place to keep definitions of all ebf exception classes
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/LdfException.h,v 1.1.1.1 2004/04/15 20:02:21 heather Exp $
*/

class LdfExceptionBase {
public:
    LdfExceptionBase() {}
};

class LdfBadInstrument : public LdfExceptionBase {
public:
    LdfBadInstrument(const std::string& instrument) : m_name(instrument) {}
    std::string m_name;
};

#endif
