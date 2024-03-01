boolean checkForData()
{
  if (_new_icm_data)
  {
    _new_icm_data = false;
    convertQuatToEuler();
    return true;
  }
  //  }
  return false;
  //  */
}

void convertQuatToEuler()
{
  _attitude_rad[0] = -atan2(2.0 * (_quat[0] * _quat[1] + _quat[2] * _quat[3]), -(1.0 - 2.0 * (_quat[1] * _quat[1] + _quat[2] * _quat[2])));
  // _attitude_rad[1] = asin(2 * (_quat[0] * _quat[2] - _quat[3] * _quat[1]));
  // _attitude_rad[2] = atan2(2.0 * (_quat[0] * _quat[3] + _quat[1] * _quat[2]), (1.0 - 2.0 * (_quat[2] * _quat[2] + _quat[3] * _quat[3])));
  _pilot_steering_angle_deg = constrain(_attitude_rad[0] * kRad2Deg, -kMaxSteeringAngleDeg, kMaxSteeringAngleDeg);
}

// void sendPilotCommandsMessage()
// {
//   uint8_t pilot_status = _align_headset_button.checkDown() | (_action_button.checkDown() << 1);
//   _drv_interpreter.setHeaderValues(DrvMessageId::kPilotSteeringWheel, DrvMessageLength::kPilotSteeringWheel);
//   _drv_interpreter.packValue(_attitude_rad[0] * kRad2Deg, 0);
//   _drv_interpreter.packValue(pilot_status, 4);
//   _drv_interpreter.prepareMessage();
//   _drv_interpreter.writeMessage(&output_port);
// }

float map_f(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
