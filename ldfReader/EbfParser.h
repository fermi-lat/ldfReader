#ifndef EbfParser_H
#define EbfParser_H 1

/** @class EbfParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/EbfParser.h,v 1.7 2006/08/01 15:52:15 heather Exp $
*/

#include <string>

namespace ldfReader {
    class EbfParser {
    public:

        EbfParser() { };

        virtual ~EbfParser() { };

        virtual void clear() = 0;

        /// Load data for the current event in the EBF file
        virtual int loadData() = 0;

        /// Moves event pointer to the next event in the EBF file
        virtual int nextEvent() = 0;

        virtual void printHeader() const = 0;

        /// Returns total number of events in input file
        virtual unsigned long long eventCount() const = 0;

        /// Turn on or off debug output.  Returns old value of flag
        virtual bool setDebug(bool on);

        virtual int setAcdRemap(const std::string &filename) ;


        virtual void setIgnoreSegFault();

        /// Set flag that determines what method we use to retrieve the
        /// the run ID.  We may use the LdfReader::runId routine (old I&T)
        /// or use ctx.run.startedAt (real data)
        virtual void setOldStyleRunId(bool flag=true);

        // local exception class
        class Exception{ };

    private:

    };
}
#endif
