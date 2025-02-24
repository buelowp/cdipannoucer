#pragma once
#include <LocoNetStreamRP2040.h>

class CDIP {
    public:
        CDIP(bool state, bool inverted, int addr, LocoNetBus *bus) : m_active(state), m_inverted(inverted), m_addr(addr), m_bus(bus)
        {
            if (m_inverted)
                m_active = !m_active;

            m_previous = m_active;
            stateChange();
        }
        ~CDIP() {}

        void active(bool state) 
        { 
            m_active = state;
            if (m_inverted)
                m_active = !m_active;

            if (m_previous != m_active)
                stateChange();
        }
        void invert(bool state) { m_inverted = state; }

    private:
        void stateChange() {
            //requestSwitch(m_bus, m_addr, true, m_active);
            if (m_active)
                Serial.printf("Turnout %d: CLOSED", m_addr);
            else
                Serial.printf("Turnout %d: THROWN", m_addr);
        }

        bool m_active;
        bool m_inverted;
        bool m_previous;
        uint16_t m_addr;
        LocoNetBus *m_bus;
};