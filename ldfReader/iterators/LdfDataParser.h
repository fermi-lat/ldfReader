#ifndef LDFDATAPARSER_H
#define LDFDATAPARSER_H 1

#include "LDFiteratorBase.h"
#include "EbfDataParser.h"

/** @class LdfDataParser
@brief Provides callbacks for each component.
$Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/src/iterators/LdfDataParser.h,v 1.2 2008/11/11 04:28:56 heather Exp $
*/
namespace ldfReader {
    class LdfDataParser : public LDFiteratorBase,
                          public EbfDataParser
    {

    public:

//        typedef enum {
//            ID_ORG = 0x101f0010,
//            ID_WITH_OSW = 0x102f0010
//        } EBF_FORMAT_ID;

//        LdfDataParser() : LDFiteratorBase(), EbfDataParser("") { };

        LdfDataParser(ACDmap* a = 0, CALmap* c = 0, TKRmap* t = 0) : 
            LDFiteratorBase(a), EbfDataParser("", a, c, t) { };

        virtual ~LdfDataParser() {}

//        virtual int EBF(const EBFevent *start, const EBFevent *end);

        unsigned int iterate2(const unsigned char* buf, unsigned int len, 
                              bool swap=DFC_BIG_ENDIAN);

//        virtual unsigned int status() { return 0; } //EBFeventIterator.status(); };

        //! Virtual base class methods that are implemented by this class
        //! These are not meant to be reimplemented by subclasses
        virtual void summary(ASCsummary&);
        virtual void cno(ASCcno&);
        virtual void tile(unsigned /*channel*/, const ASCtile&);

        virtual int handleError(const LATcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const LATdatagram*,     unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const ASCcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const ASCtileContribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;

    };
} // end ldfReader namespace
#endif
