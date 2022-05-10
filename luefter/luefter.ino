#define BOARD "MEGA"

#define LUEFTER_COUNT 1
#define LUEFTER_STARTPIN 2
#define DEFAULT_LUEFTER_SPEED 128

#define READY 202
#define SUCCESS 200
#define NOT_FOUND 404
#define FORBIDDEN 403
#define INTERNAL_ERROR 500

#define IGNORE_CODE 666

int luefter_pins[LUEFTER_COUNT];
int luefter_speeds[LUEFTER_COUNT];

void setup() {
    Serial.begin(9600);

    {
        // if it doesnt works just use uno
        if (BOARD == "UNO") 
        {
            TCCR1B = TCCR1B & B11111000 | B00000001;
        }
        else if (BOARD == "MEGA") 
        {
            TCCR0B = TCCR0B & B11111000 | B00000001;
        }
        
    }

    for (int i = 0; i < LUEFTER_COUNT; i++) {
        luefter_pins[i] = LUEFTER_STARTPIN + i;
        luefter_speeds[i] = DEFAULT_LUEFTER_SPEED;
        analogWrite(luefter_pins[i], luefter_speeds[i]);
    }

    Serial.print(READY);
}

void loop() {
    if(Serial.available() > 0) {
        String input = Serial.readString();

        byte adress = input[0];

        switch (adress)
        {
        case 0x00:
            // luefter
            int i = (int) input[1];
            luefter_speeds[i] = (int) input[2];
            analogWrite(luefter_pins[i], luefter_speeds[i]);
            Serial.print(SUCCESS);
            break;
        
        default:
            Serial.print(NOT_FOUND);
            break;
        }

    }

    for (int i = 0; i < LUEFTER_COUNT; i++) {
        analogWrite(luefter_pins[i], luefter_speeds[i]);
    }
}
