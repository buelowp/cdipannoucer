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
#define CDIP_3_ADDR         512
#define CDIP_4_ADDR         513

#define STATE_CHANGE_TIMEOUT    250

LocoNetBus bus;
LocoNetDispatcher parser(&bus);

struct repeating_timer cdip1_timer;
struct repeating_timer cdip2_timer;
struct repeating_timer cdip3_timer;
struct repeating_timer cdip4_timer;

CDIP cdip1(true, false, CDIP_1_ADDR, &bus);
CDIP cdip2(true, false, CDIP_2_ADDR, &bus);
CDIP cdip3(true, false, CDIP_3_ADDR, &bus);
CDIP cdip4(true, false, CDIP_4_ADDR, &bus);

// The line below initialised the LocoNet interface for the correct signal polarity of the IoTT LocoNet Interface board
// See: https://myiott.org/index.php/iott-stick/communication-modules/loconet-interface
LocoNetStreamRP2040 lnStream(&Serial1, LOCONET_PIN_RX, LOCONET_PIN_TX, &bus, true, true);

bool cdip1_inactive(struct repeating_timer *t) 
{
    cdip1.active(false);
    return true;
}

bool cdip2_inactive(struct repeating_timer *t) 
{
    cdip2.active(false);
    return true;
}

bool cdip3_inactive(struct repeating_timer *t) 
{
    cdip3.active(false);
    return true;
}

bool cdip4_inactive(struct repeating_timer *t) 
{
    cdip4.active(false);
    return true;
}

void LocoNetActiveInterrupt(void)
{
  lnStream.handleLocoNetActivityInterrupt();
}

void cdip1_state(void)
{
    cdip1.active(true);
    cancel_repeating_timer(&cdip1_timer);
    add_repeating_timer_ms(250, cdip1_inactive, NULL, &cdip1_timer);
}

void cdip2_state(void)
{
    cdip2.active(true);
    cancel_repeating_timer(&cdip2_timer);
    add_repeating_timer_ms(250, cdip2_inactive, NULL, &cdip2_timer);
}

void cdip3_state(void)
{
    cdip3.active(true);
    cancel_repeating_timer(&cdip3_timer);
    add_repeating_timer_ms(250, cdip3_inactive, NULL, &cdip3_timer);
}

void cdip4_state(void)
{
    cdip4.active(true);
    cancel_repeating_timer(&cdip4_timer);
    add_repeating_timer_ms(250, cdip4_inactive, NULL, &cdip4_timer);
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

    add_repeating_timer_ms(STATE_CHANGE_TIMEOUT, cdip1_inactive, NULL, &cdip1_timer);
    add_repeating_timer_ms(STATE_CHANGE_TIMEOUT, cdip2_inactive, NULL, &cdip2_timer);
    add_repeating_timer_ms(STATE_CHANGE_TIMEOUT, cdip3_inactive, NULL, &cdip3_timer);
    add_repeating_timer_ms(STATE_CHANGE_TIMEOUT, cdip4_inactive, NULL, &cdip4_timer);

    attachInterrupt(LOCONET_PIN_RX, LocoNetActiveInterrupt, FALLING);

    attachInterrupt(CDIP_1_GPIO, cdip1_state, FALLING);
    attachInterrupt(CDIP_2_GPIO, cdip2_state, FALLING);
    attachInterrupt(CDIP_3_GPIO, cdip3_state, FALLING);
    attachInterrupt(CDIP_4_GPIO, cdip4_state, FALLING);

    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    Serial.println("Finished Setup");
}

void loop()
{
    lnStream.process();
}