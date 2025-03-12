/*
MIT License

Copyright (c) 2025 Peter Buelow

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <LocoNetStreamRP2040.h>

class CDIP {
    public:
        CDIP(bool state, int addr, LocoNetBus *bus, int timeout = 500, bool inverted = false) : m_active(state), m_addr(addr), m_bus(bus), m_timeout(timeout), m_inverted(inverted)
        {
            if (m_inverted)
                m_active = !m_active;

            m_previous = m_active;
        }

        ~CDIP() 
        {
            cancel_repeating_timer(&m_timer);
        }

        void active(bool state) 
        { 
            if (state != m_previous) {
                Serial.printf("%d active\n", m_addr);
                setActive(state);
                m_previous = state;
                stateChange();
            }
        }

        void setInverted(bool inverted) 
        {
            m_inverted = inverted;
        }

        static bool handler(struct repeating_timer *t)
        {
            Serial.printf("static handler\n");
            static_cast<CDIP*>(t->user_data)->active(false);
            return false;
        }

        void restartTimer()
        {
            cancel_repeating_timer(&m_timer);
            add_repeating_timer_ms(m_timeout, handler, this, &m_timer);
        }

        void start()
        {
            add_repeating_timer_ms(m_timeout, handler, this, &m_timer);
            Serial.printf("Started Cobalt IP Digital Monitor for address %d\n", m_addr);
            stateChange();
        }

    private:
        void stateChange() {
            reportSwitch(m_bus, m_addr);
            if (m_active)
                Serial.printf("Turnout %d: THROWN\n", m_addr);
            else
                Serial.printf("Turnout %d: CLOSED\n", m_addr);
        }

        void setActive(bool state) {
            if (m_inverted) {
                m_active = !state;
            }
            else {
                m_active = state;
            }
        }

        bool m_active;
        bool m_inverted;
        bool m_previous;
        uint16_t m_addr;
        int m_timeout;
        LocoNetBus *m_bus;
        struct repeating_timer m_timer;
};