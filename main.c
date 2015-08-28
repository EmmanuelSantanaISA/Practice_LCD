/* 
 * File:   main.c
 * Author: emmanuelsantana
 *
 * Created on August 12, 2015, 4:13 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>

#include "LCDDisplay.h"

#define MENUBTN 10
#define UPBTN 29
#define DOWNBTN 31
#define OKBTN 26

unsigned short ROWS = 2;
unsigned short COLS = 16;
unsigned short RS = 24;
unsigned short E = 22;
unsigned short D4 = 3;
unsigned short D5 = 5;
unsigned short D6 = 7;
unsigned short D7 = 8;
unsigned char bNextMenuState;
unsigned char bCurrentState;
int lcd;
int variableNumber;
int variableNumberSnapshot = 0;
unsigned short variableNumberSnapshotActive = 0;
unsigned short okState;
char *optionsArray[3];
unsigned char selectedOption = 0;
char variableString[] = "Emmanuel";
unsigned short cursorPostion = 0;
unsigned char currentChar;
char *variableStringCopy[16];

typedef enum {
    STATE0 = 0,
    STATE1,
    STATE2,
    STATE3,
    STATE4,
    STATE5,
    STATE6,
    MAX_STATE
} eStates;


void vfnState0(void);
void vfnState1(void);
void vfnState2(void);
void vfnState3(void);
void vfnState4(void);
void vfnState5(void);
void vfnState6(void);
void initLCD();
void changeLetter(char *string, int postition, char letter);

void (* const vfnapStateMachineFuncPtr[]) (void) = {
    vfnState0,
    vfnState1,
    vfnState2,
    vfnState3,
    vfnState4,
    vfnState5,
    vfnState6
};

typedef struct {
    unsigned short pin;
    unsigned short state;
    unsigned short changed;
    unsigned short isPressed;
} sButton;

int checkButtonState(sButton button);

sButton menuButton;
sButton upButton;
sButton downButton;
sButton okButton;

void initButtons(void) {
    menuButton.changed = 0;
    menuButton.state = 0;
    menuButton.pin = MENUBTN; // Physycal 10

    upButton.changed = 0;
    upButton.state = 0;
    upButton.pin = UPBTN; // Physycal 11

    downButton.changed = 0;
    downButton.state = 0;
    downButton.pin = DOWNBTN; // Physycal 12

    okButton.changed = 0;
    okButton.state = 0;
    okButton.pin = OKBTN; // Physycal 13
    return;
}

int main(int argc, char** argv) {
    bNextMenuState = STATE0;
    initLCD();
    initButtons();
    optionsArray[0] = "Option 1";
    optionsArray[1] = "Option 2";
    optionsArray[2] = "Option 3";
    vfnapStateMachineFuncPtr[0]();
    for (;;) {
        menuButton.state = !digitalRead(menuButton.pin);
        if (menuButton.state == 0) {
            if (menuButton.isPressed) {
                menuButton.isPressed = 0;
            }
        }
        if (menuButton.state == 1 && menuButton.isPressed == 0) {
            menuButton.isPressed = 1;
            printf("MENU button pressed\n\r");
            switch (bCurrentState) {
                case 0:
                    vfnapStateMachineFuncPtr[STATE1]();
                    break;
                case 1:
                    vfnapStateMachineFuncPtr[STATE2]();
                    break;
                case 2:
                    vfnapStateMachineFuncPtr[STATE3]();
                    break;
                case 3:
                    vfnapStateMachineFuncPtr[STATE0]();
                    break;
                case 4:
                    variableNumberSnapshotActive = 0;
                    variableNumber = variableNumberSnapshot;
                    printf("Current state: %d.\n\r", bCurrentState);
                    printf("Next state: %d.\n\r\n\r", bNextMenuState);
                    printf("Number: %d.\n\r\n\r", variableNumber);
                    printf("Snapshot number: %d.\n\r\n\r", variableNumberSnapshot);
                    printf("Snapshot flag: %d.\n\r\n\r", variableNumberSnapshotActive);
                    vfnapStateMachineFuncPtr[STATE1]();
                    break;
                case 5:
                    if (variableString == '\0') {
                        int i = 0;
                        for (i = 0; i < 15; i++) {
                            cursorPostion = 0;
                            //variableString--;
                        }
                    } else {
                        cursorPostion++;
                        //variableString = cursorPostion;
                    }
                    lcdPosition(lcd, cursorPostion, 1);
                    break;
                case 6:
                    break;
                default:
                    break;
            }
            // vfnapStateMachineFuncPtr[bNextMenuState]();
            printf("Menu button pressed\n\r");
        }

        upButton.state = !digitalRead(upButton.pin);
        if (upButton.state == 0) {
            if (upButton.isPressed) {
                upButton.isPressed = 0;
            }
        }
        if (upButton.state == 1 && upButton.isPressed == 0) {
            upButton.isPressed = 1;
            printf("UP button pressed, current state: %d, numero: %d\n\r", bCurrentState, variableNumber);
            switch (bCurrentState) {
                case 0:

                    break;
                case 1:
                    printf("OK button pressed, going to state: %d\n\r", STATE4);
                    vfnapStateMachineFuncPtr[STATE4]();
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    if (variableNumber < 255) {
                        variableNumber++;
                        vfnapStateMachineFuncPtr[4]();
                    }
                    break;
                case 5:
                    currentChar = variableString[cursorPostion];
                    printf("Current Char: %c\n\r", currentChar);
                    lcdPosition(lcd, cursorPostion, 1);
                    char newLetter = ++currentChar;
                    printf("New Char: %c\n\r", newLetter);
                    lcdPutchar(lcd, newLetter);
                    lcdPosition(lcd, cursorPostion, 1);
                    changeLetter(variableString, cursorPostion, newLetter);
                    break;
                case 6:
                    if (selectedOption < 2) {
                        selectedOption++;
                        vfnapStateMachineFuncPtr[STATE6]();
                    }

                    break;
                default:
                    break;
            }
        }

        downButton.state = !digitalRead(downButton.pin);
        if (downButton.state == 0) {
            if (downButton.isPressed) {
                downButton.isPressed = 0;
            }
        }
        if (downButton.state == 1 && downButton.isPressed == 0) {
            downButton.isPressed = 1;
            printf("Down button pressed, current state: %d, numero: %d\n\r", bCurrentState, variableNumber);
            switch (bCurrentState) {
                case 0:
                    break;
                case 1:
                    printf("OK button pressed, going to state: %d\n\r", STATE4);
                    vfnapStateMachineFuncPtr[STATE4]();
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    if (variableNumber > 0) {
                        variableNumber--;
                        vfnapStateMachineFuncPtr[4]();
                    }
                    break;
                case 5:
                    currentChar = variableString[cursorPostion];
                    printf("Current Char: %c\n\r", currentChar);
                    lcdPosition(lcd, cursorPostion, 1);
                    char newLetter = --currentChar;
                    lcdPutchar(lcd, newLetter);
                    lcdPosition(lcd, cursorPostion, 1);
                    changeLetter(variableString, cursorPostion, newLetter);
                    break;
                case 6:
                    if (selectedOption > 0) {
                        selectedOption--;
                        vfnapStateMachineFuncPtr[STATE6]();
                    }

                default:
                    break;
            }
        }

        okButton.state = !digitalRead(okButton.pin);
        if (okButton.state == 0) {
            if (okButton.isPressed) {
                okButton.isPressed = 0;
            }
        }
        if (okButton.state == 1 && okButton.isPressed == 0) {
            okButton.isPressed = 1;
            switch (bCurrentState) {
                case 0:
                    break;
                case 1:
                    vfnapStateMachineFuncPtr[STATE4]();
                    break;
                case 2:
                    vfnapStateMachineFuncPtr[STATE5]();
                    break;
                case 3:
                    vfnapStateMachineFuncPtr[STATE6]();
                    break;
                case 4:
                    vfnapStateMachineFuncPtr[STATE1]();

                    break;
                case 5:
                    lcdCursorBlink(lcd, 0);
                    lcdPosition(lcd, 0, 0);
                    cursorPostion = 0;
                    vfnapStateMachineFuncPtr[STATE2]();
                    break;
                case 6:
                    vfnapStateMachineFuncPtr[STATE3]();
                    break;
                default:
                    printf("opcion invalida OK");
                    break;
            }
        }
    }
    return (EXIT_SUCCESS);
}

void initLCD() {
    printf("Iniciando...\n\r");
    wiringPiSetupPhys();

    lcd = lcdInit(ROWS, COLS, 4, RS, E, D4, D5, D6, D7, 0, 0, 0, 0);
    printf("Status: %d\n\r", lcd);
    if (lcd == -1) {
        printf("Fallo la inicializacion! \n\r");
    } else {
        printf("Iniciado correctamente! \n\r");
    }
    //    lcdPosition(lcd, 0, 0);
    //    lcdPuts(lcd, "Hola");
    pinMode(menuButton.pin, INPUT);
    pinMode(upButton.pin, INPUT);
    pinMode(downButton.pin, INPUT);
    pinMode(okButton.pin, INPUT);
}

//int checkButtonState(sButton button) {
//    //printf("Entro");
//    unsigned short portb_snapshot = 1;
//    unsigned short pressed = 0;
//    // This is for the XXXXX input
//    // ------------------------------------------------------
//    if (button.changed == 1) {
//        if (!((portb_snapshot ^ button.state) & !digitalRead(button.pin))) {
//            // If the line was changed last time, and it is the same state as last
//            // time, then we need to lock it in here (If the bits are not the same then this routine
//            // will be called again and the correct value will be locked in)
//            if (portb_snapshot & !digitalRead(button.pin)) {
//                // Do this when the line goes high
//                pressed = 1;
//            } else {
//                // Do this when the line goes low
//            }
//            // Clear the changed flag
//            button.changed = 0;
//        }
//    }
//    // Mask out any changed input pins
//    button.changed = ((button.state ^ (portb_snapshot & !digitalRead(button.pin))) > 0); // XOR with last last_state to find changed pins
//    button.state = portb_snapshot & !digitalRead(button.pin);
//    printf("Snap: %d Read: %d BOTON: %d.\n\r", portb_snapshot, !digitalRead(button.pin), button.pin);
//    //printf("PRESEEED%d",!digitalRead(button.pin));
//    return pressed;
//}

void vfnState0(void) {
    lcdClear(lcd);
    bCurrentState = STATE0;
    printf("Current state: %d.\n\r", bCurrentState);
    bNextMenuState = STATE1;
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Welcome!");
}

void vfnState1(void) {
    lcdClear(lcd);
    bCurrentState = STATE1;
    printf("Current state: %d.\n\r", bCurrentState);
    bNextMenuState = STATE2;
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Change number");
}

void vfnState2(void) {
    lcdClear(lcd);
    bCurrentState = STATE2;
    printf("Current state: %d.\n\r", bCurrentState);
    bNextMenuState = STATE3;
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Adjust string");
}

void vfnState3(void) {
    lcdClear(lcd);
    bCurrentState = STATE3;
    printf("Current state: %d.\n\r", bCurrentState);
    bNextMenuState = STATE0;
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Select");
    lcdPosition(lcd, 0, 1);
    lcdPuts(lcd, "option");
}

void vfnState4(void) {
    char *p, text[32];
    int i = variableNumber;
    sprintf(text, "%d", i);
    p = text;
    bCurrentState = STATE4;
    printf("Current state: %d.\n\r", bCurrentState);
    if (variableNumberSnapshotActive == 0) {
        variableNumberSnapshot = variableNumber;
        variableNumberSnapshotActive = 1;
    }
    lcdClear(lcd);
    bCurrentState = STATE4;
    printf("Current state: %d.\n\r", bCurrentState);
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    printf("Number: %d.\n\r\n\r", variableNumber);
    printf("Snapshot number: %d.\n\r\n\r", variableNumberSnapshot);
    printf("Snapshot flag: %d.\n\r\n\r", variableNumberSnapshotActive);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Value:");
    lcdPosition(lcd, 0, 1);
    lcdPuts(lcd, p);
}

void vfnState5(void) {
    lcdCursorBlink(lcd, 1);
    lcdClear(lcd);
    bCurrentState = STATE5;
    printf("Current state: %d.\n\r", bCurrentState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Current string:");
    lcdPosition(lcd, 0, 1);
    lcdPuts(lcd, variableString);
    lcdPosition(lcd, 0, 1);
}

void vfnState6(void) {
    lcdClear(lcd);
    bCurrentState = STATE6;
    printf("Current state: %d.\n\r", bCurrentState);
    printf("Next state: %d.\n\r\n\r", bNextMenuState);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, "Selected option:");
    lcdPosition(lcd, 0, 1);
    printf("Selected Option: %d\n\r", selectedOption);
    lcdPuts(lcd, optionsArray[selectedOption]);
}

void changeLetter(char *string, int postition, char letter) {
    *string = letter;
    printf("Cadena: %s\n\r", string);
}