sigfox-th-sensor
[![build](https://github.com/pilotak/sigfox-th-sensor/workflows/build/badge.svg)](https://github.com/pilotak/sigfox-th-sensor/actions)
[![Framework Badge mbed](https://img.shields.io/badge/framework-mbed-008fbe.svg)](https://os.mbed.com/)

## Calibration
Below you can find details how to calibrate the battery gauge

<details>
<summary>Basic calibration</summary>

Run following code in order to prepare & calibrate the battery gauge. Apply precisely 3.6V to battery clips and place the board to 24°C *(or adjust these values below)*.
> **Really important** to power the board via programming header during calibration (no current can flow through battery).
Be careful! the board runs at +2V5 so the programmer has to have a same voltage levels or the regulator must not be populated when powering/programming at +3V3.

```cpp
#include "mbed.h"
#include "BQ35100.h"

BQ35100 gauge(GAUGE_ENABLE_PIN);
I2C i2c(I2C_SDA, I2C_SCL);

int main() {
    if (gauge.init(&i2c)) {
        debug("Init OK\n");

    } else {
        debug("Could not init the gauge\n");
        return 0;
    }

    if (gauge.setSecurityMode(BQ35100::SECURITY_UNSEALED)) {
        debug("Device unsealed\n");

    } else {
        debug("Unseal failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.setGaugeMode(BQ35100::ACCUMULATOR_MODE)) {
        debug("Gauge mode set\n");

    } else {
        debug("Set gauge mode failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.setDesignCapacity(3800)) {
        debug("Design capacity set\n");

    } else {
        debug("Design capacity set failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.setBatteryAlert(0)) { // no alerts
        debug("Alerts cleared\n");

    } else {
        debug("Alert clear failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.startGauge()) {
        debug("Gauge started\n");

    } else {
        debug("Could not start the gauge\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.calibrateVoltage(3600)) { // mV
        debug("Voltage calibration successful\n");

    } else {
        debug("Voltage calibration failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.performCCOffset()) {
        debug("CC offset successful\n");

    } else {
        debug("CC offset failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.performBoardOffset()) {
        debug("Board offset successful\n");

    } else {
        debug("Board offset failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.useInternalTemp(true) && gauge.calibrateTemperature(240)) { // 0.1°C
        debug("Internal temperature calibration successful\n");

    } else {
        debug("Internal temperature calibration failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.useInternalTemp(false) && gauge.calibrateTemperature(240)) { // 0.1°C
        debug("External temperature calibration successful\n");

    } else {
        debug("External temperature calibration failed\n");
        return 0;
    }

    debug("Done\n");

    gauge.setSecurityMode(BQ35100::SECURITY_SEALED);
}
```
</details>

<details>
<summary>Current calibration</summary>

Run following code in order to calibrate flow current for battery gauge. Apply 100mA load *(or adjust these values below)* between "battery +" and GND (**not** battery -)
```cpp
#include "mbed.h"
#include "BQ35100.h"

BQ35100 gauge(GAUGE_ENABLE_PIN);
I2C i2c(I2C_SDA, I2C_SCL);

int main() {
    if (gauge.init(&i2c)) {
        debug("Init OK\n");

    } else {
        debug("Could not init the gauge\n");
        return 0;
    }

    ThisThread::sleep_for(5s);

    if (gauge.setSecurityMode(BQ35100::SECURITY_UNSEALED)) {
        debug("Device unsealed\n");

    } else {
        debug("Unseal failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.setGaugeMode(BQ35100::ACCUMULATOR_MODE)) {
        debug("Gauge mode set\n");

    } else {
        debug("Set gauge mode failed\n");
        return 0;
    }

    ThisThread::sleep_for(1s);

    if (gauge.startGauge()) {
        debug("Gauge started\n");

    } else {
        debug("Could not start the gauge\n");
        return 0;
    }

    debug("Apply 100mA current within next 10s\n");

    ThisThread::sleep_for(10s);

    if (gauge.calibrateCurrent(100)) { // mA
        debug("Current calibration successful\n");

    } else {
        debug("Current calibration failed\n");
        return 0;
    }

    debug("Done\n");

    gauge.setSecurityMode(BQ35100::SECURITY_SEALED);
}
```
</details>
