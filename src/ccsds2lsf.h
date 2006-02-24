#ifndef CCSDS2LSF_H
#define CCSDS2LSF_H 1


#include "eventFile/LSE_Context.h"
#include "eventFile/LSE_Info.h"
#include "lsfDataStore/LsfTimeTone.h"
#include "lsfDataStore/LsfRunInfo.h"
#include "lsfDataStore/LsfDatagramInfo.h"
#include "lsfDataStore/LsfGemScalers.h"
#include "enums/Lsf.h"

#include <map>
#include <string>


/** @class ccsds2lsf 
@brief 

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/ccsds2lsf.h,v 1.1 2006/02/21 19:48:53 heather Exp $
*/

namespace ldfReader {
    class ccsds2lsf  {
    public:

        ccsds2lsf();

        ~ccsds2lsf();

        void timeToneCnv(eventFile::LSE_Context::FromTimetone ccsds, 
                         lsfDataStore::TimeTone &timetone);

        void runInfoCnv(eventFile::LSE_Context::FromRun ccsds,
                        lsfDataStore::RunInfo &run);

        void datagramInfoCnv(eventFile::LSE_Context::FromOpen open, 
                             eventFile::LSE_Context::FromClose close, 
                             lsfDataStore::DatagramInfo &datagram);

        void scalerCnv(eventFile::LSE_Context::FromScalers ccsds, 
                       lsfDataStore::GemScalers& scalers);

    private:


    };
}
#endif
