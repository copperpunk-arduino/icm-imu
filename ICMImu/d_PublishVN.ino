#define field_bit(x) (1<<(x))

void PublishVnBinary1()
{
//  byte vn_binary1_buffer_[kVnBinary1Length];
//  vn_binary1_buffer_[0] = 0xFA;
//  vn_binary1_buffer_[1] = 1; // Group 1
  // Fields 1
  // 1: TimeGPS (8)
  // 3: YPR (12)
  // 5: Body rate (12)
  // 6: Position-LLA (24)
  // 7: Velocity-NED (12)
  //-----------------------
  // Fields 2
  // 10: MagPres (20)
  // 12: INS status (2)
  uint16_t fields_word = field_bit(3) | field_bit(5);
  byte fields_1 = fields_word & 0xFF;
  byte fields_2 = fields_word >> 8;
  vn_binary1_buffer_[2] = fields_1;
  vn_binary1_buffer_[3] = fields_2;
  // Sum of Data: 12 + 12 = 24
  // Message Length: 24 + 6 = 30
  int index = 4;
  float yaw_deg = attitude_[2] * RAD2DEG;
  float pitch_deg = attitude_[1] * RAD2DEG;
  float roll_deg = attitude_[0] * RAD2DEG;
  memcpy(&vn_binary1_buffer_[index], &yaw_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &pitch_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &roll_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_float_[0], 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_float_[1], 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_float_[2], 4); index += 4;
  uint16_t checksum = CalculateVNChecksum(&vn_binary1_buffer_[1], kVnBinary1Length-3);
  vn_binary1_buffer_[index] = checksum >> 8; index++;
  vn_binary1_buffer_[index] = checksum & 0xFF;
 
  output_port.write(vn_binary1_buffer_, kVnBinary1Length);
  output_port.flush();
}

uint16_t CalculateVNChecksum(byte * data, unsigned int len)
{
  unsigned int i = 0;
  uint16_t crc = 0;
  for (i = 0; i < len; i++) {
    crc = (byte)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (byte)(crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00ff) << 5;
  }
  return crc;
}
