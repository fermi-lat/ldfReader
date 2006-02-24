#ifndef CCSDS2LSF_CXX
#define CCSDS2LSF_CXX 1


#include "./ccsds2lsf.h"
#include "lsfDataStore/LsfGemTime.h"


namespace ldfReader {

ccsds2lsf::ccsds2lsf() {

}

ccsds2lsf::~ccsds2lsf() {
   
}

void ccsds2lsf::timeToneCnv(eventFile::LSE_Context::FromTimetone ccsds, lsfDataStore::TimeTone &timetone) {
    unsigned char curFlags = 0;
    if (ccsds.missingGps) curFlags |= lsfDataStore::TimeTone::MISSING_GPS_MASK;
    if (ccsds.missingCpuPps) curFlags |= lsfDataStore::TimeTone::MISSING_CPU_MASK;
    if (ccsds.missingTimeTone) curFlags |= lsfDataStore::TimeTone::MISSING_TIMETONE_MASK;
    if (ccsds.missingLatPps) curFlags |= lsfDataStore::TimeTone::MISSING_LAT_MASK;

    lsfDataStore::GemTime curTimeHack(ccsds.timeHack.hacks,
                                      ccsds.timeHack.tics);

    timetone.set(ccsds.incomplete, ccsds.timeSecs, ccsds.flywheeling, 
                 curFlags, curTimeHack);


}


void ccsds2lsf::runInfoCnv(eventFile::LSE_Context::FromRun ccsds, lsfDataStore::RunInfo &run) {

  // HMK Check setting of platform and origin!!!!
    run.set(enums::Lsf::Platform(ccsds.platform), 
            enums::Lsf::DataOrigin(ccsds.origin), 
            ccsds.groundId, 
            ccsds.startedAt); 


}

void ccsds2lsf::datagramInfoCnv(eventFile::LSE_Context::FromOpen open, eventFile::LSE_Context::FromClose close,
               lsfDataStore::DatagramInfo &datagram) {

    // HMK check setting of action and reason - use enums!!!
   datagram.set(enums::Lsf::Open::Action(open.action), 
                enums::Lsf::Open::Reason(open.reason), 
                enums::Lsf::Crate(open.crate),
                enums::Lsf::Mode(open.mode), 
                enums::Lsf::Close::Action(close.action), 
                enums::Lsf::Close::Reason(close.reason), 
                open.datagrams,
                open.modeChanges);

   /*
datagram.set(m_openActionCol[open.actionTxt], 
                m_openReasonCol[open.reasonTxt], 
                m_crateCol[open.crateTxt],
                m_modeCol[open.modeTxt], 
                m_closeActionCol[close.actionTxt], 
                m_closeReasonCol[close.reasonTxt], 
                open.datagrams,
                open.modeChanges);
*/

}

void ccsds2lsf::scalerCnv(eventFile::LSE_Context::FromScalers ccsds, lsfDataStore::GemScalers &scalers) {

    scalers.set(ccsds.elapsed, ccsds.livetime, ccsds.prescaled, 
               ccsds.discarded, ccsds.sequence, ccsds.deadzone);

}


}
#endif
