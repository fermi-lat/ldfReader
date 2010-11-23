#ifndef ldfReader_AcdDigi_H
#define ldfReader_AcdDigi_H

#include <vector>

namespace ldfReader {
    /** @class AcdDigi
      * @brief Local storage of CAL log data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/AcdDigi.h,v 1.10.300.1 2010/11/23 16:48:07 heather Exp $
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

        typedef enum 
        {
           NOCABLE=-1
        } UnDef;

        class AcdPmt {
        public:
            AcdPmt():m_pha(0),m_range(0),m_side(B),m_channel(0),
                     m_cable(NOCABLE),m_hit(false),m_accept(false),
                     m_oddError(NOERROR), m_headerParity(NOERROR) {};

            AcdPmt(unsigned int pha, int r, PmtSide s, short c, short m, 
                   bool hit, bool phaAccept, ParityError err = NOERROR, 
                   ParityError headerParity=NOERROR) {
                m_pha = pha;
                m_range = r;
                m_side = s;
                m_channel = c;
                m_cable = NOCABLE;
                m_more = m;
                m_hit = hit;
                m_accept = phaAccept;
                m_oddError = err;
                m_headerParity = headerParity;
            };


             AcdPmt(PmtSide s, short c, bool hit, bool phaAccept, 
                  ParityError err = NOERROR, ParityError headerParity=NOERROR
                  ) 
             {
                 m_side = s;
                 m_channel = c;
                 m_cable = NOCABLE;
                 m_hit = hit;
                 m_accept = phaAccept;
                 m_oddError = err;
                 m_headerParity = headerParity;
                 m_pha = 0;
                 m_range = 0;
             };

             void initPhaValues(unsigned int pha, int r, short more,
                  ParityError err=NOERROR) {
                 m_pha = pha;
                 m_range = r;
                 m_more = more;
                 m_oddError = err;

             }

             void initCable(short c) {
                 m_cable = c;
             }

            ~AcdPmt() {};

            void print(bool header=true) const {
                char side = m_side ? 'A' : 'B';
                if (header) {
                    printf("Chan\tPHA\trange\tside\toddErr\theaderErr\tmore\tVeto\tAccept\n");
                }
                printf("%d\t%d\t%d\t%c\t%d\t%d\t%d\t%d\t%d\n", m_channel, m_pha, m_range, side, m_oddError, m_headerParity, m_more, m_hit, m_accept);
            };

            /// Pulse Height Analysis
            unsigned int getPha() const { return m_pha; };
            int getRange() const { return m_range; };
            // Denotes A or B PMT
            const PmtSide getSide() const { return m_side; };
            ParityError getParityError() const { return m_oddError; };
            ParityError getOddParity() const { return m_oddError; };
            short getChannel() const { return m_channel; };
            short getCable() const { return m_cable; };
            short getMore() const { return m_more; };
            /// Returns corresponding bit from hit map
            bool getHit() const { return m_hit; };
            /// Returns corresponding bit from accept map
            /// Sometimes also called the zero suppression bits
            bool getAccept() const { return m_accept; };
            /// CMD/Data Error in header, also called header parity bit
            ParityError getHeaderParity() const { return m_headerParity; };

            bool operator < (const AcdPmt& a) const{ return m_channel < a.m_channel;}

        private:
            unsigned int m_pha;
            int m_range;
            PmtSide m_side;
            ParityError m_oddError;
            short m_channel;
            short m_cable;
            short m_more;
            bool m_hit; /// veto
            bool m_accept; // PHA threshold
            /// header parity bit (CMD/DATA error) from AEM header 
            ParityError m_headerParity; 
        };



        AcdDigi();
        AcdDigi(const char *name, int tileNumber=0, int id=0);
        ~AcdDigi();

        void clear();
        void init(const char *name, int tileNumber = 0, int id =0);
        void print() const ;

        /// add readout to CalXtalReadout collection
        inline void addPmt(const AcdPmt &r) { m_readout.push_back(r); } 

        const char* getTileName() const { return m_tileName; };

        int getTileNumber() const { return m_tileNumber; };

        int getTileId() const { return m_tileId; };

        const std::vector<AcdPmt> getReadout() const { return m_readout; };

        const AcdPmt* getPmtSide(PmtSide side) const {
            std::vector<AcdPmt>::const_iterator pmtIt;
            for (pmtIt = m_readout.begin(); pmtIt != m_readout.end(); pmtIt++) 
            {
                if ((*pmtIt).getSide() == side)
                    return(&(*pmtIt));
            }
            return 0;
        };

        AcdPmt* getPmtSide(PmtSide side) {
            std::vector<AcdPmt>::iterator pmtIt;
            for (pmtIt = m_readout.begin(); pmtIt != m_readout.end(); pmtIt++) 
            {
                if ((*pmtIt).getSide() == side)
                    return(&(*pmtIt));
            }
            return 0;
        };

    private:

        std::vector<AcdPmt> m_readout;
        int m_tileId;
        char *m_tileName;
        /// Tile Number as reported by LDF
        int m_tileNumber;


    };
} // end namespace
#endif
