#ifndef EBFEXCEPTION_H
#define EBFEXCEPTION_H 1

/** @file EbfException.h
@brief A place to keep definitions of all ebf exception classes
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/EbfException.h,v 1.1 2003/05/14 23:01:15 jrb Exp $
*/

class EbfExceptionBase {
public:
    EbfExceptionBase() {}
};

class EbfBadInstrument : public EbfExceptionBase {
public:
    EbfBadInstrument(const std::string& instrument) : m_name(instrument) {}
    std::string m_name;
};

#endif
