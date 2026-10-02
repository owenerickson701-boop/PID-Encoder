#include <Arduino.h>
#include <PID_v1.h>

double Setpoint, Input, Output;
double Kp = 2, Ki = 50, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

const int pin1 = 9;
const int pin2 = 10;
const int pinIn = 5; // set

const int lim = 1200; // tune to your environment. Works best with a strong light source.

short resolution = 3; // slots on encoder

float speed = 0;   // Current speed in rps
float power;       // power level of pin out
byte flag = false; // IF mid-signal
float lastTime;
float currentTime;

float light;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  speed = 0;
  Input = speed;
  Setpoint = 3; // rev/s

  // turn the PID on
  myPID.SetMode(AUTOMATIC);
  currentTime = millis();
  lastTime = currentTime;
  analogWrite(pin1, 255); // initialize
  analogWrite(pin2, 0);
}

void loop()
{
  // Measure light
  light = analogRead(pinIn);
  // Serial.println(light);

  if (light > lim) // light ON
  {
    if (flag)
    { // do nothing
    }
    else
    { // Signal starts, do math and set speed
      flag = true;
      lastTime = currentTime;
      currentTime = millis();

      // Recalc speed

      speed = 1000 * resolution / (currentTime - lastTime); // rev/s

      // PID tune speed
      Input = speed;
      myPID.Compute();

      // power = 255;
      power = Output; // if PID is on
      if (power < 100)
      {
        power = 100; // So there's no total stall
      }

      analogWrite(pin1, power);
      analogWrite(pin2, 0);

      Serial.print("   Speed: ");
      Serial.print(speed);
      Serial.print("     dT:");
      Serial.println(lastTime - currentTime);
    }
  }
  else // light OFF
  {
    if (flag)
    {
      flag = false;
    }
  }
  // Serial.print("Flag: ");
  // Serial.println(flag);
  // Serial.print("Light Level: ");
  // Serial.println(light);
}