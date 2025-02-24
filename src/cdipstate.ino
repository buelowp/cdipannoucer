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
#include <LocoNetStreamRP2040.h>
#include "pico/stdlib.h"

#include "cdip.h"

#define LOCONET_PIN_RX  PIN_SERIAL1_RX
#define LOCONET_PIN_TX  PIN_SERIAL1_TX

#define LN_STATUS       PIN_LED_G

#define CDIP_1_GPIO        D0
#define CDIP_2_GPIO        D1
#define CDIP_3_GPIO        D2
#define CDIP_4_GPIO        D3

#define CDIP_1_ADDR         510
#define CDIP_2_ADDR         511
#define CDIP_3_ADDR         500
#define CDIP_4_ADDR         513

#define STATE_CHANGE_TIMEOUT    250

LocoNetBus bus;
LocoNetDispatcher parser(&bus);

CDIP cdip1(false, CDIP_1_ADDR, &bus);
CDIP cdip2(true, CDIP_2_ADDR, &bus);
CDIP cdip3(true, CDIP_3_ADDR, &bus);
CDIP cdip4(true, CDIP_4_ADDR, &bus);

// The line below initialised the LocoNet interface for the correct signal polarity of the IoTT LocoNet Interface board
// See: https://myiott.org/index.php/iott-stick/communication-modules/loconet-interface
LocoNetStreamRP2040 lnStream(&Serial1, LOCONET_PIN_RX, LOCONET_PIN_TX, &bus, true, true);

void LocoNetActiveInterrupt(void)
{
  lnStream.handleLocoNetActivityInterrupt();
}

void cdip1_state(void)
{
    cdip1.active(true);
    cdip1.restartTimer();
}

void cdip2_state(void)
{
    cdip2.active(true);
    cdip2.restartTimer();
}

void cdip3_state(void)
{
    cdip3.active(true);
    cdip3.restartTimer();
}

void cdip4_state(void)
{
    cdip4.active(true);
    cdip4.restartTimer();
}

void setup()
{
    Serial.begin(115200);
    while((!Serial) && (millis() < 5000))
        delay(1);

    Serial.println("LocoNet2 Basic Demo");
    Serial.println("setup: before lnStream.start()");
    lnStream.start();
    Serial.println("setup: after lnStream.start()");

    parser.onPacket(CALLBACK_FOR_ALL_OPCODES, [](const lnMsg *rxPacket) {
        char tmp[100];
        formatMsg(*rxPacket, tmp, sizeof(tmp));
        Serial.printf("onPacket: %s\n", tmp);
    });

    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);

    pinMode(CDIP_1_GPIO, INPUT);
    pinMode(CDIP_2_GPIO, INPUT);
    pinMode(CDIP_3_GPIO, INPUT);
    pinMode(CDIP_4_GPIO, INPUT);

    attachInterrupt(LOCONET_PIN_RX, LocoNetActiveInterrupt, FALLING);

    attachInterrupt(CDIP_1_GPIO, cdip1_state, FALLING);
    attachInterrupt(CDIP_2_GPIO, cdip2_state, FALLING);
    attachInterrupt(CDIP_3_GPIO, cdip3_state, FALLING);
    attachInterrupt(CDIP_4_GPIO, cdip4_state, FALLING);

    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, HIGH);

    cdip3.start();
    Serial.println("Finished Setup");
}

void loop()
{
    lnStream.process();
}