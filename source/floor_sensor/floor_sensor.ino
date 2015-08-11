#include <DHT.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
#include <SimpleModbusSlaveSoftwareSerial.h>

#define DHT_TYPE DHT22

const int DHT_PIN = 4;

const int BAY_1_PING_PIN = 12;
const int BAY_1_LED_PIN = 13;
const int BAY_1_DOOR_CONTROL_PIN = 11;

const int BAY_2_PING_PIN = 9;
const int BAY_2_LED_PIN = 10;
const int BAY_2_DOOR_CONTROL_PIN = 8;

const int MAX_DISTANCE = 200;

NewPing bay1(BAY_1_PING_PIN, BAY_1_PING_PIN, MAX_DISTANCE);
NewPing bay2(BAY_2_PING_PIN, BAY_2_PING_PIN, MAX_DISTANCE);

DHT dht(DHT_PIN, DHT_TYPE);

enum
{
  HUMIDITY,
  TEMPERATURE_C,
  TEMPERATURE_F,
  TOTAL_ERRORS,
  TOTAL_REGISTERS_SIZE
};

unsigned int holding_registers[TOTAL_REGISTERS_SIZE];

void setup()
{
  Serial.begin(115200);
  Serial.println("Configuring Modbus");
  modbus_configure(9600, 100, 1, TOTAL_REGISTERS_SIZE);
  Serial.println("Starting DHT");
}

void loop()
{
  delay(1000);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to Read from DHT");
  }
  else
  {
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.print(" *C");
    Serial.print("\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature * 1.8 + 32);
    Serial.println(" *F");
  }
    
  holding_registers[TOTAL_ERRORS] = modbus_update(holding_registers);
  holding_registers[HUMIDITY] = humidity * 100;
  holding_registers[TEMPERATURE_C] = temperature * 100;
  holding_registers[TEMPERATURE_F] = (temperature * 1.8 + 32) * 100;
  
  Serial.print("Total Errors: ");
  Serial.println(holding_registers[TOTAL_ERRORS]);
  Serial.print("Humidity (% * 100): ");
  Serial.println(holding_registers[HUMIDITY]);
  Serial.print("Temperature (*C * 100): ");
  Serial.println(holding_registers[TEMPERATURE_C]);
  Serial.print("Temperature (*F * 100): ");
}
