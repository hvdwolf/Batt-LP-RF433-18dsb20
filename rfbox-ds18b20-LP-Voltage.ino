/*
                 Arduino Nano V3
	                    _____
  RX             D0 -|  N  |- 
  TX             D1 -|  A  |-             GND
  RESET             -|  N  |-             RESET
  GND               -|  O  |-   	        5V
  receive        D2 -|     |- A7
                 D3 -|  V  |- A6      
  transmit       D4 -|  3  |- A5          SCL
  DS18b20        D5 -|     |- A4          SDA
                 D6 -|     |- A3      
                 D7 -|     |- A2      
                 D8 -|     |- A1      
                 D9 -|     |- A0      			 
                D10 -|     |-             AREF
                D11 -|     |-             3.3V			
                D12 -|_____|- D13         LED/SCK


                Voltage measurement

  A0 - -------       +V--------Battery +
             |       |
             |       \
   A         |       / R1 Resistor
   R         |       \
   D         |       |
   U         ---------
   I                 |
   N                 \
   O                 / R2 Resistor
                     \
                     |
  GND - ---------------------- Battery -                       
                

v 0.3    Add Rocketstream LowPower library
v 0.2    Remove led blink
v 0.1    Read temperature and send via RF

 * Generic Sender code : Send a value (counter) over RF 433.92 mhz
 * Fréquence : 433.92 mhz
 * Protocole : homepi 
 * Licence : CC -by -sa
 * Auteur : Yves Grange
 * Version : 0.1
 * Last update : 10/10/2014
 * https://github.com/Yves911/generic_433_sender
 *
 * Based on: Valentin CARRUESCO aka idleman and Manuel Esteban aka Yaug (http://manuel-esteban.com) work  
 * Sonar source: http://arduinobasics.blogspot.nl/2012/11/arduinobasics-hc-sr04-ultrasonic-sensor.html
 */


// Includes
#include <OneWire.h> // http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <DallasTemperature.h> // http://download.milesburton.com/Arduino/MaximTemperature/DallasTemperature_LATEST.zip
#include <LowPower.h>  // https://github.com/hvdwolf/Low-Power

// Define vars
#define senderPin 4
#define ONE_WIRE_BUS 5 // DS18B20 PIN

/*
  Resistors are aligned in series.
  One end goes to Battery - and also to Arduino GND
  The other goes to Battery + and also to Arduino Vin
  The middle (connection between two resistors) goes to Arduino A0
*/
#define ReadVoltagePin A0
long int ResistorR1 = 1000000;  // 1 MΩ
long int ResistorR2 = 470000; // 470 KΩ
float VoltageDivider = (ResistorR1 + ResistorR2) / ResistorR2;


long codeKit = 1000;  // Your unique ID for your Arduino node
int Bytes[30]; 
int BytesData[30]; 
int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

// Start includes
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature
DeviceAddress Sump;


void itob(unsigned long integer, int length)
{  
 for (int i=0; i<length; i++){
   if ((integer / power2(length-1-i))==1){
     integer-=power2(length-1-i);
     Bytes[i]=1;
   }
   else Bytes[i]=0;
 }
}

void itobCounter(unsigned long integer, int length)
{  
 for (int i=0; i<length; i++){
   if ((integer / power2(length-1-i))==1){
     integer-=power2(length-1-i);
     BytesData[i]=1;
   }
   else BytesData[i]=0;
 }
}

unsigned long power2(int power){    //gives 2 to the (power)
 unsigned long integer=1;          
 for (int i=0; i<power; i++){      
   integer*=2;
 }
 return integer;
}

/**
 * Crée notre signal sous forme binaire
**/
void buildSignal()
{
  Serial.println(codeKit);
  // Converti les codes respectifs pour le signal en binaire
  itob(codeKit, 14);
  for(int j=0;j < 14; j++){
   Serial.print(Bytes[j]);
  }
  Serial.println();
}

// Convert 0 in 01 and 1 in 10 (Manchester conversion)
void sendPair(bool b) {
 if(b)
 {
   sendBit(true);
   sendBit(false);
 }
 else
 {
   sendBit(false);
   sendBit(true);
 }
}

//Envois d'une pulsation (passage de l'etat haut a l'etat bas)
//1 = 310µs haut puis 1340µs bas
//0 = 310µs haut puis 310µs bas
void sendBit(bool b) {
 if (b) {
   digitalWrite(senderPin, HIGH);
   delayMicroseconds(650);   //506 orinally, but tweaked.
   digitalWrite(senderPin, LOW);
   delayMicroseconds(2024);  //1225 orinally, but tweaked.
 }
 else {
   digitalWrite(senderPin, HIGH); 
   delayMicroseconds(650);   //506 orinally, but tweaked.
   digitalWrite(senderPin, LOW);
   delayMicroseconds(4301);   //305 orinally, but tweaked.
 }
}

/** 
 * Transmit data
 * @param boolean  positive : if the value you send is a positive or negative one
 * @param long Counter : the value you want to send
 **/
void transmit(boolean positive, unsigned long Counter, int BytesType[], int repeats)
{
 int ii;
 for(ii=0; ii<repeats;ii++)
 {
  int i;
  itobCounter(Counter, 30);

  // Send the unique ID of your Arduino node
  for(i=0; i<14;i++)
 {
  sendPair(Bytes[i]);
 }

  // Send protocol type
 for(int j = 0; j<4; j++)
 {
  sendPair(BytesType[j]);
 }

 // Send the flag to mark the value as positive or negative
 sendPair(positive);

 // Send value (ie your counter)
 for(int j = 0; j<30; j++)
 {
   sendPair(BytesData[j]);
 }

 // Send the flag "End of the transmission"
 digitalWrite(senderPin, HIGH);
 delayMicroseconds(650);     
 digitalWrite(senderPin, LOW);
 delayMicroseconds(8602);
 }
}

void transmitOLD(boolean positive, unsigned long Counter, int BytesType[])
{
 int i;
 itobCounter(Counter, 30);

 // Send the unique ID of your Arduino node
 for(i=0; i<14;i++)
 {
   sendPair(Bytes[i]);
 }

 // Send protocol type
 for(int j = 0; j<4; j++)
 {
   sendPair(BytesType[j]);
 }

 // Send the flag to mark the value as positive or negative
 sendPair(positive);

 // Send value (ie your counter)
 for(int j = 0; j<30; j++)
 {
   sendPair(BytesData[j]);
 }

 // Send the flag "End of the transmission"
 digitalWrite(senderPin, HIGH);
 delayMicroseconds(650);     
 digitalWrite(senderPin, LOW);
 delayMicroseconds(8602);

}


void ReadTemperature()
{
  // Read DS18B20 and transmit value as sensor 1
 float temperature;
 sensors.begin(); //start up temp sensor
 delay(400); //Wait for newly restarted system to stabilize
 (!sensors.getAddress(Sump, 0));
 sensors.setResolution(Sump, 11);
 
 sensors.requestTemperatures(); // Get the temperature
 temperature = sensors.getTempCByIndex(0); // Get temperature in Celcius
 
 unsigned long CounterValue = temperature * 10;
 int BytesType[] = {0,0,0,1};
 transmit(true, CounterValue, BytesType, 6);
 Serial.println(CounterValue);   
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void ReadVoltage()
{
 int sensorValue = analogRead(ReadVoltagePin); //read the A0 pin value
 float voltage = (sensorValue / 1023.00) * readVcc() * VoltageDivider; //convert the value to a true voltage; read correct internal reference voltage
 // Above line and readVcc function from http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
 // Also see https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 // Reduces inaccurracy to 10%
 unsigned long VoltageValue = voltage * 10;
 int BytesType[] = {0,0,0,2};
 transmit(true, VoltageValue, BytesType, 6);
 Serial.println(VoltageValue);
}

void setup()
{
  pinMode(senderPin, OUTPUT);
  buildSignal();
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results. - See more at: http://blog.jacobean.net/?p=353#sthash.JogJVBkM.dpuf
  }

void loop()
{
 // Read the temperature
 ReadTemperature();
 // Read Voltage from battery
 ReadVoltage();
 
 // Enter power down state for 60 s with ADC and BOD module disabled, but that doesn't work. 8s is the max.
 //LowPower.powerDown(SLEEP_60S, ADC_OFF, BOD_OFF);
 // Stupidly do it a number of times -> 5x is 40 seconds
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

