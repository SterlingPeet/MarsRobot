#include <Servo.h>
#include <Romi32U4.h>
#include <PololuRPiSlave.h>

/* This example is a lightly edited version of Pololu's Romi Raspberry
 * Pi Remote Control sketch.  For our purposes, we treat is as a black
 * box sub-system that we can control from a Raspberry Pi over I2C.
 *
 * The example and libraries are available for download at:
 *
 * https://github.com/pololu/pololu-rpi-slave-arduino-library
 */

// Custom data structure that we will use for interpreting the buffer.
// This format has been adjusted from the original format to place the
// primary readback data at the beginning and the write area at the end.
// This supports a shorter single read call for regular telemetry.

struct Data
{
    // addr 0
    int16_t leftEncoder, rightEncoder;
    // addr 4
    uint16_t batteryMillivolts;
    // addr 6
    uint16_t analog[6];
    // addr 18
    bool buttonA, buttonB, buttonC;
    // addr 21
    bool yellow, green, red;
    // addr 24
    int16_t leftMotor, rightMotor;
    // addr 28
    bool playNotes;
    char notes[14];
    // addr
};

PololuRPiSlave<struct Data, 5> slave;
PololuBuzzer                   buzzer;
Romi32U4Motors                 motors;
Romi32U4ButtonA                buttonA;
Romi32U4ButtonB                buttonB;
Romi32U4ButtonC                buttonC;
Romi32U4Encoders               encoders;

void setup()
{
    // Set up the slave at I2C address 20 (or 0x14 hex).
    slave.init(20);

    // Play startup sound.
    buzzer.play("v10>>g16>>>c16");
}

void loop()
{
    // Call updateBuffer() before using the buffer, to get the latest
    // data including recent master writes.
    slave.updateBuffer();

    // Write various values into the data structure.
    slave.buffer.buttonA = buttonA.isPressed();
    slave.buffer.buttonB = buttonB.isPressed();
    slave.buffer.buttonC = buttonC.isPressed();

    // Change this to readBatteryMillivoltsLV() for the LV model.
    slave.buffer.batteryMillivolts = readBatteryMillivolts();

    for (uint8_t i = 0; i < 6; i++)
    {
        slave.buffer.analog[i] = analogRead(i);
    }

    // READING the buffer is allowed before or after finalizeWrites().
    ledYellow(slave.buffer.yellow);
    ledGreen(slave.buffer.green);
    ledRed(slave.buffer.red);

    int set_left  = slave.buffer.leftMotor;
    int set_right = slave.buffer.rightMotor;

    // If the commanded motor values are inside the allowed range,
    // set the speet.  We expect values between -300 and 300.
    if (set_left >= -300 && set_left <= 300 && set_right >= -300 && set_right <= 300)
    {
        motors.setSpeeds(set_left, set_right);
    }

    // Playing music involves both reading and writing, since we only
    // want to do it once.
    static bool startedPlaying = false;

    if (slave.buffer.playNotes && !startedPlaying)
    {
        buzzer.play(slave.buffer.notes);
        startedPlaying = true;
    }
    else if (startedPlaying && !buzzer.isPlaying())
    {
        slave.buffer.playNotes = false;
        startedPlaying         = false;
    }

    slave.buffer.leftEncoder  = encoders.getCountsLeft();
    slave.buffer.rightEncoder = encoders.getCountsRight();

    // When you are done WRITING, call finalizeWrites() to make modified
    // data available to I2C master.
    slave.finalizeWrites();
}
