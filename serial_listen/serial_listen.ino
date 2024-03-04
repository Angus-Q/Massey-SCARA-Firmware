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
  parameters = NULL; //-- function does not accept parameters

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
  parameters = NULL;
  while(1) {
    if (commandReady) {
      Serial.println(commandPtr);
      vPortFree(commandPtr);
      commandPtr = NULL;  //-- message display command ptr
      commandReady = 0; //-- reset commandReady flag
    }
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---Command Dump---");
  Serial.println("Enter a command:");

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
  
  // Delete "setup and loop" task
  vTaskDelete(NULL);
  
}

void loop() {
  // Execution should never get here, as the task is deleted in the setup task
}
