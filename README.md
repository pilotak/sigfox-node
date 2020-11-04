sigfox-th-sensor

Run following code in order to prepare & calibrate the battery gauge. Apply precisely 3.6V to battery clips and place the board to 24°C *(or adjust these values below)*.
> **REALLY** important to power the board via programming header during calibration (no current can flow through battery). Be careful! the board runs at +2V5 so the programmer has to have a same voltage levels or the regulator must not be populated when powering/programming at +3V3.

```cpp
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

    if (gauge.setGaugeMode(BQ35100::ACCUMULATOR_MODE)) {
        debug("Gauge mode set\n");

    } else {
        debug("Set gauge mode failed\n");
        return 0;
    }

    if (!gauge.setDesignCapacity(3800)) {
        debug("Design capacity set\n");

    } else {
        debug("Design capacity set failed\n");
        return 0;
    }

    if (!gauge.setBatteryAlert(0)) { // no alerts
        debug("Alerts cleared\n");

    } else {
        debug("Alert clear failed\n");
        return 0;
    }

    if (gauge.startGauge()) {
        debug("Gauge started\n");

    } else {
        debug("Could not start the gauge\n");
        return 0;
    }

    if (gauge.calibrateVoltage(3600)) { // mV
        debug("Voltage calibration successful\n");

    } else {
        debug("Voltage calibration failed\n");
        return 0;
    }

    if (gauge.performCCOffset()) {
        debug("CC offset successful\n");

    } else {
        debug("CC offset failed\n");
        return 0;
    }

    if (gauge.performBoardOffset()) {
        debug("Board offset successful\n");

    } else {
        debug("Board offset failed\n");
        return 0;
    }

    /*if (gauge.calibrateCurrent(100)) { // mA
        debug("Current calibration successful\n");

    } else {
        debug("Current calibration failed\n");
        return 0;
    }*/

    if (gauge.useInternalTemp(true) && gauge.calibrateTemperature(240)) { // 0.1°C
        debug("Internal temperature calibration successful\n");

    } else {
        debug("Internal temperature calibration failed\n");
        return 0;
    }

    if (gauge.useInternalTemp(false) && gauge.calibrateTemperature(240)) { // 0.1°C
        debug("External temperature calibration successful\n");

    } else {
        debug("External temperature calibration failed\n");
        return 0;
    }

    gauge.setSecurityMode(BQ35100::SECURITY_SEALED);
}
```
