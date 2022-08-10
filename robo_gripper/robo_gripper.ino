#include <Braccio.h>
#include <Servo.h>

#define MOTOR_COUNT 6
#define VERTICAL_COUNT 3

#define SUCCESS 200
#define NOT_FOUND 404
#define FORBIDDEN 403
#define INTERNAL_ERROR 500

#define IGNORE_CODE 666

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

int pos[MOTOR_COUNT] = {60,  15, 180, 90, 0,  73};
int max_min[MOTOR_COUNT][2] = {{0, 180}, {15, 165}, {0, 180}, {0, 180}, {0, 180}, {10, 73}};
int vertical_Motors[VERTICAL_COUNT] = {1, 2, 3};

byte wrist_angle = 90;
byte height = 0;


int goToAngel(int servo, int angel) {
  if (servo >= MOTOR_COUNT || servo < 0) return NOT_FOUND;

  if (angel < max_min[servo][0])
  {
    pos[servo] = max_min[servo][0];
    return FORBIDDEN;
  }

  if (angel > max_min[servo][1])
  {
    pos[servo] = max_min[servo][1];
    return FORBIDDEN;
  }

  pos[servo] = angel;
  return SUCCESS;
}


int vertical(int step_) {
  if (step_ == 0) return SUCCESS;

  int ranges[VERTICAL_COUNT];
  byte winkelsumme = 195;
  int error_code = SUCCESS;

  int total_range = 0;
  bool dir = step_ > 0;

  for (byte i = 0; i < VERTICAL_COUNT; i++) {
    byte index = vertical_Motors[i];

    // if step is bigger than 0 get the upper bound else the lower one
    int bound = max_min[index][(int) dir];

    // compute range of this motor
    ranges[i] = bound - pos[index];

    // add the range to total
    total_range += ranges[i];
  }

  float factor = (float) step_ / (float) total_range;
  if (factor > 1.0) {
    factor = 1.0;
    error_code = FORBIDDEN;
  }

  for (byte i = 0; i < VERTICAL_COUNT; i++) {
    byte index = vertical_Motors[i];
    pos[index] += (int) (factor * (float) step_);
  }

  for (int i = 0; i < 6; i++) {
    if (pos[i] < max_min[i][0]) {
      pos[i] = max_min[i][0];
      error_code = INTERNAL_ERROR;
    } else if (pos[i] > max_min[i][1]) {
      pos[i] = max_min[i][1];
      error_code = INTERNAL_ERROR;
    }
  }

  return error_code;
}

int up(byte step_) {
  return vertical((int)step_);
}

int down(byte step_) {
  return vertical(-((int)step_));
}

int get_position() {
  Serial.print(SUCCESS);
  Serial.print((char) MOTOR_COUNT);

  for (int i = 0; i < MOTOR_COUNT; i++)
  {
    Serial.print((char) pos[i]);
  }
}

int set_height(byte height_) {
  int error_code = SUCCESS;

  if (height_ <= 150 && height_ >= 0) {
    height = height_;
  }
  else {
    error_code = FORBIDDEN;
    if (height_ < 0) height = 0;
    else height = 150;
  }

  pos[1] = height+15;
  pos[2] = 180-height;
  pos[3] = wrist_angle;

  return error_code;
}

int set_angle(byte angle_) {
  // total sum = 285
  // m1 = angle
  // m2 = 180 - angle
  // m3 = 285 - (angle + 180 - angle)
  int error_code = SUCCESS;

  if (angle_ <= 0 && angle_ >= 180) {
    wrist_angle = angle_;
  }
  else {
    error_code = FORBIDDEN;
    if (angle_ < 0) wrist_angle = 0;
    else wrist_angle = 180;

  }
  
  wrist_angle = angle_;
  if (set_height(height) != SUCCESS) error_code = INTERNAL_ERROR;

  return error_code;
}

int execute(String input) {
  if (input[0] == 'u') return down(input[1]);
  if (input[0] == 'd') return down(input[1]);
  if (input[0] == 's') return goToAngel(input[1], input[2]);
  if (input[0] == 'g') {get_position(); return IGNORE_CODE;}
  if (input[0] == 'h') return set_height(input[1]);
  if (input[0] == 'a') return set_angle(input[1]);

  if (input[0] == 'p')
  {
    int error_code = 202;
    for (int i = 0; i < MOTOR_COUNT; i++) {
      int current_error_code = goToAngel(i, input[i + 1]);
      if (current_error_code != 202) {
        error_code = current_error_code;
      }
    }
    return error_code;
  }
}


void setup() {
  Braccio.begin();
  Serial.begin(9600);
  Braccio.ServoMovement(20, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
  Serial.print("202");
}

void loop() {
  if (Serial.available() > 0) {
    int error_code = execute(Serial.readString());
    if (IGNORE_CODE != error_code) {
      Serial.print(error_code);
    }

    for (int i = 0; i < 6; i++) {
      if (pos[i] < max_min[i][0]) {
        pos[i] = max_min[i][0];
        error_code = INTERNAL_ERROR;
      } else if (pos[i] > max_min[i][1]) {
        pos[i] = max_min[i][1];
        error_code = INTERNAL_ERROR;
      }
    }
    Braccio.ServoMovement(10, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
  }

  /*
    Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
    M1=base degrees. Allowed values from 0 to 180 degrees
    M2=shoulder degrees. Allowed values from 15 to 165 degrees
    M3=elbow degrees. Allowed values from 0 to 180 degrees
    M4=wrist vertical degrees. Allowed values from 0 to 180 degrees
    M5=wrist rotation degrees. Allowed values from 0 to 180 degrees
    M6=gripper degrees. Allowed values from 10 to 73 degrees. 10: the toungue is open, 73: the gripper is closed.
  */
}
