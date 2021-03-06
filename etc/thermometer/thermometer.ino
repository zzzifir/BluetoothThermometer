#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Data wire is plugged into port 10 on the Arduino
#define ONE_WIRE_BUS 10
#define TEMPERATURE_PRECISION 9

#define EXEC_SINGLE_MEASUREMENT 0
#define EXEC_CONTINUOUS_MEASUREMENT 1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer;

LiquidCrystal_I2C lcd(0x27,16,2);  // init display

int gRxPin = 9;
int gTxPin = 11;

SoftwareSerial BTSerial(gRxPin, gTxPin);

void initDisplay() 
{
  lcd.init();                     
  lcd.backlight();
  lcd.print("Thermometer");
  delay(1000);
}

void initBluetooth()
{
  BTSerial.begin(9600);
}

void initThermometer()
{
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0))
  { 
    Serial.println("Unable to find address for Device 0"); 
  }
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);  
}

void setup(void)
{
  Serial.begin(9600);
  initDisplay();
  initThermometer();
  initBluetooth();
}

float getTemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  return sensors.getTempC(deviceAddress);
}

void printTemperature(float temp) 
{
  lcd.setCursor(0, 0);
  lcd.print("Temp C: ");
  lcd.print(temp);
  
  lcd.setCursor(0, 1);
  lcd.print("Temp F: ");
  lcd.print(DallasTemperature::toFahrenheit(temp));  
}

void sendTemperature(float temp)
{
  Serial.println(temp);
}

byte getCommand() 
{
  byte inputSymbol;
  while (Serial.available() > 0)
  {
    inputSymbol = Serial.read();
  }
  
  return inputSymbol;
}

void loop(void)
{ 
  byte command = getCommand();
  float temp = getTemperature(insideThermometer);
  printTemperature(temp);
  if (command == EXEC_SINGLE_MEASUREMENT) {
   sendTemperature(temp);
  } else if (command == EXEC_CONTINUOUS_MEASUREMENT) {
    for (int i = 0; i < 60; i++) {
      printTemperature(temp);
      delay(1000);
    }
  }
}

