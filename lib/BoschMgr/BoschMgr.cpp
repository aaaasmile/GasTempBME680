#include "BoschMgr.h"
#include <Arduino.h>
#include <Wire.h>
#include "bsec.h"

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

///////////////// bsec
Bsec iaqSensor;
String output;
void errLeds(void);

void checkIaqSensorStatus(void)
{
    if (iaqSensor.status != BSEC_OK)
    {
        if (iaqSensor.status < BSEC_OK)
        {
            output = "BSEC error code : " + String(iaqSensor.status);
            Serial.println(output);
            for (;;)
                errLeds(); /* Halt in case of failure */
        }
        else
        {
            output = "BSEC warning code : " + String(iaqSensor.status);
            Serial.println(output);
        }
    }

    if (iaqSensor.bme680Status != BME680_OK)
    {
        if (iaqSensor.bme680Status < BME680_OK)
        {
            output = "BME680 error code : " + String(iaqSensor.bme680Status);
            Serial.println(output);
            for (;;)
                errLeds(); /* Halt in case of failure */
        }
        else
        {
            output = "BME680 warning code : " + String(iaqSensor.bme680Status);
            Serial.println(output);
        }
    }
    iaqSensor.status = BSEC_OK;
}

void errLeds(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

BoschMgr::BoschMgr()
{
}

void BoschMgr::Setup()
{
    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
    Serial.println(output);
    checkIaqSensorStatus();

    iaqSensor.setConfig(bsec_config_iaq);
    checkIaqSensorStatus();

    //loadState();

    bsec_virtual_sensor_t sensorList[7] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();

    // Print the header
    output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%]";
    Serial.println(output);
}

void BoschMgr::Loop()
{
    unsigned long time_trigger = millis();
    if (iaqSensor.run())
    { // If new data is available
        output = String(time_trigger);
        output += ", " + String(iaqSensor.rawTemperature);
        output += ", " + String(iaqSensor.pressure);
        output += ", " + String(iaqSensor.rawHumidity);
        output += ", " + String(iaqSensor.gasResistance);
        output += ", " + String(iaqSensor.iaq);
        output += ", " + String(iaqSensor.iaqAccuracy);
        output += ", " + String(iaqSensor.temperature);
        output += ", " + String(iaqSensor.humidity);
        Serial.println(output);
        //updateState();
    }
    else
    {
        checkIaqSensorStatus();
    }
}