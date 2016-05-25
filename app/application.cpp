#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/Adafruit_PCD8544/Adafruit_PCD8544.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>

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

void readData() {
    uint8_t a;
    uint64_t info;

    if (!ReadTemp.MeasureStatus())  // the last measurement completed
    {
        display.clearDisplay();
        display.setCursor(0, 0);

        if (ReadTemp.GetSensorsCount())   // is minimum 1 sensor detected ?
            Serial.println("******************************************");

        for (a = 0; a < ReadTemp.GetSensorsCount(); a++)   // prints for all sensors
        {
            Serial.print(" T");
            Serial.print(a + 1);
            Serial.print(" = ");
            if (ReadTemp.IsValidTemperature(a))   // temperature read correctly ?
            {
                Serial.print(ReadTemp.GetCelsius(a));
                Serial.print(" Celsius, (");
                Serial.print(ReadTemp.GetFahrenheit(a));
                Serial.println(" Fahrenheit)");

                display.printf("T%d: %.2f %cC\n", a + 1, ReadTemp.GetCelsius(a), (char) 0x7f);
                display.print(degr_symbl);
            }
            else
                Serial.println("Temperature not valid");

            Serial.print(" <Sensor id.");

            info = ReadTemp.GetSensorID(a) >> 32;
            Serial.print((uint32_t) info, 16);
            Serial.print((uint32_t) ReadTemp.GetSensorID(a), 16);
            Serial.println(">");
        }
        display.display();
        Serial.println("******************************************");
        ReadTemp.StartMeasure();  // next measure, result after 1.2 seconds * number of sensors
    }
    else
        Serial.println("No valid Measure so far! wait please");


}


void displayTest() {
    display.begin();
    display.setRotation(2);
    display.setContrast(5);
    display.display(); // show splashscreen
}

void init() {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(false); // Enable debug output to serial

    WifiStation.enable(false);
    WifiAccessPoint.enable(false);

    displayTest();

    ReadTemp.Init(16);

    ReadTemp.StartMeasure(); // first measure start,result after 1.2 seconds * number of sensors
    procTimer.initializeMs(10000, readData).start();   // every 10 seconds

}

