#ifndef ldfReader_CalDigi_H
#define ldfReader_CalDigi_H

#include <vector>

namespace ldfReader {
    /** @class CalDigi
      * @brief Local storage of CAL log data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/CalDigi.h,v 1.3 2004/02/14 05:11:04 heather Exp $
    */
    class CalDigi {
    public:
        /// Xtal ends are labeled by POSitive or NEGative face
        typedef enum
        {
            POS = 0,
            NEG
        } XtalFace;


        /// each Xtal end can report four energy ranges
        typedef enum
        {
            LEX8 = 0,
            LEX1,
            HEX8,
            HEX1
        } AdcRange;

        /// readout can be either best-of-four energy ranges
        /// or all energy ranges
        typedef enum
        {
            BESTRANGE = 0,
            ALLRANGE = 2
        } CalTrigMode;

        /// diode type  -  Large or Small
        typedef enum
        {
            LARGE = 0,
            SMALL = 1
        } DiodeType;

        class CalReadout {
        public:
            CalReadout() {};
            CalReadout(char rangeP, unsigned short adcP, char rangeM, 
                unsigned short adcM) :
            m_adcP(adcP), 
                m_adcM(adcM),
                m_rangeP(rangeP), 
                m_rangeM(rangeM){
                    //m_status = 0;
                };

            ~CalReadout() {};

            void init();

            /// retrieve pulse height from specified face
            inline unsigned short getAdc(CalDigi::XtalFace face) const {return face == CalDigi::POS ? m_adcP : m_adcM;};

            /// retrieve energy range from specified face
            inline char getRange(CalDigi::XtalFace face) const {return face == CalDigi::POS ? m_rangeP : m_rangeM;};



        private:
            /// ADC value from POSitive face
            unsigned short m_adcP;
            /// ADC value from NEGative face
            unsigned short m_adcM;
            /// gain range from POSitive face
            char m_rangeP;
            /// gain range from NEGative face
            char m_rangeM;
            /// status bits for readouts
            //unsigned short m_status;
        };



        CalDigi();
        ~CalDigi();

        void clear();
        void init(unsigned short layer, unsigned short col, CalDigi::CalTrigMode mode);
        void addReadout();

        unsigned short getLayerNum() const{ return m_layerNum; };
        unsigned short getColumnNum() const { return m_columnNum; };
        CalDigi::CalTrigMode getMode() const { return m_mode; };

        /// add readout to CalXtalReadout collection
        inline void addReadout(const CalReadout &r) { m_readout.push_back(r); } 
        //const CalReadoutCol& getReadoutCol() const { return m_readout;}	

        /// Retrieve energy range for selected face and readout
        inline char getRange(short readoutIndex, CalDigi::XtalFace face) const
        {
            return (readoutIndex < (short)m_readout.size()) ? ((m_readout[readoutIndex])).getRange(face) : (char)-1;
        }

        /// Retrieve pulse height for selected face and readout
        inline short getAdc(short readoutIndex, CalDigi::XtalFace face) const
        {
            return (readoutIndex < (short)m_readout.size()) ? ((m_readout[readoutIndex])).getAdc(face) : (short)-1;
        }

        /// Retrieve ranges and pulse heights from both ends of selected readout
        inline const CalReadout* getXtalReadout(short readoutIndex) const
        {
            if ( readoutIndex < (short)m_readout.size() )
                return &(m_readout[readoutIndex]);
            else
                return 0;

        }

        /// Retrieve pulse height from selected range
        inline short getAdcSelectedRange(char range, CalDigi::XtalFace face) const
        {
            char nRanges = (char)m_readout.size();
            if (nRanges == 1)
                return (range == ((m_readout[0])).getRange(face)) ? ((m_readout[0])).getAdc(face) : (short)-1;
            else
                return ((m_readout[(nRanges + range - ((m_readout[0])).getRange(face)) % nRanges])).getAdc(face);
        }


        int getNumReadouts() const { return m_readout.size(); };


    private:

        unsigned short m_layerNum;
        unsigned short m_columnNum;
        CalDigi::CalTrigMode m_mode;

        std::vector<CalReadout> m_readout;


    };
} // end namespace
#endif
