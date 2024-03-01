#include <Icm20948.h>
#include <SensorTypes.h>
#include "Icm20948MPUFifoControl.h"
#include <Wire.h>
#include <DrvInterpreter.h>
#include <DigitalButton.h>

// Serial Monitor port
#define debug_port Serial
#define output_port Serial1

#define DEBUG
#ifdef DEBUG
#define debugPrint(x) debug_port.print(x)
#define debugPrintDec(x, y) debug_port.print(x, y)
#define debugPrintHex(x) debug_port.print(x, HEX)
#define debugPrintln(x) debug_port.println(x)
#define debugPrintlnDec(x, y) debug_port.println(x, y)
#define debugPrintlnHex(x) debug_port.println(x, HEX)

#else
#define debugPrint(x)
#define debugPrintDec(x, y)
#define debugPrintHex(x)
#define debugPrintln(x)
#define debugPrintlnDec(x, y)
#define debugPrintlnHex(x)
#endif

#define ANALOG_READ_MAX_COUNTS 1023

const float kDeg2Rad = 0.017453293f;
const float kRad2Deg = 57.295779513f;

// ------------------- ICM-20948 Definitions -------------------
int rc = 0;
#define THREE_AXES 3
static int unscaled_bias[THREE_AXES * 2];
/* FSR configurations */
int32_t cfg_acc_fsr = 4;    // Default = +/- 4g. Valid ranges: 2, 4, 8, 16
int32_t cfg_gyr_fsr = 2000; // Default = +/- 2000dps. Valid ranges: 250, 500, 1000, 2000
static const uint8_t dmp3_image[] =
    {
#include "icm20948_img.dmp3a.h"
};

// static const float cfg_mounting_matrix[9] = {
//     1.f, 0, 0,
//     0, 1.f, 0,
//     0, 0, 1.f};

// For 90 pitch rotation
static const float cfg_mounting_matrix[9] = {
    0, 0, 1,
    0, 1, 0,
    -1, 0, 0};

static uint8_t convert_to_generic_ids[INV_ICM20948_SENSOR_MAX] = {
    INV_SENSOR_TYPE_ACCELEROMETER,
    INV_SENSOR_TYPE_GYROSCOPE,
    INV_SENSOR_TYPE_RAW_ACCELEROMETER,
    INV_SENSOR_TYPE_RAW_GYROSCOPE,
    INV_SENSOR_TYPE_UNCAL_MAGNETOMETER,
    INV_SENSOR_TYPE_UNCAL_GYROSCOPE,
    INV_SENSOR_TYPE_BAC,
    INV_SENSOR_TYPE_STEP_DETECTOR,
    INV_SENSOR_TYPE_STEP_COUNTER,
    INV_SENSOR_TYPE_GAME_ROTATION_VECTOR,
    INV_SENSOR_TYPE_ROTATION_VECTOR,
    INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR,
    INV_SENSOR_TYPE_MAGNETOMETER,
    INV_SENSOR_TYPE_SMD,
    INV_SENSOR_TYPE_PICK_UP_GESTURE,
    INV_SENSOR_TYPE_TILT_DETECTOR,
    INV_SENSOR_TYPE_GRAVITY,
    INV_SENSOR_TYPE_LINEAR_ACCELERATION,
    INV_SENSOR_TYPE_ORIENTATION,
    INV_SENSOR_TYPE_B2S};
#define AK0991x_DEFAULT_I2C_ADDR 0x0C   /* The default I2C address for AK0991x Magnetometers */
#define AK0991x_SECONDARY_I2C_ADDR 0x0E /* The secondary I2C address for AK0991x Magnetometers */

#define ICM_I2C_ADDR_REVA 0x68 /* I2C slave address for INV device on Rev A board */
#define ICM_I2C_ADDR_REVB 0x69 /* I2C slave address for INV device on Rev B board */

#define AD0_VAL 1 // The value of the last bit of the I2C address.
uint8_t I2C_Address = 0x69;
inv_icm20948_t icm_device_;
// ---------------------------------------------------------
// Pedals
const int kAccelPedalPin = A2;
const int kBrakePedalPin = A1;
// Accel Analog Ouput
const float kPedalVoltageMaxV = 4;
const float kPedalVoltageMinV = 1;

bool _new_icm_data = false;
float _quat[4];
const unsigned int kImuIntervalMs = 10;
float _attitude_rad[3];

float _pilot_accel_scaled, _pilot_brake_scaled, _pilot_steering_angle_deg;
float _max_accel_scaled = 1.0f;
const float kDeltaMaxAccelScaled = 0.1f;
const float kMaxSteeringAngleDeg = 90;

// -------------------------------------------
const int kAlignHeadsetButtonPin = MISO;
const int kActionButtonPin = MOSI;
DigitalButton _action_button(kActionButtonPin, 3, true);
DigitalButton _align_headset_button(kAlignHeadsetButtonPin, 3, true);

DrvInterpreter _drv_interpreter;

void setup()
{
  //------------- Serial Port Setup -----------------
#ifdef DEBUG
  debug_port.begin(115200);
#endif
  output_port.begin(115200);

  debugPrintln("ICM Setup...");
  icmSetup();
  debugPrintln("Done!");
  pinMode(kActionButtonPin, INPUT_PULLUP);
  pinMode(kAlignHeadsetButtonPin, INPUT_PULLUP);

  _pilot_accel_scaled = 0;
  _pilot_brake_scaled = 0;
}

void loop()
{
  pollIcm();
  if (checkForData())
  {
    // printAttitudeDeg();
    // printTime();
    getPedalInput();
    sendPilotCommands();
  }
}
