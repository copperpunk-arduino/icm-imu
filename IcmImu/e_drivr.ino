void sendPilotCommands()
{
    int8_t steering_angle_rounded_deg = round(_pilot_steering_angle_deg);
    uint8_t accel_pct = round(_pilot_accel_scaled * 100);
    uint8_t brake_pct = round(_pilot_brake_scaled * 100);
    uint8_t pilot_status = _align_headset_button.checkDown() | (_action_button.checkDown() << 1);
    _drv_interpreter.setHeaderValues(DrvMessageId::kPilotCommandsV4, DrvMessageLength::kPilotCommandsV4);
    _drv_interpreter.packValue(accel_pct, 0);
    _drv_interpreter.packValue(brake_pct, 1);
    _drv_interpreter.packValue(steering_angle_rounded_deg, 2);
    _drv_interpreter.packValue(pilot_status, 3);
    _drv_interpreter.prepareMessage();
    // printPilotCommands(accel_pct, brake_pct, steering_angle_rounded_deg, pilot_status);
    _drv_interpreter.writeMessage(&output_port);
}

void printPilotCommands(uint8_t accel_pct, uint8_t brake_pct, int8_t steering_deg, uint8_t pilot_status)
{
    debugPrint(F("Acc/Brk/Str/Stat: "));
    debugPrint(accel_pct);
    debugPrint(F("/"));
    debugPrint(brake_pct);
    debugPrint(F("/"));
    debugPrint(steering_deg);
    debugPrint(F("/"));
    debugPrintln(pilot_status);
}