const float Kp;
const float Ki;
const float Kd;

float controller (float setpoint, float sensor_reading)
{
    static float I = 0;
    float error = setpoint - sensor_reading;
    float P = Kp * error;
    float dI = Ki * error * delta_t;
    I = (I - dI);
    float control = P - I;

    return control;
}

