#ifndef ldfReader_GemData_CXX
#define ldfReader_GemData_CXX

/** @file GemData.cxx
@brief Implementation of the GemData class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/GemData.cxx,v 1.6 2005/02/22 05:54:08 heather Exp $
*/


#include "ldfReader/data/GemData.h"

namespace ldfReader {

    GemData::GemData(const GemData& gem) {
        clear();
        m_tkrVector = gem.m_tkrVector;
        m_roiVector = gem.m_roiVector;
        m_cal_LE_Vector = gem.m_cal_LE_Vector;
        m_cal_HE_Vector = gem.m_cal_HE_Vector;
        m_cno_Vector = gem.m_cno_Vector;
        m_conditionSummary = gem.m_conditionSummary;
        m_tileList = gem.m_tileList;   
        m_liveTime = gem.m_liveTime; 
        m_prescaled = gem.m_prescaled; 
        m_discarded = gem.m_discarded;
        m_sent_condArr = gem.m_sent_condArr;
        m_triggerTime = gem.m_triggerTime;
        m_onePpsTime = gem.m_onePpsTime;
        m_deltaEventTime = gem.m_deltaEventTime;
        m_deltaWindOpenTime = gem.m_deltaWindOpenTime;
        m_exist = gem.m_exist;
        m_lenInBytes = gem.m_lenInBytes;
        m_missed = gem.m_missed;
    }

    void GemData::initTrigger(unsigned short tkr, unsigned short roi,
                  unsigned short calLE, unsigned short calHE, 
                  unsigned short cno, unsigned short conditionSummary, 
                  unsigned short missed,
                  const GemDataTileList &tileList) {
        m_tkrVector = tkr;
        m_roiVector = roi;
        m_cal_LE_Vector = calLE;
        m_cal_HE_Vector = calHE;
        m_cno_Vector = cno;
        m_conditionSummary = conditionSummary;
        m_missed = missed;
        m_tileList = tileList;
    }

                  /*
    void GemData::initSummary(unsigned liveTime, unsigned prescaled, 
                  unsigned discarded, unsigned sent, unsigned triggerTime,
                  const GemDataOnePpsTime &time, unsigned short deltaEvtTime, unsigned short deltaWindOpenTime) {
        m_liveTime = liveTime;
        m_prescaled = prescaled;
        m_discarded = discarded;  
        m_sent_condArr.sent = sent;
        m_triggerTime = triggerTime;
        m_onePpsTime = time;
        m_deltaEventTime = deltaEvtTime;
        m_deltaWindOpenTime = deltaWindOpenTime;
    }
*/

    void GemData::initSummary(unsigned liveTime, unsigned prescaled, 
                  unsigned discarded, GemDataCondArrivalTime condArr, unsigned triggerTime,
                  const GemDataOnePpsTime &time, unsigned short deltaEvtTime, unsigned short deltaWindOpenTime) {
        m_liveTime = liveTime;
        m_prescaled = prescaled;
        m_discarded = discarded;  
        m_sent_condArr.condArr.init(condArr.condArr());
        m_triggerTime = triggerTime;
        m_onePpsTime = time;
        m_deltaEventTime = deltaEvtTime;
        m_deltaWindOpenTime = deltaWindOpenTime;
    }


    void GemData::clear() {
        m_tkrVector = 0;
        m_roiVector = 0;  
        m_cal_LE_Vector = 0;
        m_cal_HE_Vector = 0;
        m_cno_Vector = 0;
        m_conditionSummary = 0;
        m_tileList.clear();
        m_liveTime = 0;
        m_prescaled = 0;
        m_discarded = 0;
        m_sent_condArr.sent = 0;
        m_sent_condArr.condArr.init(0);
        m_triggerTime = 0;
        m_onePpsTime.clear();
        m_deltaEventTime = 0;
        m_deltaWindOpenTime = 0;
        m_exist = false;
        m_lenInBytes = 0;
        m_packetError=0;
        m_missed = 0;
    }

   void GemData::print() const {
       if (!exist()) {
           printf("No GEM\n");
           return;
       }
       printf("GEM:\n");
       printf("Contribution length = %lu Bytes\n", m_lenInBytes);
       printf("ROI vector = 0x%04x\n", roiVector());
       printf("TKR vector = 0x%04x\n", tkrVector());
       printf("CAL HE vector = 0x%04x\n", m_cal_HE_Vector);
       printf("CAL LE vector = 0x%04x\n", m_cal_LE_Vector);
       printf("Condition Summary = 0x%04x\n", m_conditionSummary);
       printf("Missed = 0x%04x\n", m_missed);
       printf("CNO vector        = 0x%04x\n", m_cno_Vector);
       m_tileList.print();
       printf("Live time         = 0x%08x = %d\n", m_liveTime, m_liveTime);
       printf("Prescaled         = 0x%08x = %d\n", m_prescaled, m_prescaled);
       printf("Discarded         = 0x%08x = %d\n", m_discarded, m_discarded);
       printf("Conditional Arrival Time   = 0x%08x = %d\n", condArrTime().condArr() , condArrTime().condArr());
       printf("Trigger Time      = 0x%08x = %d\n", m_triggerTime, m_triggerTime);
       m_onePpsTime.print();
       printf("Delta event time  = 0x%08x = %d\n", m_deltaEventTime, m_deltaEventTime); 
       printf("Delta Window Open Time = 0x%08x = %d\n", m_deltaWindOpenTime, m_deltaWindOpenTime);
   }
}
#endif
