#ifndef ldfReader_LATDATA_H
#define ldfReader_LATDATA_H

#include "TowerData.h"
#include "AcdDigi.h"
#include "GemData.h"
#include "EventSummaryData.h"
#include "DiagnosticData.h"
#include <map>

/** @class LatData
@brief Singleton class to provide global access to LAT data for one event

$Header
*/

namespace ldfReader {

    class LatData {
    public:

        static LatData *instance();
        ~LatData() { clearTowers(); };

    protected:
        LatData();

    public:
        void setSummary(unsigned summary) { m_summaryData.setSummary(summary); };

        void setTime(unsigned sec, unsigned nanosec) { m_summaryData.setTime(sec, nanosec); }
        void setPpcTimeBase(unsigned upperWord, unsigned lowerWord) { m_summaryData.setPpcTimeBase(upperWord, lowerWord); };
        const EventSummaryData& summaryData() { return m_summaryData;};

        void setRunId(unsigned int runId) { m_runId = runId; };
        unsigned int runId() const { return m_runId; };

        void setGem(const GemData& gem) { m_gem = gem; };
        const GemData& getGem() const { return m_gem; };

        void setFormatIdentity(unsigned id) { m_formatIdentity = id; };
        unsigned getFormatIdentity() const { return m_formatIdentity; };

        const DiagnosticData* diagnostic() const { return &m_diagnostic; };
        DiagnosticData* diagnostic() { return &m_diagnostic; };

        const TowerData* getTower(unsigned int id) const;
        TowerData* getTower(unsigned int id);

        const std::map<unsigned int, TowerData*>& getTowerMap() { return m_towerMap; };

        void addTower(TowerData *tower) { m_towerMap[tower->getTowerId()] = tower; };

        const std::map<const char*, AcdDigi*>& getAcdCol() const { return m_acdCol; };
        void addAcd(AcdDigi *acd) { m_acdCol[acd->getTileName()] = acd; };
        const AcdDigi* getAcd(const char *name) const;
        AcdDigi* getAcd(const char *name);

        void clearTowers();

    private:

        static LatData *m_instance;
        EventSummaryData m_summaryData;
        DiagnosticData m_diagnostic;
        unsigned int m_runId;
        std::map<unsigned int, TowerData*> m_towerMap;
        std::map<const char*, AcdDigi*> m_acdCol;
        GemData m_gem;
        // Corresponds to the EBF format identity printed in LDFdump
        // Right now we "know" this is an unsigned from LATtypeId.h
        // Be better to do this in a nicer fashion
        unsigned m_formatIdentity;

    };
}
#endif
