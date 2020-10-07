sigfox-th-sensor

```cpp
int main(){    
    if (gauge.init(&i2c)) {
        printf("ok\n");

        if (!gauge.setSecurityMode(BQ35100::SECURITY_UNSEALED)) {
            debug("Error 1\n");
            return 0;
        }

        if (!gauge.setGaugeMode(BQ35100::ACCUMULATOR_MODE)) {
            debug("Error 2\n");
            return 0;
        }

        if (!gauge.useInternalTemp(true)) {
            debug("Error 3\n");
            return 0;
        }

        if (!gauge.setDesignCapacity(3800)) {
            debug("Error 4\n");
            return 0;
        }

        if (!gauge.setBatteryAlert(0)) { // no alerts
            debug("Error 5\n");
            return 0;
        }

        if (!gauge.newBattery()) {
            debug("Error 7\n");
            return 0;
        }

        if (!gauge.setSecurityMode(BQ35100::SECURITY_SEALED)) {
            debug("Error 8\n");
            return 0;
        }

        debug("OK\n");
    }
```