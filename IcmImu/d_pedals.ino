void getPedalInput()
{
    _pilot_brake_scaled = getScaledValue(kBrakePedalPin);
    if (_pilot_brake_scaled > 0)
    {
        _pilot_accel_scaled = 0;
    }
    else
    {
        _pilot_accel_scaled = _max_accel_scaled * getScaledValue(kAccelPedalPin);
    }
}

float getScaledValue(int pin)
{
    float pin_voltage = map_f(analogRead(pin), 0, ANALOG_READ_MAX_COUNTS, 0, 5);
    float capped_pin_voltage = constrain(pin_voltage, kPedalVoltageMinV, kPedalVoltageMaxV);
    // debugPrint("pin voltage (");
    // debugPrint(pin);
    // debugPrint(": ");
    // debugPrintDec(pin_voltage, 2);
    // debugPrint("/: ");
    // debugPrintlnDec(capped_pin_voltage, 2);

    return applyDeadband(map_f(capped_pin_voltage, kPedalVoltageMinV, kPedalVoltageMaxV, 0, 1));
}