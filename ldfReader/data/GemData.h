#ifndef ldfReader_GemData_H
#define ldfReader_GemData_H

#include <vector>
#include <cstdio>

#include "EventSummaryCommon.h"
#include "GEMcontribution.h"

namespace ldfReader {

class GemDataTileList
{
public:
  GemDataTileList()  {}
  GemDataTileList (unsigned short xzm, unsigned short xzp, unsigned short yzm,
               unsigned short yzp, unsigned xy, unsigned short rbn, 
               unsigned short na) :
               m_xzp(xzp), m_xzm(xzm), m_yzp(yzp), m_yzm(yzm), m_xy(xy),
               m_rbn(rbn), m_na(na) { };

  GemDataTileList(const GemDataTileList &tileList) : 
               m_xzp(tileList.m_xzp), m_xzm(tileList.m_xzm), m_yzp(tileList.m_yzp), 
               m_yzm(tileList.m_yzm), m_xy(tileList.m_xy), m_rbn(tileList.m_rbn), 
               m_na(tileList.m_na) { };
               
  ~GemDataTileList() {}

  void init (unsigned short xzm, unsigned short xzp, unsigned short yzm,
               unsigned short yzp, unsigned xy, unsigned short rbn, 
               unsigned short na) { 
               m_xzp = xzp;
               m_xzm = xzm;
               m_yzp = yzp;
               m_yzm = yzm;
               m_xy = xy;
               m_rbn = rbn;
               m_na = na;
               };

  void clear() {
               m_xzp = 0;
               m_xzm = 0;
               m_yzp = 0;
               m_yzm = 0;
               m_xy = 0;
               m_rbn = 0;
               m_na = 0;
  };


  void print() const {
      printf("tile list: \nXZM, XZP = 0x%04x, 0x%04x\n", XZM(), XZP() );
      printf("YZM, YZP = 0x%04x, 0x%04x\n", YZM(), YZP());
      printf("XY = 0x%08x\n", XY());
      printf("RBN, NA = 0x%04x, 0x%04x\n", RBN(), NA());
  }

  unsigned short XZM()  const { return m_xzm; }
  unsigned short XZP()  const { return m_xzp; }
  unsigned short YZM()  const { return m_yzm; };
  unsigned short YZP()  const { return m_yzp; };
  unsigned       XY()   const { return m_xy; };
  unsigned short RBN()  const { return m_rbn; };
  unsigned short NA()   const { return m_na; };

private:
  unsigned short m_xzp;
  unsigned short m_xzm;
  unsigned short m_yzp;
  unsigned short m_yzm;
  unsigned       m_xy;
  unsigned short m_rbn;
  unsigned short m_na;
};


/** @class GemDataOnePpsTime
 *  @brief This is a helper class for parsing the One PPS time structure.
 */
class GemDataOnePpsTime
{
public:
  GemDataOnePpsTime()  {}
  GemDataOnePpsTime(unsigned time, unsigned sec) { 
     m_timebase = time; 
     m_seconds = sec;
  };
  GemDataOnePpsTime(const GemDataOnePpsTime &time) : 
              m_timebase(time.m_timebase), m_seconds(time.m_seconds) {};

  ~GemDataOnePpsTime() {}

  void init(unsigned timebase, unsigned sec) { 
       m_timebase = timebase; 
       m_seconds = sec; 
  };

  void clear() { 
     m_timebase = 0; 
     m_seconds = 0; 
  };

  void print() const {
    printf("OnePPSTime:\n seconds, timebase =  %d, %d\n", m_seconds, m_timebase); 
  }

  unsigned timebase()  const { return m_timebase; };
  unsigned seconds()   const { return m_seconds; };

private:
  unsigned m_timebase;
  unsigned m_seconds;
};

class GemDataCondArrivalTime 
{
public:

  void init(unsigned val) { m_condArr = val; };
  unsigned short external()  const;
  unsigned short cno()       const;
  unsigned short calLE()     const;
  unsigned short calHE()     const;
  unsigned short tkr()       const;
  unsigned short roi()       const;
  unsigned condArr() const { return m_condArr; };

private:
  unsigned m_condArr;
  
};



    /** @class GemData
      * @brief Local storage of GEM data
      * $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/ldfReader/data/GemData.h,v 1.9 2005/04/18 17:36:16 heather Exp $
    */
    class GemData {
    public:

        GemData() { clear(); };
        GemData(const GemData& gem);
        ~GemData() { clear(); };

        void clear();

        // GEM doc v2.5 redefined sent() field as condition_arrival_times
        // We keep both accessors to this field for backward compatibility,
        // but overload the memory location with a union.
        union sent_condArr {
            unsigned sent;
            GemDataCondArrivalTime condArr;
        };


        void initTrigger(unsigned short tkr, unsigned short roi, 
                         unsigned short calLE,
                         unsigned short calHE, unsigned short cno, 
                         unsigned short conditionSummary, 
                         unsigned short missed,
                         const GemDataTileList &tileList);

        void initSummary(unsigned liveTime, unsigned prescaled, 
                         unsigned discarded, GemDataCondArrivalTime condArr,
                         unsigned triggerTime, const GemDataOnePpsTime &time, 
                         unsigned short deltaEvtTime, unsigned short deltaWindOpenTime);

       /* void initSummary(unsigned liveTime, unsigned prescaled, 
                         unsigned discarded, unsigned sent, 
                         unsigned triggerTime, const GemDataOnePpsTime &time, 
                         unsigned short deltaEvtTime, unsigned short deltaWindOpenTime);  */


        void print() const;



       unsigned short tkrVector() const { return m_tkrVector;};
       unsigned short roiVector() const { return m_roiVector;};
       unsigned short calLEvector() const { return m_cal_LE_Vector;};
       unsigned short calHEvector() const { return m_cal_HE_Vector;};
       unsigned short cnoVector() const { return m_cno_Vector;};
       unsigned short conditionSummary() const { return m_conditionSummary;};
       unsigned short missed() const { return m_missed; };
       const GemDataTileList& tileList() const { return m_tileList; };
       unsigned liveTime() const {return m_liveTime; };
       unsigned prescaled() const { return m_prescaled;};
       unsigned discarded() const { return m_discarded;};
       //unsigned sent() const { return m_sent_condArr.sent; };
       GemDataCondArrivalTime condArrTime()   const { return m_sent_condArr.condArr; };
       unsigned triggerTime() const { return m_triggerTime; };
       const GemDataOnePpsTime& onePpsTime() const { return m_onePpsTime; };
       unsigned short deltaEventTime() const { return m_deltaEventTime; };
       unsigned short deltaWindowOpenTime() const { return m_deltaWindOpenTime; };

       const EventSummaryCommon& summary() const { return m_summary; };
       void setSummary(const EventSummaryCommon &summary) { m_summary = summary; };
       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

       void initLength(unsigned long length) { m_lenInBytes = length; };
       unsigned long lenInBytes() const { return m_lenInBytes; };

       void initPacketError(unsigned packetError) { m_packetError=packetError; };
       unsigned packetError() const { return m_packetError; };

    private:

        unsigned short m_tkrVector;
        unsigned short m_roiVector;
        unsigned short m_cal_LE_Vector;
        unsigned short m_cal_HE_Vector;
        unsigned short m_cno_Vector;
        unsigned short m_conditionSummary;
        unsigned short m_missed;
        GemDataTileList m_tileList;
        unsigned  m_liveTime;
        unsigned  m_prescaled;
        unsigned  m_discarded;
        sent_condArr  m_sent_condArr;
        unsigned  m_triggerTime;
        GemDataOnePpsTime m_onePpsTime;
        unsigned short m_deltaEventTime;
        unsigned short m_deltaWindOpenTime;

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;
        bool m_exist;

        unsigned m_packetError;

        unsigned long m_lenInBytes;
       };

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the external trigger bit
* \return The condition arrival time in 50 ns ticks of the external trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::external() const
{ 
  return (m_condArr >> 25) & ((1 << 5) -1);
}

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the cno trigger bit
* \return The condition arrival time in 50 ns ticks of the cno trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::cno()      const
{ 
  return (m_condArr >> 20) & ((1 << 5) -1);
}

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the calHE trigger bit
* \return The condition arrival time in 50 ns ticks of the calHE trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::calHE()    const
{ 
  return (m_condArr >> 15) & ((1 << 5) -1);
}

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the calLE trigger bit
* \return The condition arrival time in 50 ns ticks of the calLE trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::calLE()    const
{ 
  return (m_condArr >> 10) & ((1 << 5) -1);
}

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the tkr trigger bit
* \return The condition arrival time in 50 ns ticks of the tkr trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::tkr()      const
{ 
  return (m_condArr >>  5) & ((1 << 5) -1);
}

/*!
* \brief  Return the condition arrival time in 50 ns ticks of the roi trigger bit
* \return The condition arrival time in 50 ns ticks of the roi trigger bit
*/
inline unsigned short  GemDataCondArrivalTime::roi()      const
{ 
  return (m_condArr     ) & ((1 << 5) -1);
}



    } // end namespace
#endif
