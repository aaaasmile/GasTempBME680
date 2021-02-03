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

uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};

BoschMgr::BoschMgr(bool useErpomState)
{
    this->_stateUpdateCounter = 0;
    this->_iaq = 0;
    this->_co2Equivalent = 0;
    this->_useEpromState = useErpomState;
}

void BoschMgr::checkIaqSensorStatus(bool debug)
{
    // this function is called between two sensor data
    String output;
    // if (debug)
    // {
    //     Serial.println("Check status");
    // }
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
            if (debug)
            {
                output = "BSEC warning code : " + String(iaqSensor.status);
                Serial.println(output);
            }
        }
    }

    if (iaqSensor.bme680Status != BME680_OK)
    {
        if (iaqSensor.bme680Status < BME680_OK)
        {
            output = "BME680 error code : " + String(iaqSensor.bme680Status);
            Serial.println(output);
            for (;;)
                this->errLeds(); /* Halt in case of failure */
        }
        else
        {
            if (debug)
            {
                output = "BME680 warning code : " + String(iaqSensor.bme680Status);
                Serial.println(output);
            }
        }
    }
    iaqSensor.status = BSEC_OK;
}

void BoschMgr::errLeds(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

void BoschMgr::loadState(void)
{
    if (!this->_useEpromState)
        return;

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
        checkIaqSensorStatus(false);
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

void BoschMgr::updateState(void)
{
    if (!this->_useEpromState)
        return;

    bool update = false;
    /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
    if (this->_stateUpdateCounter == 0)
    {
        if (iaqSensor.iaqAccuracy >= 3)
        {
            update = true;
            this->_stateUpdateCounter++;
        }
    }
    else
    {
        /* Update every STATE_SAVE_PERIOD milliseconds */
        if ((this->_stateUpdateCounter * STATE_SAVE_PERIOD) < millis())
        {
            update = true;
            this->_stateUpdateCounter++;
        }
    }

    if (update)
    {
        iaqSensor.getState(bsecState);
        this->checkIaqSensorStatus(false);

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

void BoschMgr::Setup()
{
    String output;
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length

    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
    Serial.println(output);
    this->checkIaqSensorStatus(true);

    Serial.println("Set config. Using eprom state? " + String(this->_useEpromState));
    iaqSensor.setConfig(bsec_config_iaq);
    this->checkIaqSensorStatus(true);

    this->loadState();

    // IGSA
    // Qui c'è un override dove mancano i sensori CO2 virtuali
    // Di default non hanno il sample rate (vedi funzione void Bsec::beginCommon())
    // Qui di seguito vengono abilitati i sensori virtuali che si desiderano
    // L'abilitazione avviene settando il sample rate come BSEC_SAMPLE_RATE_LP
    // da uno stato di default BSEC_SAMPLE_RATE_DISABLED. La lista che segue non fa altro
    // che settare i valori dei 9 sensori con il sample rate LP. Dei 14 valori possibili, ne setto 9.
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
    this->checkIaqSensorStatus(true);

    // Print the header
    String header = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%]";
    Serial.println(header);
}

float BoschMgr::Next(bool debug)
{
    //Serial.println("Next is called");
    unsigned long time_trigger = millis();
    if (iaqSensor.run())
    {
        // new data are available, that  means the delay between two calls is gone
        this->_valSensors = "TS: " + String(time_trigger);
        this->_valSensors += ", TEMP-RAW: " + String(iaqSensor.rawTemperature);
        this->_valSensors += ", PRES: " + String(iaqSensor.pressure);
        this->_valSensors += ", HUMI-RAW: " + String(iaqSensor.rawHumidity);
        this->_valSensors += ", GASO: " + String(iaqSensor.gasResistance);
        this->_valSensors += ", IAQ: " + String(iaqSensor.iaq);
        this->_valSensors += ", IAQA: " + String(iaqSensor.iaqAccuracy);
        this->_valSensors += ", TEMP: " + String(iaqSensor.temperature);
        this->_valSensors += ", HUMY: " + String(iaqSensor.humidity);
        this->_valSensors += ", CO2: " + String(iaqSensor.co2Equivalent);
        this->_valSensors += ", VOC: " + String(iaqSensor.breathVocEquivalent);
        if (debug)
        {
            Serial.println(this->_valSensors);
        }

        this->_iaq = iaqSensor.iaq;
        this->_co2Equivalent = iaqSensor.co2Equivalent;

        this->updateState();
    }
    else
    {
        this->checkIaqSensorStatus(debug);
    }
    return this->_iaq;
}

String BoschMgr::GetData()
{
    String res = this->_valSensors;
    this->_valSensors = "";
    return res;
}