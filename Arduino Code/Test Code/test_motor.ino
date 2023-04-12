// ------------------------------------------------------------
// Code to validate motor and limit switches
// With paddle installed on the motor shaft, motor should spin
// ccw until paddle hits the limit switch, reverse direction 
// and spin cw until it hits the other switch and repeat
// ------------------------------------------------------------

const int motor_pwm_pin = 5;  // motor speed control
const int motor_in1_pin = 3;  // I3 on motor controller
const int motor_in2_pin = 6;  // I4 on motor controller
const int switch_pin = 8;    // limit switches share a pin

int dir = 1; //cw = 1, ccw = 0

void setup()
{
  pinMode(motor_pwm_pin,OUTPUT) ;  	
  pinMode(motor_in1_pin,OUTPUT) ;
  pinMode(motor_in2_pin,OUTPUT) ;
  pinMode(switch_pin, INPUT_PULLUP);

  // spin cw initially
  digitalWrite(motor_in1_pin, LOW) ;
  digitalWrite(motor_in2_pin, HIGH) ;
  analogWrite(motor_pwm_pin,50) ;
}
 
void loop() {

  if (digitalRead(switch_pin) == LOW && dir == 1)
  {
    // Switch 1 CLOSED
    digitalWrite(motor_in1_pin, LOW) ;
    digitalWrite(motor_in2_pin, LOW) ;

    delay(500);

    // spin ccw, I3 HIGH, I4 LOW
    digitalWrite(motor_in1_pin, HIGH) ;
    digitalWrite(motor_in2_pin, LOW) ;
    analogWrite(motor_pwm_pin,100) ;
    dir = 0;
    delay(200);
  }
  
  if (digitalRead(switch_pin) == LOW && dir == 0)
  {
    // Switch 2 CLOSED
    digitalWrite(motor_in1_pin, LOW) ;
    digitalWrite(motor_in2_pin, LOW) ;

    delay(500);

    // spin cw, I4 HIGH, I3 LOW
    digitalWrite(motor_in1_pin, LOW) ;
    digitalWrite(motor_in2_pin, HIGH) ;
    analogWrite(motor_pwm_pin,100) ;
    dir = 1;
    delay(200);
  }
}