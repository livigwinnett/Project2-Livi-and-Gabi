#include "mbed.h"
#include "arm_book_lib.h"
#include "servo_motor.h"

#define DUTY_MIN 0.023
#define DUTY_MAX 0.091
#define PERIOD 0.02
PwmOut servo(PF_9);

void gateInit()
{
    servo.period(PERIOD); // 20ms period
    servo.write(DUTY_MAX);
}

 void gateOpen()
{
        servo.write(DUTY_MAX);
        delay(3000);
        servo.write(DUTY_MAX/2);
        delay(3000);
}

 void gateClose()
{
        servo.write(DUTY_MAX/2);
        delay(3000);
        servo.write(DUTY_MAX);
        delay(3000);
}
