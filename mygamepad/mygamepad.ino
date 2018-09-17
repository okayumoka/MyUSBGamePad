// My GamePad

#include <Joystick.h>

#define BUTTONS_COUNT 14

#define AXIS_MIN 0
#define AXIS_MAX 2
#define AXIS_HOME 1

#define BUTTONS_PIN_UP    2  // D2
#define BUTTONS_PIN_RIGHT 3
#define BUTTONS_PIN_DOWN  4
#define BUTTONS_PIN_LEFT  5
#define BUTTONS_PIN_1     6
#define BUTTONS_PIN_2     7
#define BUTTONS_PIN_3     8
#define BUTTONS_PIN_4     9
#define BUTTONS_PIN_5     10
#define BUTTONS_PIN_6     16
#define BUTTONS_PIN_7     14
#define BUTTONS_PIN_8     15
#define BUTTONS_PIN_META  18 // A0
#define BUTTONS_PIN_TURBO 19 // A1

#define BUTTON_UP    100
#define BUTTON_RIGHT 101
#define BUTTON_DOWN  102
#define BUTTON_LEFT  103
#define BUTTON_1     0
#define BUTTON_2     1
#define BUTTON_3     2
#define BUTTON_4     3
#define BUTTON_5     4
#define BUTTON_6     5
#define BUTTON_7     6
#define BUTTON_8     7
#define BUTTON_META  1000
#define BUTTON_TURBO 1001

const int buttonPins[] = {
    BUTTONS_PIN_UP    ,
    BUTTONS_PIN_RIGHT ,
    BUTTONS_PIN_DOWN  ,
    BUTTONS_PIN_LEFT  ,
    BUTTONS_PIN_1     ,
    BUTTONS_PIN_2     ,
    BUTTONS_PIN_3     ,
    BUTTONS_PIN_4     ,
    BUTTONS_PIN_5     ,
    BUTTONS_PIN_6     ,
    BUTTONS_PIN_7     ,
    BUTTONS_PIN_8     ,
    BUTTONS_PIN_META  ,
    BUTTONS_PIN_TURBO
};

const int buttonsCode[] = {
    BUTTON_UP    ,
    BUTTON_RIGHT ,
    BUTTON_DOWN  ,
    BUTTON_LEFT  ,
    BUTTON_1     ,
    BUTTON_2     ,
    BUTTON_3     ,
    BUTTON_4     ,
    BUTTON_5     ,
    BUTTON_6     ,
    BUTTON_7     ,
    BUTTON_8     ,
    BUTTON_META  ,
    BUTTON_TURBO
};

// Joystick_ Joystick;
Joystick_ Joystick = Joystick_(
    0x03,                    // reportid
    JOYSTICK_TYPE_GAMEPAD,   // type
    BUTTONS_COUNT,           // button count
    0,                       // hat switch count
    true,                    // x axis enable
    true,                    // y axis enable
    false,                   // z axis enable
    false,                   // right x axis enable
    false,                   // right y axis enable
    false,                   // right z axis enable
    false,                   // rudder enable
    false,                   // throttle enable
    false,                   // accelerator enable
    false,                   // brake enable
    false                    // steering enable
);

int currentState[BUTTONS_COUNT];
int latestState[BUTTONS_COUNT];
bool turboState[BUTTONS_COUNT];
bool turboPressState[BUTTONS_COUNT];

void setup() {
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
        currentState[i] = HIGH;
        latestState[i] = HIGH;
        turboState[i] = false;
        turboPressState[i] = false;
    }

    Joystick.begin(true);
    Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
    Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);
    Joystick.setXAxis(AXIS_HOME);
    Joystick.setYAxis(AXIS_HOME);

    Serial.begin(9600);
}

void loop() {
    bool isTurboMode = false;
    bool isMetaMode = false;

    // get current state
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        currentState[i] = digitalRead(buttonPins[i]);
        if (buttonPins[i] == BUTTONS_PIN_TURBO) {
            isTurboMode = currentState[i] == LOW;
        } else if (buttonPins[i] == BUTTONS_PIN_META) {
            isMetaMode = currentState[i] == LOW;
        }
    }

    // press or release
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        int button = buttonsCode[i];

        if (currentState[i] != latestState[i]) {
            // press or release
            if (currentState[i] == LOW) {
                if (isTurboMode) {
                    // toggle turbo mode.
                    Serial.print("SET TURBO ");
                    Serial.println(button);
                    setTurbo(button, i);
                } else {
                    // press button
                    Serial.print("PRESS ");
                    Serial.println(button);
                    setButton(button, true);
                }
            } else {
                // release button
                Serial.print("RELEASE ");
                Serial.println(button);
                setButton(button, false);
            }
            latestState[i] = currentState[i];
        } else if (currentState[i] == LOW && turboState[i]) {
            // turbo mode
            if (turboPressState[i]) {
                Serial.print("RELEASE by TURBO ");
                Serial.println(button);
                setButton(button, false);
            } else {
                Serial.print("PRESS by TURBO ");
                Serial.println(button);
                setButton(button, true);
            }
            turboPressState[i] = !turboPressState[i];
        }
    }
    delay(1);
}

void setButton(int button, bool isPressed) {
    switch (button) {
        case BUTTON_UP:
            if (isPressed) Joystick.setXAxis(AXIS_MAX);
            else Joystick.setXAxis(AXIS_HOME);
            break;
        case BUTTON_DOWN:
            if (isPressed) Joystick.setXAxis(AXIS_MIN);
            else Joystick.setXAxis(AXIS_HOME);
            break;
        case BUTTON_RIGHT:
            if (isPressed) Joystick.setYAxis(AXIS_MAX);
            else Joystick.setYAxis(AXIS_HOME);
            break;
        case BUTTON_LEFT:
            if (isPressed) Joystick.setYAxis(AXIS_MIN);
            else Joystick.setYAxis(AXIS_HOME);
            break;
        case BUTTON_META:
        case BUTTON_TURBO:
            // do nothing.
            break;
        default:
            if (isPressed) Joystick.pressButton(button);
            else Joystick.releaseButton(button);
            break;
    }
}

void setTurbo(int button, int index) {
    if (button == BUTTON_META || button == BUTTON_TURBO) return;
    turboState[index] = !turboState[index];
    if (turboState[index]) {
        turboPressState[index] = false;
    }
}


