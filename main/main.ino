
//--                Include Libraries
/* ======================================================
 
Required for using the arduino microcontroller with a RTOS
  is the Arduino_FreeRTOS library. This allows for the
  creation of tasks, and impliments the kernel onto the
  microcontroller.

The use of a RTOS is powerful for task creation and
  seperation in comparason to other porogram architectures
  such as super loops. Each group member can work upon a
  task by themselves and add it to the task scheduler.
  This allows each task to have a greater degree of
  indepednace when multiple contrubutiors are developing
  code for the same project.
*/

#include <Arduino_FreeRTOS.h>
 
/* ======================================================
*/


//--                   Pin Definitions
/* ======================================================
 
Designated area for definint pins to be used within the
  firmware. Each SCARA robot should share the same electrical
  setup so pin definitions should be unanimous accross all
  SCARA rigs.
*/

//-- Example:
//-- #define PIN_NAME PIN_NUMBER
 
/* ======================================================
*/

//--                   Task Definitions
/* ======================================================
 
Please create any tasks here. This will help with the file
  structure and keep it orginised. It may not hurt to look
  into createing a spesific file for each task and including
  them all into this document (kinda like a Swift project)
*/

void startConveyerBelt(int speed) {

  while(1) {

    //-- Every tasks will run indefinitly, thus will require a while1 loop
    
  }
  
}

void stopConveyerBelt() {
  
}
 
/* ======================================================
*/


void setup() {
  

}

void loop() {
  //-- don't put code in here, this is just going to be an empty task

}
