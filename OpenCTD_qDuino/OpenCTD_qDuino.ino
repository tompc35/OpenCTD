//EC Circuit uses software serial
#include <SoftwareSerial.h>     // For use by EC circuit.

//OneWire is used to get data from both temperature and conductivity sensors
#include <OneWire.h>  //CAN WE MAKE THIS LIBRARY SMALLER?

//DallasTemperature is used to query the temeprature sensors           
#include <DallasTemperature.h>   //CAN WE MAKE THIS LIBRARY SMALLER?

// For the SD card reader.
#include <SPI.h>               
#include <SD.h>           

//Wire and MS5803_I2C libraries for communicating with the pressure sensor. 

#include <Wire.h>               
#include <SparkFun_MS5803_I2C.h> 
#include <Qduino.h>      

qduino q;

// Declare global variables.
float tempA;
float tempB;
float tempC;

float EC_float = 0;  // Electrical conductivity.

char EC_data[48];    // A 48 byte character array to hold incoming data from the conductivity circuit. 
char *EC;            // Character pointer for string parsing.

byte received_from_sensor = 0;  // How many characters have been received.
byte string_received = 0;       // Whether it received a string from the EC circuit.

double pressure_abs; //define absolute pressure variable

SoftwareSerial ecSerial(8, 9);      // Define the SoftwareSerial port for conductivity.
OneWire oneWire(6);        // Define the OneWire port for temperature.
DallasTemperature sensors(&oneWire);  // Define DallasTemperature input based on OneWire.
MS5803 sensor(ADDRESS_HIGH);          // Define pressure sensor.

// Starts it up.
//
void setup() {

  Serial.begin(9600);   // Set baud rate.
  ecSerial.begin(9600); // Set baud rate for conductivty circuit.

  pinMode(10, OUTPUT);  // Set data output pin for the SD card reader.
  pinMode(4, OUTPUT);   // Set chip select pin for the SD card reader.  

  delay(500);  // Wait half a second before continuing.
  SD.begin(4); // Initialize SD card
  delay(500);  // Wait half a second before continuing.
  
  sensor.reset(); //reset pressure sensor 
  sensor.begin(); //initialize pressure sensor
  
  sensors.begin();  // Intialize the temperature sensors.
  
  delay(250);       // Wait a quarter second to continue.

  // Indicate start of sampling
  File dataStart = SD.open("datalog.txt", FILE_WRITE);
  if (dataStart) {
    dataStart.println("SAMPLE");
    dataStart.close();
  }

  q.setup();    // set up user led on qduino

}

void loop() {

  // Read any pending data from the EC circuit.
  if (ecSerial.available() > 0) {
    received_from_sensor = ecSerial.readBytesUntil(13, EC_data, 48);

    // Null terminate the data by setting the value after the final character to 0.
    EC_data[received_from_sensor] = 0;
  }

  // Parse data, if EC_data begins with a digit, not a letter (testing ASCII values).
  if ((EC_data[0] >= 48) && (EC_data[0] <=57)) {
    parse_data();
  }

  delay(10);  // Wait 10 milliseconds.

  // Read pressure sensor
  pressure_abs = sensor.getPressure(ADC_4096);

  // Read temperature sensors.
  sensors.requestTemperatures();
  tempA = sensors.getTempCByIndex(0);
  tempB = sensors.getTempCByIndex(1);
  tempC = sensors.getTempCByIndex(2);

  // Log to the SD card...
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(pressure_abs);
    dataFile.print("  ");
    dataFile.print(tempA);
    dataFile.print("  ");
    dataFile.print(tempB);
    dataFile.print("  ");
    dataFile.print(tempC);
    dataFile.print("  ");
    dataFile.println(EC);
    dataFile.close();
    q.setRGB(0,64,0);  // set user rgb to green
  }
  else {
    Serial.println("error writing to file");  
    q.setRGB(64,0,0);   // set user rgb to red
  }

  // Log to the serial monitor.
  Serial.print(pressure_abs);
  Serial.print("  "); 
  Serial.print(tempA);
  Serial.print("  ");
  Serial.print(tempB);
  Serial.print("  ");
  Serial.print(tempC);
  Serial.print("  ");
  Serial.print(EC);
  Serial.println("");

  delay(50);  // Wait 50 milliseconds.
}


// Parses data from the EC Circuit.
void parse_data() {

  EC = strtok(EC_data, ",");                  

}
