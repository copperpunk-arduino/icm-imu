void publishUbxBinary()
{
    float roll_deg = attitude_rad_[0] * kRad2Deg;
    float pitch_deg = attitude_rad_[1] * kRad2Deg;
    float yaw_deg = attitude_rad_[2] * kRad2Deg;
    ubx_euler_.buildEuler16Message(roll_deg, pitch_deg, yaw_deg);
    ubx_euler_.writeMessageInTxBuffer(&output_port);
}