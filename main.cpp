#include "mbed.h"
#include "SHTC3.h"
#include "Wisol.h"
#include "BQ35100.h"

I2C i2c(I2C_SDA, I2C_SCL);
BQ35100 gauge(GAUGE_ENABLE_PIN);
DigitalIn button(USER_BUTTON, PullUp);
DigitalOut done(DONE_PIN, 0);

uint8_t temp = UCHAR_MAX;
uint8_t humidity = UCHAR_MAX;
uint8_t batt_percentage = UCHAR_MAX;

uint16_t round10(uint16_t number) {
    uint8_t rem = number % 10;
    return rem >= 5 ? (number - rem + 10) : (number - rem);
}

void readSensor() {
    SHTC3 sht;
    uint16_t raw_temp;
    uint16_t raw_humidity;

    if (!sht.init(&i2c)) {
        debug("SHT init failed\n");
        return;
    }

    if (!sht.read(&raw_temp, &raw_humidity, true)) { // low power mode
        debug("SHT read failed\n");
        return;
    }

    if (raw_temp < 1872) { // -40°C
        temp = 0;

    } else if (raw_temp > 39921) { // +61.6°C
        temp = 254;

    } else {
        // convert -40°C - +61.6°C to 0-254, math round one decimal place
        raw_temp = (((int32_t)raw_temp - 1872) * 2540) / (39921 - 1872);
        raw_temp = round10(raw_temp);
        temp = raw_temp / 10;
    }

    // convert 0-100% to 0-250, math round one decimal place
    raw_humidity = (uint32_t)raw_humidity * 2500 / 65535;
    raw_humidity = round10(raw_humidity);
    humidity = raw_humidity / 10;

    debug("Temperature: %u*C, humidity: %u%%\n", (temp * 4) - 400, humidity * 4);
}

void readBatteryPercentage() {
    if (!gauge.getRemainingPercentage(&batt_percentage)) {
        debug("Get batt remaining failed\n");
        return;
    }

    debug("Batt remaining: %u%%\n", batt_percentage);
}

void send() {
    // Initializing it this was is recommended due to a fact that underling UART instance
    // is destroyed once this function ends, therefore consumes less energy.
    Wisol wisol(WISOL_TX, WISOL_RX);
    uint8_t data[3];

    if (!wisol.init()) {
        debug("Wisol init failed\n");
        return;
    }

    debug("Sending: t: %02X, h: %02X, b: %02X\n", temp, humidity, batt_percentage);
    data[0] = temp;
    data[1] = humidity;
    data[2] = batt_percentage;

    if (wisol.sendFrame(data, sizeof(data))) {
        debug("Sent\n");

    } else {
        debug("Sending failed");
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
            readSensor();
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
