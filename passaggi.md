# GasTempBME680
Dopo aver preparato la scheda con il sensore e aver provato con aurduino ide che l'esempio funzioni,
voglio avere un progetto con platform io.
In Visual Code creo un nuovo progetto usando la mia directory custom D:\Arduino\PlatformIO e mettendo
la piattaforma nodemcu 1.0. 
L'upload non mi funziona da VScode, ma devo usare WLC, che poi non è un problema (vedi sotto).

## ArduinoIDE Setup
Vedi il link https://www.mikrocontroller-elektronik.de/nodemcu-esp8266-tutorial-wlan-board-arduino-ide/
(esp8266 json nel device manager)

## Librerie
Il primo problema che ho con il codice preso di pari pari da arduino ide è che le librerie non 
vengono riconosciute. Esse sono Adafruit Sensor e Adafruit BME680.
Vanno installate con PlatformIO. Come?
Sono andato alla PIHome di Visual Code, alla voce Libraries. Qui ho cercato 
Adafruit Unified Sensor 
una volta trovata, ho aggiunto al progetto unsando il bottone "Add to project".
Sembra funzionare in quanto l'errore successivo è correlato alla libreria BME680.
Stessa procedura per BME680 e aggiunto adafruit BME680 a questo progetto tramite PlatformIO Home Library.
Processo molto semplice che funziona con VSCode.
Ora tornando al progetto la compilazione del firmaware funziona senza problemi con:
CTRL + ALT + b
Per la console seriale
CTRL + ALT + s

## Upload sul target (firmware.bin)
Attenzione: il monitor serial di VS code deve essere bloccato prima di fare l'upload.
Per l'installazione di python e del tool esptool.py in WLC vedi il progetto Hello2.
Apri WLC e 
cd /mnt/d/Arduino/PlatformIO/esptool
Check se esptool funziona con:
python esptool.py --port /dev/ttyS4  --baud 115200 flash_id

L'update del target avviene con:
python esptool.py --port /dev/ttyS4  --baud 115200 write_flash --flash_mode dio 0x0 ../GasTempBME680/.pio/build/nodemcuv2/firmware.bin

## Controllare il risultato
Basta aprire il serial monitor con il comando PlatformIO: Serial Monitor CTRL + ALT + s

## Errori
1) ld.exe section `.text1' will not fit in region `iram1_0_seg'
Sembra che si vada out of ram
Dal sito della libreria di bosch https://github.com/BoschSensortec/BSEC-Arduino-library al punto 4.
bisogna cambiare questo file 
C:\Users\igors\.platformio\packages\framework-arduinoespressif8266\tools\sdk\ld
eagle.app.v6.common.ld.h
ed aggiungere alla fine della sezione delle librerie:
*libalgobsec.a:(.literal.* .text.*)
appena prima della
*(.irom0.literal ...
che chiude la sezione.

2) Led error status sensor -2
Il sensore viene inizializzato con una porta sbagliata. Nella libreria bosch si usa:
iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
che è 0x77

Gli errori tipo:
BSEC error code : -34  
si trovano codificati nel file bsec_datatypes.h

3) Problema con #include <ESP8266WiFi.h>
Va messo in main.cpp e allora si può usare anche nella lib.

## Serial Monitor in WLC
Servono due comandi:
stty 115200  -F /dev/ttyS4 raw -echo
cat /dev/ttyS4

## Schema




                                                                   +---------------------------+
                                                                   |                           |
      +------------------+                                         |                        D5 |   +---------------------------->  Led Red
      |                  |                                         |                           |
      |                  |  VCC                                    |                        D6 |   +---------------------------->  Led Green
      |                  +-----------------------------------------|  3v3                      |
      |                  |  GND                                    |                           |
      |                  +-----------------------------------------+  GND                   D7 |   +---------------------------->  Led Yellow
      |     BME680       |  SCL                                    |                           |
      |                  +-----------------------------------------+  D1                       |
      |                  |                                         |                           |
      |                  |  SDA                                    |                           |
      |                  |                                         |             ESP8266       |
      |                  +---------------------------------------->+                           |
      +------------------+                                         |  D2                       |
                                                                   |                        A0 | +----------------------------> Button
                                                                   |                           |
                                                                   +---------------------------+


Il tasto per mostrare lo stato è collegato al pin A0 con una resistenza di 220Ohm. Il tasto
è collegato in diagonale e l'altro capo è 3.3V. Quando il tasto è premuto il valore di A0 va sale a 1024.
Quando non è premuto il valore letto è < 10. Ho visto 1 e 7 nel log.  

## Comunicazione col server
Ho impostato il software per comunicare col server dopo aver collezionato un certo numero di dati.
Dati che vengono mandato come stringa. Campi separati da virgola e recod da \n. Esempio di un record:
TS: 140255, TEMP-RAW: 21.41, PRES: 100174.00, HUMI-RAW: 47.32, GASO: 84509.00, IAQ: 25.00, IAQA: 0, TEMP: 21.35, HUMY: 47.46, CO2: 500.00, VOC: 0.50

Il protocollo usato per comunicare è https. Il client ha memorizzato nel software il fingerprint
del ceritficato del server. 
L'autentificazione avviene per mezzo di un token messo nell'header DeviceToken (shared key).

Nota che il certficato del server Lets Encrypt cambia ogni mese. Quando succede,
ESP8266 non riesce più a collegarsi col server. In questo caso mostra tutti e 3 i led accessi per qualche secondo.

Per questa ragione ho disattivato il fingerprint nell'uploader e l'ho settato inmodalità "insecure" (nessun check del certificato) in quanto il cambio del certificato (renew) ogni mese rende questa app inutilizzabile, se non dopo un fingerprint update. 
La modalità setInsecure(), però, apre la possibilità di un attacco "man in the middle" se
l'indirizzo del mio server viene rediretto su un altro server via DNS. 
Per avere una comunicazione molto più affidabile sul fatto che il server che riceve i dati sia veramente iot.invido.it, e non un fake, bisognerebbe usare setTrustAnchors() con il set
corretto della catena dei certificati basati sul CA del server invido.it. Che poi è quello che 
fa il Browser.
Come riferimento per partire si può usare:
https://github.com/OPEnSLab-OSU/SSLClient/blob/master/TrustAnchors.md

### Database dei dati
Vedi la repository https://github.com/aaaasmile/iot-invido

## Sensore BME680
Il sensore Bosch BME680 legge diversi parametri dell'aria e la libreria bme inclusa calcola a livello software
la qualità e la quantità di CO2. Parametro indiretto dal valore VOC.
La lettura del valore avviene ogni 3 secondi.
Per la libreria ho usato boschsensortec incluso con platformio. La libreria 
non è completamente open source, ma ha un parte binaria che ho messo nella dir external_lib.
L'ho presa da D:\Arduino\PlatformIO\GasTempBME680\.pio\libdeps\nodemcuv2\BSEC Software Library
In questa sotto directory ci sono anche degli esempi che ho usato per 
implementare i files BoschMgr.cpp. Ho usato la versione 1.5.1474.



