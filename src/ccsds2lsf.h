#ifndef CCSDS2LSF_H
#define CCSDS2LSF_H 1


#include "eventRet/LSE_Context.h"
#include "eventRet/LSE_Info.h"
#include "lsfDataStore/LsfTimeTone.h"
#include "lsfDataStore/LsfRunInfo.h"
#include "lsfDataStore/LsfDatagramInfo.h"
#include "lsfDataStore/LsfGemScalers.h"
#include "enums/Lsf.h"

#include <map>
#include <string>


/** @class ccsds2lsf 
@brief 

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/DfiParser.h,v 1.1 2006/02/10 19:46:18 heather Exp $
*/

namespace ldfReader {
    class ccsds2lsf  {
    public:

        ccsds2lsf();

        ~ccsds2lsf();

        void timeToneCnv(eventRet::LSE_Context::FromTimetone ccsds, 
                         lsfDataStore::TimeTone &timetone);

        void runInfoCnv(eventRet::LSE_Context::FromRun ccsds,
                        lsfDataStore::RunInfo &run);

        void datagramInfoCnv(eventRet::LSE_Context::FromOpen open, 
                             eventRet::LSE_Context::FromClose close, 
                             lsfDataStore::DatagramInfo &datagram);

        void scalerCnv(eventRet::LSE_Context::FromScalers ccsds, 
                       lsfDataStore::GemScalers& scalers);

    private:


    };
}
#endif
