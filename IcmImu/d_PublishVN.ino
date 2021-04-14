#define field_bit(x) (1<<(x))

void publishVnBinary1()
{
  // Fields 1
  // 3: YPR (12)
  // 5: Body rate (12)
  uint16_t fields_word = field_bit(3) | field_bit(5);
  byte fields_1 = fields_word & 0xFF;
  byte fields_2 = fields_word >> 8;
  vn_binary1_buffer_[2] = fields_1;
  vn_binary1_buffer_[3] = fields_2;
  // Sum of Data: 12 + 12 = 24
  // Message Length: 24 + 6 = 30
  int index = 4;
  float yaw_deg = attitude_rad_[2] * kRad2Deg;
  float pitch_deg = attitude_rad_[1] * kRad2Deg;
  float roll_deg = attitude_rad_[0] * kRad2Deg;
  memcpy(&vn_binary1_buffer_[index], &yaw_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &pitch_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &roll_deg, 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_rad_[0], 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_rad_[1], 4); index += 4;
  memcpy(&vn_binary1_buffer_[index], &gyro_rad_[2], 4); index += 4;
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
