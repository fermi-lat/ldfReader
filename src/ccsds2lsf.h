#ifndef CCSDS2LSF_H
#define CCSDS2LSF_H 1


#include "eventFile/LSE_Context.h"
#include "eventFile/LSE_Info.h"
#include "lsfData/LsfTimeTone.h"
#include "lsfData/LsfRunInfo.h"
#include "lsfData/LsfDatagramInfo.h"
#include "lsfData/LsfGemScalers.h"
#include "enums/Lsf.h"

#include <map>
#include <string>


/** @class ccsds2lsf 
@brief 

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/ccsds2lsf.h,v 1.2 2006/02/24 07:33:21 heather Exp $
*/

namespace ldfReader {
    class ccsds2lsf  {
    public:

        ccsds2lsf();

        ~ccsds2lsf();

        void timeToneCnv(eventFile::LSE_Context::FromTimetone ccsds, 
                         lsfData::TimeTone &timetone);

        void runInfoCnv(eventFile::LSE_Context::FromRun ccsds,
                        lsfData::RunInfo &run);

        void datagramInfoCnv(eventFile::LSE_Context::FromOpen open, 
                             eventFile::LSE_Context::FromClose close, 
                             lsfData::DatagramInfo &datagram);

        void scalerCnv(eventFile::LSE_Context::FromScalers ccsds, 
                       lsfData::GemScalers& scalers);

    private:


    };
}
#endif
