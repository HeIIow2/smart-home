#include <Braccio.h>
#include <Servo.h>

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

/*
  Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
  M1=base degrees. Allowed values from 0 to 180 degrees
  M2=shoulder degrees. Allowed values from 15 to 165 degrees
  M3=elbow degrees. Allowed values from 0 to 180 degrees
  M4=wrist vertical degrees. Allowed values from 0 to 180 degrees
  M5=wrist rotation degrees. Allowed values from 0 to 180 degrees
  M6=gripper degrees. Allowed values from 10 to 73 degrees. 10: the toungue is open, 73: the gripper is closed.
*/

#define MOTOR_COUNT 2

int pos[6] = {90, 90, 90, 90, 90, 42};

// delay between the movement of servos 10-30ms
#define STEP_DELAY 10

void setup()
{
  Braccio.begin();
  Serial.begin(9600);
  // Braccio.ServoMovement(10, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
  Serial.print("202");

  int move_to[MOTOR_COUNT] = {0, 0};
  move_abs(move_to);
}

void set_pos(int index, int value) 
{
  /*
   * Setting the value in the position array at index "index" to value "value"
   * If the value goes out of the set bounds in the certain index
   * it is set to the according max/min value, which is allowed.
   */
  
  const int max_min[6][2] = {{0, 180}, {15, 165}, {0, 180}, {0, 180}, {0, 180}, {10, 73}};
  pos[index] = value;

  // check for min bounds
  if (pos[index] < max_min[index][0])
  {
    pos[index] = max_min[index][0];
    
    Serial.print("to small: index ");
    Serial.print(index);
    Serial.print(", value: ");
    Serial.println(value);
  }
  else if (pos[index] > max_min[index][1])
  {
    pos[index] = max_min[index][1];

    Serial.print("to big: index ");
    Serial.print(index);
    Serial.print(", value: ");
    Serial.println(value);
  }
  else {
    Serial.println("Success");
  }
}

void commit_pos() 
{
  /*
   * After changing the values in the array "pos" to the desired values
   * this function should be called to move the robot to given position.
   * 
   * Basicly it is just a wrapper for "Braccio.ServoMovement" with less typing
   * and less posibility to fuck up (still possible though).
   */
   Braccio.ServoMovement(STEP_DELAY, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);

}


void move_abs(int positions[MOTOR_COUNT])
{
  /*
   * Moves the robot to an absolute position
   */
  
  for (int i=0; i<MOTOR_COUNT; i++) {
    set_pos(i, positions[i]);
  }
  
  commit_pos();
}


void move_rel(int positions[MOTOR_COUNT]) 
{
  /*
   * Moves the robot to a relative position
   */
   
  for (int i=0; i<MOTOR_COUNT; i++) {
    set_pos(i, pos[i] + positions[i]);
  }
  
  commit_pos();
}

void loop()
{
  int change[MOTOR_COUNT] = {10, 10};
  for (int i=0; i<18; i++) {
    move_rel(change);
    delay(100);
  }

  int change2[MOTOR_COUNT] = {-10, -10};
  for (int i=0; i<18; i++) {
    move_rel(change2);
    delay(100);
  }
}
