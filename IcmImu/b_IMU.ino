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
  _attitude_rad[0] = atan2(2.0 * (_quat[0] * _quat[1] + _quat[2] * _quat[3]), (1.0 - 2.0 * (_quat[1] * _quat[1] + _quat[2] * _quat[2])));
  _attitude_rad[1] = asin(2 * (_quat[0] * _quat[2] - _quat[3] * _quat[1]));
  _attitude_rad[2] = atan2(2.0 * (_quat[0] * _quat[3] + _quat[1] * _quat[2]), (1.0 - 2.0 * (_quat[2] * _quat[2] + _quat[3] * _quat[3])));
}

void sendRollMessage()
{
  _drv_interpreter.packValue(_attitude_rad[0], 0);
  _drv_interpreter.prepareMessage();
  _drv_interpreter.writeMessage(&output_port);
}
