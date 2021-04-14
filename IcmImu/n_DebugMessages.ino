void printAttitudeDeg() {
  debugPrintln("R/P/Y: " + String(attitude_rad_[0] * kRad2Deg ) + ", " + String(attitude_rad_[1] * kRad2Deg) + ", " + String(attitude_rad_[2] * kRad2Deg));
}

void printTime() {
  debugPrintln(String(micros() / 1000000.0, 3));
}

void printGyroDeg() {
  debugPrintln("Gyro float: " + String(gyro_rad_[0]* kRad2Deg) + ", " + String(gyro_rad_[1]* kRad2Deg) + ", " + String(gyro_rad_[2]* kRad2Deg));
}

void printQuat() {
  debugPrintln("quat: " + String(quat_[0], 4) + ", " + String(quat_[1], 4) + ", " + String(quat_[2], 4) + ", " + String(quat_[3], 4));
}
