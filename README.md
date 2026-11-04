### Progetto Sensors HUB

Il progetto ha come obiettivo quello di imparare a programmare su una piattaforma embedded e con il BLE.

La board: STM32WB5MM-DK presenta diversi moduli, tra cui:
- 128 Mbit di memoria Flash (NOR)
- Accelerometro e Giroscopio
- Sensore di Temperatura
- Time-to-Flight
- Rilevamento dei gesti
- Microfono digitale
- LED RGB e LED infrarosso 
- Micro-controllore: STM32WB5MMGH6U

![struttura_ble](docs/img/struttura.png)

Descrizione dello stack BLE, servizi:
- INFO -- servizio in cui sono presenti le diverse informazioni della board
- SENSORS -- servizio che gestisce i diversi sensori



|Servizio| Caratteristica | Proprietà     |
|--------|----------------|---------------|
|INFO    | Uptime         | read          |
|        | Date           | read, write   |
|        |                |               |  
|SENSORS | Accelerometro  | read, notify  |
|        | Temperatura    | read, notify  |