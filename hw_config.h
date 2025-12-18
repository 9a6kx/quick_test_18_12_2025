#ifndef HW_CONFIG_H
#define HW_CONFIG_H

const unsigned int rotary_pushbutton = 22;

const unsigned int start_stop_key_pushbutton = 25;

const unsigned int memory_key_1 = 26;
const unsigned int memory_key_2 = 27;
const unsigned int memory_key_3 = 28;
const unsigned int memory_key_4 = 29;

const unsigned int ext_signal_1 = 2;
// const unsigned int ext_signal_2 = 3;      // NOT IN USE v.1.0.0.

/* ROTARY ENCODER
*/
const unsigned int encoder_pin_A = 24;
const unsigned int encoder_pin_B = 23;

/* BATTERY VOLTAGE MEASUREMENT */
/* only SLAVE command module*/
//const unsigned int voltage_measurement = A2;

/*        OUTPUT DEFINITIONS        */
/*
 HEATER PINS (R-S-T)
 output for Solid State Relays
*/

const unsigned int ssr_1_1 = 39;
const unsigned int ssr_1_2 = 40;
const unsigned int ssr_1_3 = 41;

/* HEATER PINS NOT IN USE v.1.0.0.
const unsigned int ssr_2_1 = 42;
const unsigned int ssr_2_2 = 43;
const unsigned int ssr_2_3 = 44;

const unsigned int ssr_3_1 = 49;
const unsigned int ssr_3_2 = 50;
const unsigned int ssr_3_3 = 51;
*/

/* VENTILATION 
  PCB board (PWM) vent
  SSR forced air cooling vent
  Turbine - ventilacija trake
*/
const unsigned int pcb_vent = 48; 
const unsigned int ssr_vent = 47;
const unsigned int turbine = 46;

/*
  VIBRO MOTOR
*/

const unsigned int vibro_motor = 38;

/* LED DIODES
  RUNNING LED
  STOPPED LED
  WASTE BIN FULL LED
  MEMORY #1 LED
  MEMORY #2 LED
  MEMORY #3 LED
  MEMORY #4 LED
*/

const unsigned int running_led = 35;

const unsigned int stopped_led = 36;

const unsigned int bin_full_led = 37;

const unsigned int memory_1_led = 31;
const unsigned int memory_2_led = 32;
const unsigned int memory_3_led = 33;
const unsigned int memory_4_led = 34;

/* DC GEARBOX ENABLE PWM
  - omogućuje PWM sklop da bi sa generiranim PWM signalom okretao u lijevo ili desno
  - moraju biti obadva enable signala HIGH da bi PWM radio !
  - PWM_L_ENA_SOURCE          6   (D6)  - OMOGUĆAVA SMIJER LIJEVO NA PWM H-MOSTU
  - PWM_R_ENA_SOURCE          8   (D8)  - OMOGUĆAVA SMIJER DESNO NA PWM H-MOSTU
*/

const unsigned int pwm_L_enable = 6;
const unsigned int pwm_R_enable = 8;
const unsigned int pwm_L_source = 5; 
const unsigned int pwm_R_source = 7;

/* RS485 communication */
const unsigned int rs485_RX_TX = 30; // DE i RE pinovi spojeni zajedno ! LOW - prijem , HIGH - predaja
const unsigned int rs485_RX = 19;
const unsigned int rs485_TX = 18;


/* COMMUNICATION VARIABLE DEFINITIONS */

/* I2C konverter za K-tip termopara*/
#define MCP9600_I2C_ADDRESS 0x67 // ADDR Spojen na masu preko 43 kohm otpornika -> adresa 0x67
// u stvarnosti 0x64 jer sam zalemio 13kohm otpornik

/* I2C adresa LCD */
#define LCD_I2C_ADDRESS 0x27 // standardna adresa za I2C LCD
// u stvarnosti 0x3F za poljski LCD

/* MODBUS RS485 */
#define MODBUS_SLAVE_ADDR 1 // Adresa frekventnog regulatora
#define MODBUS_BAUD_RATE 19200 // Baud rate
#define MODBUS_TIMEOUT 1000 // timeout 1 sekunda
/* MODBUS registri*/
#define MODBUS_RUN_STOP 0x2000
#define MODBUS_SET_FREQUENCY 0x2001
#define MODBUS_READ_FREQUENCY 0x3000

#define INSTRUCTION_RUN 0x0001
#define INSTRUCTION_STOP 0x0005


/* LoRa Frekvencija i snaga*/
/* https://www.airspayce.com/mikem/arduino/RadioHead/ */
/* RH_RF95 Works with Semtech SX1276/77/78/79, Modtronix inAir4 and inAir9, and HopeRF RFM95/96/97/98 and other similar LoRa capable radios */
/* CLASS DESCRIPTION:
https://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html
*/
/* Ai-Thinker Ra-02 -> Semtech SX1262 */

/*
ISM:
Prema EU propisima ISM opseg 433.050 - 434.790 MHz
Max snaga 10mW ERP
Duty cycle max 10% - Master/Slave uređaj smije odašiljati najviše 1 sekundu u 10 sekundi // ako je problem cycle preskočiti ovu direktivu
ERC Recommendation 70-03
*/
#define RF95_FREQ 433.650 // 433,650 MHz
#define RF95_TX_POWER 4 // 4dBm
/*
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(RF95_TX_POWER, false);
*/

// LORA PINS RA-02 MODULE
#define RFM95_RST 9 // kontrolira Reset pin LoRa modula
#define RFM95_CS 10 // definiran kroz RH_RF95.h i SPI.h - Chip Select - active low - LoRa modul RX za SPI komandu

#define RFM95_INT 4 // definiran kroz RH_RF95.h i SPI.h - Interrupt - new msg / msg tx ended / error occured
                    // nije HW napravljen VOD na PCB - ručno dodati !!!

// SCK (D13) , MISO(D12) , MOSI(D1) -> definirano kroz SPI.h


//KASNEJŠA UPORABA :)
/*
RH_RF95 rf95(RFM95_CS, RFM95_INT);        // singleton instanca lora radio drivera
*/

/* LoRa PINs: */
/*
  Spojeno na PCB: 
 - RA_02_RESET               9   (D9)  - Ai-Thinker-Ra-02 PIN 4 (RESET)
  
  Incijalizacija varijable:
   #define RFM95_RST 9

   Reset pin je ACTIVE LOW što znači da na 0V je reset modula, moramo postaviti HIGH
   na 3V3 da bi modul normalno radio
*/
    //pinMode(RFM95_RST, OUTPUT);
    //digitalWrite(RFM95_RST, HIGH);

  /*
  Kasnije na kraju setup funkcije bi trebalo napraviti Manual reset LoRa modula

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  
  I provjeru>

   if (!rf95.init()) {
    *** OVDJE IMAMO FAIL LoRa action
    while(true); // Question: Dali ovo implementirati ili stroj može dalje ako nema LoRu ? @ 9a6kx, 06.11.2025. 13:50
  }

  */


#endif
