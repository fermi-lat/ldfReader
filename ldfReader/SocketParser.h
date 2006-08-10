#ifndef SocketParser_H
#define SocketParser_H 1

#include "ldfReader/EbfParser.h"
#include <string>
#include "data/LatData.h"
#include "EBFevent.h"
#include "LATdatagram.h"

#ifndef WIN32  // Linux UDF sockets
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include "../src/iterators/EbfDatagramParser.h"

struct sockaddr_in;

/** @class SocketParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure, when using a socket to obtain data.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/SocketParser.h,v 1.1 2006/08/08 17:03:42 heather Exp $
*/

namespace ldfReader {
    class SocketParser : public EbfParser {
    public:

        //SocketParser();

        SocketParser(unsigned int server=1);

        virtual ~SocketParser();

        virtual void clear();

        /// Load data for the current event in the EBF file
        virtual int loadData();

        /// Moves event pointer to the next event in the EBF file
        virtual int nextEvent();

        virtual void printHeader() const { };

        virtual unsigned long long eventCount() const { return m_evtCount; }

        void ignoreSegFault(bool value);

        unsigned int runId() { return m_runId; };

        unsigned int eventId() { return m_eventId; };

        unsigned long eventSize() { return m_eventSize; };

        bool end();

    private:
        double timeForTds();

        // buffer for the event
        unsigned char  *m_rowBuf;   // could just use m_datagram
        long            m_maxSize;
        long            m_evtCount;

        std::string     m_instrument;

        unsigned int m_runId;
        unsigned int m_eventId;
        unsigned long m_eventSize;

        int m_ebfSize;

        unsigned char* m_buffer;   // probably obsolete

        static const unsigned PORT;
        static const unsigned BufferSize;
        static const std::string GROUP;

        //    EBFevent *m_evt, *m_end;
        LATdatagram *m_end, *m_start, *m_datagram;
        EbfDatagramParser *m_datagramParser;

        int m_handle;  // stores return value from socket call
#ifndef WIN32
        struct sockaddr_in m_client_addr;
#else 
        struct sockaddr_in *m_client_addr;  // must be a pointer on windows, can't include winsock2 here
#endif

    };
}
#endif
