#ifndef LATCOMPONENTPARSER_H
#define LATCOMPONENTPARSER_H 1

#include "LATcomponentIterator.h"

/** @class LatComponentParser
@brief Provides callbacks for each component.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatComponentParser.h,v 1.1.1.1 2004/04/15 20:02:23 heather Exp $
*/
namespace ldfReader {
    class LatComponentParser : public LATcomponentIterator
    {
    public:
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

    private:
        unsigned _calSrc;
    };
}
#endif

