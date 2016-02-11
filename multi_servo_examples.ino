#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
 
// Pulse widths for the Tower Pro SG90 - determined through experimentation
#define SERVOMIN  150 
#define SERVOMAX  600
 
// Array mapping logical addresses to physical addresses
// The values depend on the routing of the wires from the servo arms
uint8_t servoaddr[] = {  0,  1,  2,  3,
                         4,  5,  6,  7,
                         8, 9, 10,  11,
                        12, 13, 14, 15  };
 
// a 4x4 grid has 7 diagonals. Servos on the same diagonal
// have the same position as eachother. I use the "counter"
//  array to keep track of the current positions.
uint8_t counter[7]   = {0, 0, 0, 0, 0, 0, 0};
 
// We also need to keep track of what direction the servos
// in each diagonal are moving: 1 or -1.
int8_t counterdir[7] = {1, 1, 1, 1, 1, 1, 1};
 
void setup() {
  pwm.begin();
  pwm.setPWMFreq(60); // Data sheet says they operate at 50Hz
}
 
void loop() {
  // Update the counters
  for(uint8_t i = 0; i < 7; i++) {
    // Move the current counter in the current direction, multiplied
    // by the diagonal number (i). This makes different diagonals move
    // at different speeds.
    counter[i] = max(0, min(255, counter[i] + counterdir[i]*(i+1)));
 
    // Change direction if we've reached a limit
    if(counter[i] == 0) {
      counterdir[i] = 1;
    } else if(counter[i] == 255) {
      counterdir[i] = -1;
    }
  }
 
  // Update the servos
  uint8_t x; // x position
  uint8_t y; // y position
  uint8_t d; // diagonal index
  uint8_t v; // value to set servo to
  for(uint8_t i = 0; i < 16; i++) {
    x = i % 4;
    y = i / 4;
    d = x + y;
    v = counter[d];
 
    // If x>1, the servo is on the other side of the center line, so
    // we need to invert its value
    if(x > 1) {
      v = 255-v;
    }
 
    // Set the current servo's value. 
    pwm.setPWM(servoaddr[i], 0, map(v, 0, 255, SERVOMIN, SERVOMAX));
  }
  delay(50);
}
