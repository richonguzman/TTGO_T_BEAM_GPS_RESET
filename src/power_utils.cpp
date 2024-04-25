#include "power_utils.h"
#include "pins_config.h"

#define I2C_SDA 21
#define I2C_SCL 22
#define IRQ_PIN 35

#ifdef HAS_AXP192
XPowersAXP192 PMU;
#endif
#ifdef HAS_AXP2101
XPowersAXP2101 PMU;
#endif

bool    pmuInterrupt;

namespace POWER_Utils {

    void activateGPS() {
        #ifdef HAS_AXP192
        PMU.setLDO3Voltage(3300);
        PMU.enableLDO3();
        #endif
        #ifdef HAS_AXP2101
        PMU.setALDO3Voltage(3300);
        PMU.enableALDO3();
        #endif
    }

    bool begin(TwoWire &port) {
        #ifdef HAS_AXP192
        bool result = PMU.begin(Wire, AXP192_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);
        if (result) {
            PMU.disableDC2();
            PMU.disableLDO2();
            PMU.disableLDO3();
            PMU.setDC1Voltage(3300);
            PMU.enableDC1();
            PMU.setProtectedChannel(XPOWERS_DCDC3);
            PMU.disableIRQ(XPOWERS_AXP192_ALL_IRQ);
        }
        return result;
        #endif

        #ifdef HAS_AXP2101
        bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);
        if (result) {
            PMU.disableDC2();
            PMU.disableDC3();
            PMU.disableDC4();
            PMU.disableDC5();
            PMU.disableALDO1();
            PMU.disableALDO4();
            PMU.disableBLDO1();
            PMU.disableBLDO2();
            PMU.disableDLDO1();
            PMU.disableDLDO2();
            PMU.setDC1Voltage(3300);
            PMU.enableDC1();
            PMU.setButtonBatteryChargeVoltage(3300);
            PMU.enableButtonBatteryCharge();
            PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        }
        return result;
        #endif
    }

    void setup() {
        Wire.end();
        #ifdef HAS_AXP192
        Wire.begin(SDA, SCL);
        if (begin(Wire)) {
            Serial.println("AXP192 init done!");
        } else {
            Serial.println("AXP192 init failed!");
        }
        activateGPS();
        #endif

        #ifdef HAS_AXP2101
        Wire.begin(SDA, SCL);
        if (begin(Wire)) {
            Serial.println("AXP2101 init done!");
        } else {
            Serial.println("AXP2101 init failed!");
        }
        activateGPS();
        #endif
    }

}