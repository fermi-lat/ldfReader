#ifndef ldfReader_AcdDigi_H
#define ldfReader_AcdDigi_H

#include <vector>

namespace ldfReader {
    /** @class AcdDigi
      * @brief Local storage of CAL log data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/AcdDigi.h,v 1.1 2004/04/21 22:11:38 heather Exp $
    */
    class AcdDigi {
    public:
        /// PMT ends are labeled by A or B
        typedef enum
        {
            B = 0,
            A
        } PmtSide;

        typedef enum {
            NOERROR = 0,
            ERROR = 1
        } ParityError;

        class AcdPmt {
        public:
            AcdPmt() {};
            AcdPmt(unsigned int pha, int r, PmtSide s, ParityError err = NOERROR) {
                m_pha = pha;
                m_range = r;
                m_side = s;
                m_error = err;
            };

            ~AcdPmt() {};

            void print() const {
                char side = m_side ? 'A' : 'B';
                printf("PHA, range, side, parity error: %d, %d, %c, %d\n", m_pha,  m_range, side, m_error);
            };

            unsigned int getPha() const { return m_pha; };
            int getRange() const { return m_range; };
            const PmtSide getSide() const { return m_side; };
            ParityError getParityError() const { return m_error; };

        private:
            unsigned int m_pha;
            int m_range;
            PmtSide m_side;
            ParityError m_error;
        };



        AcdDigi();
        AcdDigi(const char *name, int tileNumber=0, int id=0);
        ~AcdDigi();

        void clear();
        void init(const char *name, int number = 0, int id =0);
        void print() const ;

        /// add readout to CalXtalReadout collection
        inline void addPmt(const AcdPmt &r) { m_readout.push_back(r); } 

        const char* getTileName() const { return m_tileName; };

        int getTileNumber() const { return m_tileNumber; };

        int getTileId() const { return m_tileId; };

        const std::vector<AcdPmt> getReadout() const { return m_readout; };

    private:

        std::vector<AcdPmt> m_readout;
        int m_tileId;
        const char *m_tileName;
        /// Tile Number as reported by LDF
        int m_tileNumber;


    };
} // end namespace
#endif
