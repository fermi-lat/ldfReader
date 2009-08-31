#ifndef AdfParser_H
#define AdfParser_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/src/iterators/AdfParser.h,v 1.1 2006/07/24 20:07:41 heather Exp $

/** @class AdfParser
* @brief The parsing of the Ancillary BeamTest contributions.
*
*/

namespace ldfReader {

    class AdfParser
    {
    public:
        AdfParser() { };

        virtual ~AdfParser() {}

        int parseHeader(const unsigned char* buf);
        int parseEvent(const unsigned char* buf);
        int parseTrailer(const unsigned char* buf);

    //virtual int handleError(CALcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

    private:
    };
}
#endif

