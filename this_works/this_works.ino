#include <HardwareSerial.h>
#include <Arduino.h>
#include "HX711_ADC.h"

#define RXD2 16
#define TXD2 17

//-- stepper motor conveyor definitions
#define stepPin 4 //pulse
#define dirPin 2
#define enPin 15

float xx = 0;
boolean newDataReady = false;
unsigned long t = 0;

// HX711 circuit wiring
const int HX711_dout = 18;
const int HX711_sck = 19;

int reading_prev;

HX711_ADC LoadCell(HX711_dout, HX711_sck);

//-- FreeRTOS for ESP32 using single processor core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

bool homeOnce = 1;

//--      FLAGS      --//
static volatile uint8_t commandReady = 0;

//--      DEFINITIONS      --//
static char *commandPtr = NULL;
static const uint8_t bufferLength = 255; //-- length of serial buffer is 255

//-- Read serial input task
void readSerial(void *parameters) {
  
  char c; //-- input input character from serial input
  char command[bufferLength];  //-- character buffer or 'queue'
  uint8_t index = 0;  //-- buffer index 'idx'

  memset(command, 0, bufferLength); //-- set the buffer to empty so new commands can be inputted

  //-- start task
  while (1) {

    if (Serial.available() > 0) {

      c = Serial.read();  //-- store the serial character to the stack variable 'c'

      //-- check the buffer length is not overlowing and another character can be stored in the array
      if (index < bufferLength - 1) {

        command[index] = c; //-- store the recieved character into the character buffer array
        index++;  //-- incriment the buffer counter by 1, as 1 character has been added to the buffer
        
      }

      //-- check if the character recieved is a new line character, indicating the end of the commmand
      if (c == '\n') {

        //-- replace the last 'new line' character with a 'null terminator' to null terminate the character array (string)
        command[index - 1] = '\0';

        if (commandReady == 0) {  //-- command not ready, so we can make it ready ig

          commandPtr = (char *)pvPortMalloc(index * sizeof(char));  //-- copy the character array to the static character array 'commandPtr'
          configASSERT(commandPtr);
          memcpy(commandPtr, command, index);
          commandReady = 1; //-- callback task can now print and process the recieved command!
          
          
        }
      //-- upon new line recieved
      memset(command, 0, bufferLength);
      index = 0;
      }
    }
  }
}

void compute_weight(void *parameters) {

  LoadCell.setSamplesInUse(32); //-- default 16 to now 64

  while(1) {

    
    
    // if (LoadCell.update()) {
    //   xx = LoadCell.getData();
    // }
   
    LoadCell.update();
    xx = LoadCell.getData();

//    Serial.print("frick: ");
//    Serial.println(xx);
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }

  
}

void testingArmCommands(void *parameters) {

  digitalWrite(enPin,LOW); //-- disabled by default, low to run

  

  while(1) {
    
    if (homeOnce) {
      vTaskDelay(1500 / portTICK_PERIOD_MS);
//      Serial.println("STEP 1: HOME THE BOT");
      Serial2.println("HOME");
      //vTaskDelay(26000 / portTICK_PERIOD_MS);
      homeOnce = 0;
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      

    }
    String loc;


    vTaskDelay(1000 / portTICK_PERIOD_MS);  //-- this may (may be able to be reduced?)
//    Serial.println("AIR now, close the bois");
    Serial2.println("AIR,V");
    vTaskDelay(850 / portTICK_PERIOD_MS);
//    Serial.println("STEP 3: MOVE TO PICKUP");
    Serial2.println("MOVE,1,6.25,0"); //-- move to pickup location
    vTaskDelay(850 / portTICK_PERIOD_MS);
    if (loc == ("MOVE,-13,-92.5,0")) {
      vTaskDelay(1600 / portTICK_PERIOD_MS);
    }
//    Serial.println("STEP 4: RAM DOWN");
    Serial2.println("AIR,U"); //-- RAM DOWN
    Serial2.println("AIR,U"); //-- RAM DOWN
    vTaskDelay(550 / portTICK_PERIOD_MS);
//    Serial.println("STEP 5:");
    Serial2.println("AIR,B"); //-- air b for air bud (open claws)
    vTaskDelay(230 / portTICK_PERIOD_MS);
//    Serial.println("STEP 6: air up");
    Serial2.println("AIR,D");
    vTaskDelay(300 / portTICK_PERIOD_MS);
    Serial2.println("MOVE,22,-21,0"); //-- weight location
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.println("AIR,U"); //-- ram down
    vTaskDelay(600 / portTICK_PERIOD_MS);
//    Serial.println("AIR now, close the bois");
    Serial2.println("AIR,V");
    //vTaskDelay(400 / portTICK_PERIOD_MS);
    //Serial2.println("AIR,D");
    vTaskDelay(2200 / portTICK_PERIOD_MS);  //-- ram up
    
    //-- take measurment

    Serial.print("Data: ");

    if (xx > 11.1) {
      loc = "MOVE,-34,-10,0";
    } else if (xx < 4) {
      loc = "MOVE,-55,-10,0";
    } else {
      loc = "MOVE,-13,-92.5,0";
    }
    Serial.print(xx);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.println("AIR,U");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.println("AIR,B");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.println("AIR,D"); //-- ram up
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.println(loc);
    vTaskDelay(500 / portTICK_PERIOD_MS);
//    Serial.println("STEP 7:");
    // Serial2.println("AIR,U");
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
//    Serial.println("STEP 8:");
    Serial2.println("AIR,V");
    vTaskDelay(300 / portTICK_PERIOD_MS); 
// //    Serial.println("STEP 9:");
//     Serial2.println("AIR,D");  //-- d for down but it goes up
    if(loc == ("MOVE,-13,-92.5,0")){
      vTaskDelay(550 / portTICK_PERIOD_MS);
      Serial2.println("AIR,U");
      vTaskDelay(175 / portTICK_PERIOD_MS);
      Serial2.println("MOVE,-39,-92.5,0");
      vTaskDelay(650 / portTICK_PERIOD_MS);
      Serial2.println("AIR,D");  //-- d for down but it goes up
      //vTaskDelay(700 / portTICK_PERIOD_MS);
    }



  }
  
}

void echoCommand(void *parameters) {
  while(1) {
//    Serial.println(scale.get_units(10), 1);  //-- only 1 active serial print now for monitor purpose
    //-- there is a problem where sometimes a command does not send through serial monitor
    //-- check! does the origional arduino code work or have this problem?
    //-- could it be todo with context switching? Ask sen...
    if (commandReady) {
      if (String(commandPtr) == "fuck") {
//        Serial.println("fucken aye this works");
        Serial2.println("AIR,U");
      }
      if (String(commandPtr) == "DIS_CONV") {
        digitalWrite(enPin,HIGH);
      }
      if (String(commandPtr) == "EN_CONV") {
        digitalWrite(enPin, LOW);
      }
      if (String(commandPtr) == "START") {

          vTaskDelay(100 / portTICK_PERIOD_MS);
          xTaskCreatePinnedToCore(testingArmCommands,
                          "Run Program",
                          4048,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

          xTaskCreatePinnedToCore(compute_weight,
                          "mass",
                          4024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
      }
//      Serial.println(commandPtr);
      vPortFree(commandPtr);
      commandPtr = NULL;  //-- message display command ptr
      commandReady = 0; //-- reset commandReady flag
    }
  }
}


void runConv(void *parameters) {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,HIGH); //-- disabled by default, low to run
  digitalWrite(dirPin,LOW); //-- correct direction
  while(1) {
    //-- step with the microcontroller to make the belt move
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(80);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(80);
  }
  
}

void setup(void) {

  
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  LoadCell.begin();

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;

  LoadCell.start(stabilizingtime, _tare);
  
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("hi");
    while(1);
    
  }

  LoadCell.setCalFactor(1881.77); //-- 1866.75
  
  Serial.println();
  Serial.println("---Command Dump---");
  Serial.println("Enter a command:");

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
//  Serial.println("Serial2 Initialised");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  
  Serial2.print("SPEEDSET,900,750");

  xTaskCreatePinnedToCore(readSerial,
                          "Read Command",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Start Serial print task
  xTaskCreatePinnedToCore(echoCommand,
                          "Echo Command",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  xTaskCreatePinnedToCore(runConv,
                          "Run Belt",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  
  // Delete "app_main" task
  vTaskDelete(NULL);
}

void loop(void) {
  
}
