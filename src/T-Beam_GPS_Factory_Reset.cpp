/*************************************************************************
Purpose: to reset U-blox NEO GPS devices on
- TTGO T-Beam V1.0
- TTGO T-Beam V1.1 (use V1.0)
- TTGO T-Beam V1.2

This sketch will bring back U-blox GPS N6M & N8M factory settings 
so that NMEA 9600 over the GPS serial output is enabled.
**************************************************************************/


#include "power_utils.h"
#include "pins_config.h"
#include <SparkFun_Ublox_Arduino_Library.h>

SFE_UBLOX_GPS myGPS;
int state = 0; // steps through states
HardwareSerial SerialGPS(1);

void setup() {
	Serial.begin(115200);
	while (!Serial);  // Wait for user to open the terminal
	Serial.println("Connected to Serial");

	POWER_Utils::setup();

	SerialGPS.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
	Serial.println("All comms started");
	delay(100);
}

void loop() {
	Serial.println();
	Serial.print("===== STATE ");
	Serial.println(state);
	
	switch (state) {
		case 0: // soft solution, should be sufficient and works in most (all) cases
			do {
				if (myGPS.begin(SerialGPS)) {
					Serial.println("Connected to GPS");
					myGPS.setUART1Output(COM_TYPE_NMEA); //Set the UART port to output NMEA only
					myGPS.saveConfiguration(); //Save the current settings to flash and BBR
					Serial.println("GPS serial connected, output set to NMEA");
					myGPS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1);
					myGPS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
					myGPS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
					myGPS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
					myGPS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
					myGPS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1);
					myGPS.enableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
					myGPS.saveConfiguration(); //Save the current settings to flash and BBR
					break;
				}
				delay(1000);
			}  while(1);
			Serial.println("Saved config");
			state++;
			break;

		case 1: // hardReset
			Serial.println("Issuing hardReset (cold start)");
			myGPS.hardReset();
			delay(3000);
			SerialGPS.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
			if (myGPS.begin(SerialGPS)) {
				Serial.println("Success.");
				state++;
			} else {
				Serial.println("*** GPS did not respond, starting over.");
				state = 0;
			}
			break;
				
		case 2: // factoryReset, expect to see GPS back at 9600 baud
			Serial.println("Issuing factoryReset");
			myGPS.factoryReset();
			delay(3000); // takes more than one second... a loop to resync would be best
			SerialGPS.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
			if (myGPS.begin(SerialGPS)) {
				Serial.println("Success.");
				Serial.println("Now GPS has been reset with factory settings,");
				Serial.println("give us some time to acquire satellites...");
				Serial.println();
				state++;
			} else {
				Serial.println("*** GPS did not respond, starting over.");
				state = 0;
			}
			break;

		case 3: // lets test
			for (uint32_t thisNr = 1; thisNr < 300000000; thisNr++) {
				if (SerialGPS.available()) {
					Serial.write(SerialGPS.read());  // print anything comes in from the GPS
				}
			}
	}
}