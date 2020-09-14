#include "mbed.h"
#include "SHTC3.h"
#include "Wisol.h"
#include "BQ35100.h"

I2C i2c(I2C_SDA, I2C_SCL);
BQ35100 gauge;
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

#if MBED_CONF_MBED_TRACE_ENABLE
Mutex trace_mutex;

static void trace_wait() {
    trace_mutex.lock();
}

static void trace_release() {
    trace_mutex.unlock();
}

void trace_init() {
    printf("Start\n");
    mbed_trace_init();
    mbed_trace_mutex_wait_function_set(trace_wait);
    mbed_trace_mutex_release_function_set(trace_release);
}
#endif

void factory() {
    gauge.setGaugeMode(BQ35100::ACCUMULATOR_MODE);
    gauge.useInternalTemp(true);
    gauge.setDesignCapacity(3800);
    gauge.setSecurityMode(BQ35100::SECURITY_SEALED);
}

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

    debug("Temp: %f (%u), humidity: %f(%u)\n",
          sht.toCelsius(global.data.temp), global.data.temp,
          sht.toPercentage(global.data.humidity), global.data.humidity
         );
}

void readBatteryPercentage() {
    uint8_t percentage;

    if (!gauge.getRemainingPercentage(&percentage)) {
        debug("Get batt remaining failed\n");
        return;
    }

    global.data.batt = percentage;

    debug("Batt remaining: %u%%\n", percentage);
}

void send() {
    Wisol wisol(WISOL_TX, WISOL_RX);

    if (!wisol.init()) {
        debug("Init FAILED\n");
        return;
    }

    wisol.sendFrame(global.packet, sizeof(global.packet));
}

int main() {
#if MBED_CONF_MBED_TRACE_ENABLE
    trace_init();
#endif

    bool button_state;
    button_state = button.read();
    button.mode(PullNone); // save energy

    i2c.frequency(400000); // speed up a little

    if (gauge.init(&i2c, GAUGE_ENABLE_PIN)) {
        if (button_state == true) { // no button pressed
            if (gauge.enableGauge()) {
                readTemp();
                readBatteryPercentage();
                send();

            } else {
                debug("Gauge enable failed\n");
            }

        } else { // button pressed
            debug("Button is pressed\n");
            gauge.newBattery(0); // use previous capacity
        }

    } else {
        debug("Init gauge FAILED\n");
    }

    gauge.disableGauge(); // this will handle GE pin

    // let the timer know we are done
    done.write(1);
    ThisThread::sleep_for(1ms);
    done.write(0);

    ThisThread::sleep_for(rtos::Kernel::wait_for_u32_forever);
}
