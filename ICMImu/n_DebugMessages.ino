void PrintVector(String header, float vector[], float mult)
{
  DebugPrintln(header + String(vector[0]*mult ) + ", " + String(vector[1]*mult) + ", " + String(vector[2] * mult));
}

void printAttitude() {
//  DebugPrintln("rel yaw: " + String(rel_pos_yaw_ * RAD2DEG, 2));
  DebugPrintln("R/P/Y: " + String(attitude_[0] * RAD2DEG ) + ", " + String(attitude_[1] * RAD2DEG) + ", " + String(attitude_[2] * RAD2DEG));
}

void printTime() {
  DebugPrintln(String(micros() / 1000000.0, 3));
}

void printAccelBody() {
  DebugPrintln("A body: " + String(accel_body_[0],3) + ", " + String(accel_body_[1],3) + ", " + String(accel_body_[2],3));
}

void printGyroFloat() {
//  imu_.getGyro(gyro_float_);
  DebugPrintln("Gyro float: " + String(gyro_float_[0]* RAD2DEG) + ", " + String(gyro_float_[1]* RAD2DEG) + ", " + String(gyro_float_[2]* RAD2DEG));
}

void printQuat() {
  DebugPrintln("quat: " + String(quat_[0], 4) + ", " + String(quat_[1], 4) + ", " + String(quat_[2], 4) + ", " + String(quat_[3], 4));
}
