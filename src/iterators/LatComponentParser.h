#ifndef LATCOMPONENTPARSER_H
#define LATCOMPONENTPARSER_H 1

#include "LATcomponentIterator.h"

/** @class LatComponentParser
@brief Provides callbacks for each component.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatComponentParser.h,v 1.3 2004/07/29 22:02:10 heather Exp $
*/
namespace ldfReader {
    class LatComponentParser : public LATcomponentIterator
    {
    public:
        typedef enum {
            ID_ORG = 0x101f0010,
            ID_WITH_OSW = 0x102f0010
        } EBF_FORMAT_ID;

        LatComponentParser() : _calSrc(0xffffffff) {}
        virtual ~LatComponentParser() {}

        virtual int UDFcomponent(EBFevent* event, EBFcontribution* contribution);
        virtual int OSWcomponent(EBFevent* event, OSWcontribution* contribution);
        virtual int GLTcomponent(EBFevent* event, GLTcontribution* contribution);
        virtual int GEMcomponent(EBFevent* event, GEMcontribution* contribution);
        virtual int ACDcomponent(EBFevent* event, AEMcontribution* contribution);
        virtual int CALcomponent(EBFevent* event, CALcontribution* contribution);
        virtual int TKRcomponent(EBFevent* event, TKRcontribution* contribution);
        virtual int diagnostic  (EBFevent* event, TEMcontribution* contribution);
        virtual int error       (EBFevent* event, TEMcontribution* contribution);
        virtual int cleanup     (EBFevent* event, TEMcontribution* contribution); 

        int commonComponentData(EBFcontribution *contribution);
        void dumpLATPcellheader(const unsigned header, const char* pfx="  ");

        virtual int handleError(EBFcontribution* contrib, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;

        virtual int handleError(EBFevent* event, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;

    private:
        unsigned _calSrc;
    };
}
#endif

