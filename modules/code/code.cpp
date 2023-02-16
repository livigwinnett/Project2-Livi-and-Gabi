//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "code.h"

#include "servo_motor.h"
#include "user_interface.h"
#include "pc_serial_com.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"
#include "display.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

extern char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
extern char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static int numberOfIncorrectCodesAlarm = 0;
static int numberOfIncorrectCodesGate = 0;
static char codeSequenceAlarm[CODE_NUMBER_OF_KEYS] = { '1', '8', '0', '5' };
static char codeSequenceGate[CODE_NUMBER_OF_KEYS] = { '1', '1', '1', '1' };

//=====[Declarations (prototypes) of private functions]========================

static bool codeMatchAlarm( char* codeToCompare );
static bool codeMatchGate( char* codeToCompare );
static void codeDeactivateAlarm();

//=====[Implementations of public functions]===================================

void codeWrite( char* newCodeSequence )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        codeSequenceAlarm[i] = newCodeSequence[i];
    }
}

bool codeMatchFrom( codeOrigin_t codeOrigin )
{
    bool codeIsCorrect = false;
    switch (codeOrigin) {
        case CODE_KEYPAD:
            if( userInterfaceCodeCompleteRead() ) {
                codeIsCorrect = codeMatchGate(codeSequenceFromUserInterface);
                userInterfaceCodeCompleteWrite(false);
                if(numberOfIncorrectCodesGate >= 3){
                    displayCharPositionWrite (0,1);
                    displayStringWrite ("Locked out!");
                }
                else if ( codeIsCorrect && numberOfIncorrectCodesGate < 3) {
                    displayCharPositionWrite (0,1);
                    displayStringWrite ("Correct Code!");
                    gateOpen();
                    gateClose();
                    resetScreen();
                    numberOfIncorrectCodesGate = 0;
                } 
                else{
                    displayCharPositionWrite (0,1);
                    displayStringWrite ("Incorrect Code!");                    
                    numberOfIncorrectCodesGate++;
                    delay(3000);
                    resetScreen();
                }
                
                
            }

        break;
        case CODE_PC_SERIAL:
            if( pcSerialComCodeCompleteRead() ) {
                codeIsCorrect = codeMatchAlarm(codeSequenceFromPcSerialCom);
                pcSerialComCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivateAlarm();
                    pcSerialComStringWrite( "\r\nThe code is correct\r\n\r\n" );
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodesAlarm++;
                    pcSerialComStringWrite( "\r\nThe code is incorrect\r\n\r\n" );
                }
            }

        break;
        default:
        break;
    }

    if ( numberOfIncorrectCodesAlarm >= 3 ) {
        systemBlockedStateWrite(ON);
    }

    return codeIsCorrect;
}

//=====[Implementations of private functions]==================================

static bool codeMatchAlarm( char* codeToCompare )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequenceAlarm[i] != codeToCompare[i] ) {
            return false;
        }
    }
    return true;
}

static bool codeMatchGate( char* codeToCompare )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequenceGate[i] != codeToCompare[i] ) {
            return false;
        }
    }
    return true;
}

static void codeDeactivateAlarm()
{
    systemBlockedStateWrite(OFF);
    incorrectCodeStateWrite(OFF);
    numberOfIncorrectCodesAlarm = 0;
}
