#ifndef ldfReader_AcdDigi_H
#define ldfReader_AcdDigi_H

#include <vector>

namespace ldfReader {
    /** @class AcdDigi
      * @brief Local storage of CAL log data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/CalDigi.h,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
    */
    class AcdDigi {
    public:
        /// PMT ends are labeled by A or B
        typedef enum
        {
            B = 0,
            A
        } PmtSide;


        /// each Xtal end can report four energy ranges
        //typedef enum
        //{
        //    LEX8 = 0,
        //    LEX1,
        //    HEX8,
        //    HEX1
        //} AdcRange;

        class AcdPmt {
        public:
            AcdPmt() {};
            AcdPmt(unsigned int pha, int r, PmtSide s) {
                m_pha = pha;
                m_range = r;
                m_side = s;
            };

            ~AcdPmt() {};

     //       void init(unsigned int pha, int r, PmtSide s) { 
     //           m_pha = pha; 
     //           m_range = r; 
     //           m_side = s};

            void print() const {
                char side = m_side ? 'A' : 'B';
                printf("PHA, range, side: %d, %d, %c\n", m_pha,  m_range, side);
            };

        private:
            unsigned int m_pha;
            int m_range;
            PmtSide m_side;
        };



        AcdDigi();
        AcdDigi(const char *name, int id=0);
        ~AcdDigi();

        void clear();
        void init(const char *name, int id =0);
        void print() const ;

        /// add readout to CalXtalReadout collection
        inline void addPmt(const AcdPmt &r) { m_readout.push_back(r); } 

        const char* getTileName() const { return m_tileName; };

        int getTildId() const { return m_tileId; };

    private:

        std::vector<AcdPmt> m_readout;
        int m_tileId;
        const char *m_tileName;


    };
} // end namespace
#endif
