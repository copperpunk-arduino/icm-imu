#include <Arduino.h>
#include "wiring_private.h"
#include <Icm20948.h>
#include <SensorTypes.h>
#include "Icm20948MPUFifoControl.h"
#include <RazorSerial.h>
#include <RazorICM.h>
#include <RazorConstants.h>
#include <SevenStateEKF.h>
#include <UBX_Parser.h>
#include <Wire.h>

#define NEOPIXEL_PIN 12

// Serial Monitor port
#define debug_port_ Serial
#define output_port Serial1

//#define DEBUG
#ifdef DEBUG
#define DebugPrint(x) debug_port_.print(x)
#define DebugPrintDec(x) debug_port_.print(x, DEC)
#define DebugPrintHex(x) debug_port_.print(x, HEX)
#define DebugPrintln(x) debug_port_.println(x)
#else
#define DebugPrint(x)
#define DebugPrintDec(x)
#define DebugPrintHex(x)
#define DebugPrintln(x)
#endif

long led_time_prev_ms_ = 0;
long loop_time_prev_ms_ = 0;
const long kLedTimeThresholdMs = 250;
const long kLoopTimeThresholdMs = 1000;
bool led_on_ = false;
int gyro_count_ = 0;



// SD Definitions
#define ACCEL_CAL_FILE_NAME "acc_cal.txt"
#define GYRO_OFFSETS_FILE_NAME "gyro_cal.txt"
#define SD_PIN 38
#define SD_CONVERT_LONG_TO_FLOAT 0.000001
#define SD_CONVERT_FLOAT_TO_LONG 1000000
boolean bSDCardInit = false;

long system_time_prev_us_;

// ICM Definitions

int rc = 0;
#define THREE_AXES 3
static int unscaled_bias[THREE_AXES * 2];
/* FSR configurations */
int32_t cfg_acc_fsr = 4; // Default = +/- 4g. Valid ranges: 2, 4, 8, 16
int32_t cfg_gyr_fsr = 2000; // Default = +/- 2000dps. Valid ranges: 250, 500, 1000, 2000
static const uint8_t dmp3_image[] =
{
#include "icm20948_img.dmp3a.h"
};
/*
  Mounting matrix configuration applied for Accel, Gyro and Mag
*/

static const float cfg_mounting_matrix[9] = {
  1.f, 0, 0,
  0, 1.f, 0,
  0, 0, 1.f
};


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
  INV_SENSOR_TYPE_B2S
};
#define AK0991x_DEFAULT_I2C_ADDR  0x0C  /* The default I2C address for AK0991x Magnetometers */
#define AK0991x_SECONDARY_I2C_ADDR  0x0E  /* The secondary I2C address for AK0991x Magnetometers */

#define ICM_I2C_ADDR_REVA      0x68  /* I2C slave address for INV device on Rev A board */
#define ICM_I2C_ADDR_REVB     0x69  /* I2C slave address for INV device on Rev B board */

#define AD0_VAL   1     // The value of the last bit of the I2C address.
uint8_t I2C_Address = 0x69;
inv_icm20948_t icm_device_;
bool new_icm_data_ = false;

RazorICM imu_;
float accel_body_[3];
float gyro_float_[3];
float quat_[4];


// Main Loop Defitions
unsigned int main_loop_counter_ = 0;
const unsigned int kMainLoopCounterMax = 50; // IMU frequency

//const float kImuSampleTime = 1.0 / ((float)kMainLoopCounterMax);

// Frequencies
const unsigned int kPublishImuFrequency = 50;
// Intervals (calculated from frequencies)
const unsigned int kPublishImuInterval = kMainLoopCounterMax / kPublishImuFrequency;
// Values (so we can offset things happening at the same frequency)
const unsigned int kPublishImuValue = 0;


float attitude_[4]; // roll,pitch,yaw,magYaw

const int kVnBinary1Length = 30;//108;
byte vn_binary1_buffer_[kVnBinary1Length];

// MISC definitions
unsigned long imuStartTime;
unsigned long totalStartTime;
boolean bPrintAnything = false;
boolean bEcho = false;
boolean bNewImu = false;
short totalMaxIndex = 0;

void setup()
{
  system_time_prev_us_ = millis();
  //------------- Debug Port Setup BEGIN -----------------
#ifdef DEBUG
  debug_port_.begin(115200);
#endif


  output_port.begin(115200);

  //------------- Debug Port Setup END -----------------
  // GPIO
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(NEOPIXEL_PIN, OUTPUT);
  // turn NeoPixel off
  digitalWrite(NEOPIXEL_PIN, LOW);
  // GPIO
  //------------- IMU Setup BEGIN--------------
  DebugPrintln("ICM Setup...");
  IcmSetup();
  DebugPrintln("Done!");
  //------------- IMU Setup END--------------

  // ----------- Publish Setup BEGIN --------------
  //VN Binary1
  vn_binary1_buffer_[0] = 0xFA;
  vn_binary1_buffer_[1] = 1;
}


void loop()
{
  ClockTasks();
  PollIcm();
  if (imuTasks()) {
//    printAttitude();
//    printGyroFloat();
    if ((main_loop_counter_ % kPublishImuInterval) == kPublishImuValue) {
      PublishVnBinary1();
    }
    main_loop_counter_++;
    if (main_loop_counter_ >= kMainLoopCounterMax) {
      main_loop_counter_ = 0;
      //      printAttitude();
      //      printAccelBody();
    }
  }
}
