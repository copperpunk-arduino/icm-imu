void printTime()
{
  debugPrintln(String(micros() / 1000000.0, 3));
}

void printAttitudeDeg()
{
  debugPrintln("R/P/Y: " + String(_attitude_rad[0] * kRad2Deg) + ", " + String(_attitude_rad[1] * kRad2Deg) + ", " + String(_attitude_rad[2] * kRad2Deg));
}
