#ifndef ldfReader_LATDATA_H
#define ldfReader_LATDATA_H

#include "TowerData.h"
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

        const DiagnosticData* diagnostic() const { return &m_diagnostic; };
        DiagnosticData* diagnostic() { return &m_diagnostic; };

        const TowerData* getTower(unsigned int id) const;
        TowerData* getTower(unsigned int id);

        const std::map<unsigned int, TowerData*>& getTowerMap() { return m_towerMap; };

        void addTower(TowerData *tower) { m_towerMap[tower->getTowerId()] = tower; };

        void clearTowers();

    private:

        static LatData *m_instance;
        EventSummaryData m_summaryData;
        DiagnosticData m_diagnostic;
        unsigned int m_runId;
        std::map<unsigned int, TowerData*> m_towerMap;

    };
}
#endif
