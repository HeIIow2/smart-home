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

// GRIPPER CONSTANTS
// delay between the movement of servos 10-30ms
#define STEP_DELAY 10
#define MOTOR_COUNT 2

int pos[6] = {90, 90, 90, 90, 90, 42};

// RESPONSE CODES
#define STARTED 666
#define SUCCES 202
#define OUT_OF_BOUNDS 403
#define NOT_FOUND 404

void setup()
{
  Braccio.begin();
  Serial.begin(9600);
  // Braccio.ServoMovement(10, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
  Serial.println(STARTED);
}

int set_pos(int index, int value)
{
  /*
     Setting the value in the position array at index "index" to value "value"
     If the value goes out of the set bounds in the certain index
     it is set to the according max/min value, which is allowed.
  */

  const int max_min[6][2] = {{0, 180}, {15, 165}, {0, 180}, {0, 180}, {0, 180}, {10, 73}};
  pos[index] = value;

  // check for min bounds
  if (pos[index] < max_min[index][0])
  {
    pos[index] = max_min[index][0];

    return OUT_OF_BOUNDS;

    Serial.print("to small: index ");
    Serial.print(index);
    Serial.print(", value: ");
    Serial.println(value);
  }

  // check for max bounds
  if (pos[index] > max_min[index][1])
  {
    pos[index] = max_min[index][1];

    return OUT_OF_BOUNDS;

    Serial.print("to big: index ");
    Serial.print(index);
    Serial.print(", value: ");
    Serial.println(value);
  }

  return SUCCES;
}

void commit_pos()
{
  /*
     After changing the values in the array "pos" to the desired values
     this function should be called to move the robot to given position.

     Basicly it is just a wrapper for "Braccio.ServoMovement" with less typing
     and less posibility to fuck up (still possible though).
  */
  Braccio.ServoMovement(STEP_DELAY, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);

}


int move_abs(int positions[MOTOR_COUNT])
{
  /*
     Moves the robot to an absolute position
  */
  int response = SUCCES;

  for (int i = 0; i < MOTOR_COUNT; i++) {
    int r = set_pos(i, positions[i]);

    if (r != SUCCES) {
      response = r;
    }
  }

  commit_pos();

  return response;
}


int move_rel(int positions[MOTOR_COUNT])
{
  /*
     Moves the robot to a relative position
  */
  int response = SUCCES;

  for (int i = 0; i < MOTOR_COUNT; i++) {
    int r = set_pos(i, pos[i] + positions[i]);

    if (r != SUCCES) {
      response = r;
    }
  }

  commit_pos();
  return response;
}


// Wrapper which read the Serial input
int move_abs_wrapper() {
  int abs_pos[MOTOR_COUNT];
  for (int i = 0; i < MOTOR_COUNT; i++) {
    // wait till data arrives
    while (Serial.available() <= 0) {}
    abs_pos[i] = (int) Serial.read();
  }

  return move_abs(abs_pos);
}

int move_rel_wrapper() {
  int rel_pos[MOTOR_COUNT];
  for (int i = 0; i < MOTOR_COUNT; i++) {
    int factor;
    // wait till data arrives
    while (Serial.available() <= 0) {}
    byte mode = Serial.read();
    if (mode == 0x00) {
      factor = -1;
    } else if (mode == 0xff) {
      factor = 1;
    }
    // wait till data arrives
    while (Serial.available() <= 0) {}
    rel_pos[i] = factor * (int) Serial.read();
  }

  return move_rel(rel_pos);
}

// COMMANDS
int exec_set() {
  // wait till data arrives
  while (Serial.available() <= 0) {}

  byte command = Serial.read();

  if (command == 0x01) {
    // move absolute
    return move_abs_wrapper();
  }

  if (command == 0x02) {
    // move relative
    return move_rel_wrapper();
  }

  return NOT_FOUND;
}

int exec_get() {
  return NOT_FOUND;
}

int execute() {
  byte mode = Serial.read();

  if (mode == 0x00) {
    return exec_set();
  }

  if (mode == 0xff) {
    return exec_get();
  }

  return NOT_FOUND;
}

void loop()
{
  if (Serial.available() > 0) {
    Serial.println(execute());
  }
}
