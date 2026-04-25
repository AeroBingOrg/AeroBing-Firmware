#include <MS5611.h>

//SPI mode
#define MS5_CS 68

//Software SPI mode (spi2)
#define SPI2_MOSI 90
#define SPI2_MISO 88
#define SPI2_SCK  89

SPIClass spi2 = SPIClass(SPI2_MOSI, SPI2_MISO, SPI2_SCK);

//for altitude calc
#define SEALEVELPRESSURE_HPA (1013.25)

MS5611_SPI ms5 = MS5611_SPI(MS5_CS, &spi2);

void setup() {
    Serial.begin(115200);
    while (!Serial)
        Serial.println("MS5611 test");

    if (!ms5.isConnected()) {
        Serial.println("Could not find valid MS5611 sensor.");
        while(1);
    }

    Serial.println("Sensor found.");
}

void loop() {
    Serial.println("Reading MS5611 data...");
    if (ms5.read() != 0) {
        Serial.println("Failed to read data.");
        while(1);
    }

    Serial.print("Temperature: ");
    Serial.println(ms5.getTemperature());
    Serial.print("Pressure (Pa): ");
    Serial.println(ms5.getPressurePascal());
    Serial.print("Altitude: ");
    Serial.println(ms5.getAltitude(SEALEVELPRESSURE_HPA));

    delay(2000);
}