#include <Icm20948.h>
#include <SensorTypes.h>
#include "Icm20948MPUFifoControl.h"
#include <Wire.h>
#include "UbxEuler.h"

// Serial Monitor port
#define debug_port_ Serial
#define output_port Serial

// #define DEBUG
#ifdef DEBUG
#define debugPrint(x) debug_port_.print(x)
#define debugPrintln(x) debug_port_.println(x)
#else
#define debugPrint(x)
#define debugPrintln(x)
#endif

//#define PUBLISH_VN
#define PUBLISH_UBX

const float kDeg2Rad = 0.017453293f;
const float kRad2Deg = 57.295779513f;

// ------------------- ICM-20948 Definitions -------------------
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
// ---------------------------------------------------------

// ------------ IMU Definitions ------------
bool new_icm_data_ = false;
float gyro_rad_[3];
float quat_[4];

// ICM-20948 DMP update interval [ms]
const unsigned int kImuIntervalMs = 10;

// VectorNav Publish interval [ms]
const unsigned int kPublishVnBinary1IntervalMs = 20; // MUST BE MULTIPLE OF kImuIntervalMs
const unsigned int kPublishVnBinary1Count = kPublishVnBinary1IntervalMs / kImuIntervalMs;


float attitude_rad_[3]; // roll,pitch,yaw

const int kVnBinary1Length = 30;//108;
byte vn_binary1_buffer_[kVnBinary1Length];

// Ubx definitions
UbxEuler ubx_euler_;

// ------------ Loop Definitions ------------
unsigned int main_loop_count_ = 0;

void setup()
{
  //------------- Serial Port Setup -----------------
#ifdef DEBUG
  debug_port_.begin(115200);
#endif
  output_port.begin(115200);

  //------------- ICM Setup --------------
  debugPrintln("ICM Setup...");
  icmSetup();
  debugPrintln("Done!");
  // ----------- Publish VN Setup --------------
  vn_binary1_buffer_[0] = 0xFA;
  vn_binary1_buffer_[1] = 1;
}


void loop()
{
  pollIcm();
  if (imuTasks())
  {
    main_loop_count_++;
    if (main_loop_count_ == kPublishVnBinary1Count)
    {
#ifdef PUBLISH_VN
      publishVnBinary1();
#endif
#ifdef PUBLISH_UBX
publishUbxBinary();
#endif
      printAttitudeDeg();
      main_loop_count_ = 0;
    }
  }
}
