#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

int LedRedpin = D5; 
int LedGreenpin = D6;
int LedYellowpin = D7;

bool SwitchRed = false;
bool SwitchGreen = true;
bool SwitchYellow = false;

void setup()
{
  pinMode(LedRedpin, OUTPUT);
  pinMode(LedGreenpin, OUTPUT);
  pinMode(LedYellowpin, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(F("My BME680 test"));

  if (!bme.begin())
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1)
      ;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop()
{
  if (!bme.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
  if (SwitchRed)
  {
    digitalWrite(LedRedpin, HIGH);
    SwitchRed = false;
    Serial.print("RED light to High");
  }
  else
  {
    digitalWrite(LedRedpin, LOW);
    SwitchRed = true;
  }
  if (SwitchGreen)
  {
    digitalWrite(LedGreenpin, HIGH);
    SwitchGreen = false;
    Serial.print("GREEN light to High");
  }
  else
  {
    digitalWrite(LedGreenpin, LOW);
    SwitchGreen = true;
  }
  if (SwitchYellow)
  {
    digitalWrite(LedYellowpin, HIGH);
    SwitchYellow = false;
    Serial.print("YELLOW light to High");
  }
  else
  {
    digitalWrite(LedYellowpin, LOW);
    SwitchYellow = true;
  }
  
}