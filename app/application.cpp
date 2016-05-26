#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/Adafruit_PCD8544/Adafruit_PCD8544.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>
#include "user_interface.h"

#define DS_PIN 0

DS18S20 ReadTemp;
Timer procTimer;

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 12, 5, 4);


static const char degr_symbl[5] = {0x00, 0x07, 0x05, 0x07, 0x00};

void displayTemps() {
    uint8_t a;

    if (!ReadTemp.MeasureStatus()) {
        display.clearDisplay();
        display.setCursor(0, 0);

        for (a = 0; a < ReadTemp.GetSensorsCount(); a++) {
            if (ReadTemp.IsValidTemperature(a)) {
                display.printf("T%d: %.2f %cC\n", a + 1, ReadTemp.GetCelsius(a), (char) 0x7f);
            }
        }
        display.display();
    }
    else {
        display.println("Measure err.");
        Serial.println("Measure err.");
    }


}


void initDisplay() {
    display.begin();
    display.setRotation(2);
    display.setContrast(5);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Measuring...");
    display.display();
}

void init() {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(false); // Enable debug output to serial

    WifiStation.enable(false);
    WifiAccessPoint.enable(false);

    initDisplay();

    ReadTemp.Init(DS_PIN);
    ReadTemp.StartMeasure();

    displayTemps();

    system_deep_sleep(30 * 1000 * 1000);
}

