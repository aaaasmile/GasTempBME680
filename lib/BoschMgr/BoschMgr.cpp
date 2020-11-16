#include "BoschMgr.h"
#include <EEPROM.h>
#include <Arduino.h>
#include <Wire.h>
#include "bsec.h"

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000)

///////////////// bsec
Bsec iaqSensor;
String output;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

void errLeds(void);

void checkIaqSensorStatus(void)
{
    Serial.println("Check status");
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

void loadState(void)
{
    Serial.println("Load state");
    if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
    {
        // Existing state in EEPROM
        Serial.println("Reading state from EEPROM");

        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
        {
            bsecState[i] = EEPROM.read(i + 1);
            Serial.println(bsecState[i], HEX);
        }

        iaqSensor.setState(bsecState);
        checkIaqSensorStatus();
    }
    else
    {
        // Erase the EEPROM with zeroes
        Serial.println("Erasing EEPROM");

        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
            EEPROM.write(i, 0);

        EEPROM.commit();
    }
}

void updateState(void)
{
    bool update = false;
    /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
    if (stateUpdateCounter == 0)
    {
        if (iaqSensor.iaqAccuracy >= 3)
        {
            update = true;
            stateUpdateCounter++;
        }
    }
    else
    {
        /* Update every STATE_SAVE_PERIOD milliseconds */
        if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis())
        {
            update = true;
            stateUpdateCounter++;
        }
    }

    if (update)
    {
        iaqSensor.getState(bsecState);
        checkIaqSensorStatus();

        Serial.println("Writing state to EEPROM");

        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
        {
            EEPROM.write(i + 1, bsecState[i]);
            Serial.println(bsecState[i], HEX);
        }

        EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
        EEPROM.commit();
    }
}

BoschMgr::BoschMgr()
{
}

void BoschMgr::Setup()
{
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length

    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
    Serial.println(output);
    checkIaqSensorStatus();

    Serial.println("Set config");
    iaqSensor.setConfig(bsec_config_iaq);
    checkIaqSensorStatus();

    loadState();

    // IGSA
    // Qui c'è un override dove mancano i sensori CO2 virtuali
    // Di default non hanno il sample rate (vedi funzione void Bsec::beginCommon())
    // Qui di segiuto vengono abilitati i sensori virtuali che si desiderano
    // L'abilitazione avviene settando BSEC_SAMPLE_RATE_LP
    bsec_virtual_sensor_t sensorList[9] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    };

    iaqSensor.updateSubscription(sensorList, 9, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();

    // Print the header
    output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%]";
    Serial.println(output);
}

void BoschMgr::Next()
{
    unsigned long time_trigger = millis();
    if (iaqSensor.run())
    { // If new data is available
        output = "TS " + String(time_trigger);
        output += ", TEMP-RAW " + String(iaqSensor.rawTemperature);
        output += ", PRES " + String(iaqSensor.pressure);
        output += ", HUMI-RAW " + String(iaqSensor.rawHumidity);
        output += ", GASO " + String(iaqSensor.gasResistance);
        output += ", IAQ " + String(iaqSensor.iaq);
        output += ", IAQA " + String(iaqSensor.iaqAccuracy);
        output += ", TEMP " + String(iaqSensor.temperature);
        output += ", HUMY " + String(iaqSensor.humidity);
        output += ", CO2 " + String(iaqSensor.co2Equivalent);
        output += ", VOC " + String(iaqSensor.breathVocEquivalent);
        Serial.println(output);
        updateState();
    }
    else
    {
        checkIaqSensorStatus();
    }
}