#include "mbed.h"
#include "SHTC3.h"
#include "Wisol.h"
#include "BQ35100.h"

I2C i2c(I2C_SDA, I2C_SCL);
BQ35100 gauge(GAUGE_ENABLE_PIN);
DigitalIn button(USER_BUTTON, PullUp);
DigitalOut done(DONE_PIN, 0);

struct global_data_t {
    uint16_t temp;
    uint16_t humidity;
    uint8_t batt;
};

union global_packet_t {
    global_data_t data = {
        USHRT_MAX,
        0,
        UCHAR_MAX
    };
    uint8_t packet[5];
};

global_packet_t global;

void readTemp() {
    SHTC3 sht;

    if (!sht.init(&i2c)) {
        debug("SHT init failed\n");
        return;
    }

    if (!sht.read(&global.data.temp, &global.data.humidity, true)) {
        debug("SHT read failed\n");
        return;
    }

#if !defined(NDEBUG)
    int16_t t = (int16_t)sht.toCelsius(global.data.temp);
    uint16_t h = (uint16_t)sht.toPercentage(global.data.humidity);
    debug("Temperature: %i*C, humidity: %u%%\n", t, h);
#endif
}

void readBatteryPercentage() {
    if (!gauge.getRemainingPercentage(&global.data.batt)) {
        debug("Get batt remaining failed\n");
        return;
    }

    debug("Batt remaining: %u%%\n", global.data.batt);
}

void send() {
    Wisol wisol(WISOL_TX, WISOL_RX);

    if (!wisol.init()) {
        debug("Init FAILED\n");
        return;
    }

    debug("Sending: t: %u, h: %u, b :%u\n", global.data.temp, global.data.humidity, global.data.batt);

    if (wisol.sendFrame(global.packet, sizeof(global.packet))) {
        debug("Sent\n");
    }
}

int main() {
    bool button_state = (bool)button.read();
    button.mode(PullNone); // save energy

    i2c.frequency(400000); // speed up a little

    if (gauge.init(&i2c)) {
        if (button_state == false) {
            debug("Button is pressed\n");
            gauge.newBattery();
            ThisThread::sleep_for(100ms);
        }

        if (gauge.startGauge()) {
            readTemp();
            readBatteryPercentage();
            send();

        } else {
            debug("Gauge enable failed\n");
        }

        gauge.disableGauge();

    } else {
        debug("Init gauge failed\n");
    }

    // let the timer know we are done
    for (auto i = 0; i < 2; i++) {
        done.write(1);
        ThisThread::sleep_for(2ms);
        done.write(0);
    }

    // We should not get here, but just in case
    ThisThread::sleep_for(rtos::Kernel::wait_for_u32_forever);
}