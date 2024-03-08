#include <HardwareSerial.h>
#include <Arduino.h>

#define RXD2 16
#define TXD2 17

//-- stepper motor conveyor definitions
#define stepPin 4 //pulse
#define dirPin 2
#define enPin 15

//-- FreeRTOS for ESP32 using single processor core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

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

void echoCommand(void *parameters) {
  while(1) {

    //-- there is a problem where sometimes a command does not send through serial monitor
    //-- check! does the origional arduino code work or have this problem?
    //-- could it be todo with context switching? Ask sen...
    if (commandReady) {
      if (String(commandPtr) == "fuck") {
        Serial.println("fucken aye this works");
        Serial2.print("HOME\n");
      }
      if (String(commandPtr) == "DIS_CONV") {
        digitalWrite(enPin,HIGH);
      }
      if (String(commandPtr) == "EN_CONV") {
        digitalWrite(enPin, LOW);
      }
      Serial.println(commandPtr);
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
    delayMicroseconds(40);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(40);
  }
  
}

void setup(void) {

  
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---Command Dump---");
  Serial.println("Enter a command:");

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial2 Initialised");

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
