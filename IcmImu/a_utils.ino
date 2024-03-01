float applyDeadband(float value, float threshold = 0.01f)
{
    if ((value < -threshold) || (value > threshold))
    {
        return value;
    }
    return 0;
}