/********************************************************************
Expected behaviour:
  (ear side)
  1. wait for button press 
  *button pressed*
  2. turn LED on
  3. start recording
  *button pressed again or after 15 seconds*
  4. stop recording 
  5. turn LED off
  6. save recording to SD card and name file as "#.wav"
  7. rotate motor counterclockwise until it hits limit switch #2 
  (mouth side)
  8. play audio clips 
  10. Wait for ~6s, rotate clockwise until it hits limit switch #1 
  (ear side)
  *repeat*
  
NOTE : to use TMRpcm recording functions, uncomment 
#define buffSize 128 AND #define ENABLE_RECORDING in pcmConfig.h
https://github.com/TMRh20/TMRpcm/wiki/Advanced-Features#recording-audio
*********************************************************************/

#include <SD.h> 
#include <SPI.h> 
#include <TMRpcm.h> //for audio playing & recording

// IO pins
const int motor_pwm_pin = 5;  // motor speed control
const int motor_in1_pin = 3;  // I3 on motor controller
const int motor_in2_pin = 6;  // I4 on motor controller
const int switch_pin = 8;    // limit switches share a pin

const int sd_cs_pin = 4;      
const int amp_sd_pin = 1;    // turns amplifier on/off
const int mic_pin = A0;
const int speaker_pin = 9;

const int led_pin = 7;   
const int button_pin = 2; 

// global variables
TMRpcm audio;
bool isRecording = false;
bool btnDisabled = false;
int recordingCount = 0;
int prevBtnState = HIGH; // prevents unwanted behaviour if button was held down
int recordingStart = 0;
int switchStart = 0;
int dir = 1;            // cw = 1, ccw = 0;
String fileName;

void setup() {

  pinMode(motor_pwm_pin,OUTPUT) ;  	
  pinMode(motor_in1_pin,OUTPUT) ; 	
  pinMode(motor_in2_pin,OUTPUT) ;
  pinMode(switch_pin, INPUT_PULLUP);   // use internal pull-up for switches/buttons, reverses normal states

  pinMode(amp_sd_pin, OUTPUT);
  audio.speakerPin = speaker_pin;
  audio.CSPin = sd_cs_pin;

  pinMode(button_pin, INPUT_PULLUP); 
  pinMode(led_pin, OUTPUT);

  // Initialize SD card 
  if (!SD.begin(sd_cs_pin)) {
    outputError("SD card initialization failed!"); 
    return;
  }

  File dir = SD.open("/");
  recordingCount = getRecordingCount(dir);
  dir.close();

  audio.quality(1); 
  digitalWrite(amp_sd_pin, LOW);
  digitalWrite(led_pin, LOW);

  // rotate to ear side as starting position (cw)
  digitalWrite(motor_in1_pin, LOW);
  digitalWrite(motor_in2_pin, HIGH);
  analogWrite(motor_pwm_pin, 50);
}

void loop() {

  int buttonState = digitalRead(button_pin);
  int switchState = digitalRead(switch_pin);
  int recordingTime = millis() - recordingStart;
  int rotateTime = millis() - switchStart;

  // button pressed to begin recording
  if (btnDisabled == false && buttonState == LOW && prevBtnState == HIGH && isRecording == false) 
  {
    digitalWrite(led_pin, HIGH);
    isRecording = true;
    recordingStart = millis();
    
    // generate file name
    fileName = String(recordingCount) + ".wav";
    int charLength = fileName.length()+1;
    char charFileName[charLength];
    fileName.toCharArray(charFileName, charLength);
    audio.startRecording(charFileName, 16000, mic_pin); 
  }

  else if ((buttonState == LOW && prevBtnState == HIGH && isRecording == true) || (recordingTime > 15000 && isRecording == true))
  {
    digitalWrite(led_pin, LOW);
    isRecording = false;
    btnDisabled = true;

    // stop and save recording
    int charLength = fileName.length()+1;
    char charFileName[charLength];
    fileName.toCharArray(charFileName, charLength);
    audio.stopRecording(charFileName); 
    
    recordingCount += 1;
    delay(2000);

    // spin ccw, I3 HIGH, I4 LOW
    switchStart = millis();
    digitalWrite(motor_in1_pin, HIGH);
    digitalWrite(motor_in2_pin, LOW);
    analogWrite(motor_pwm_pin, 80);
    dir = 0;
    delay(200);
  }

  else if ((switchState == LOW && dir == 1 ) || (rotateTime > 1640 && dir == 1))
  {
    // stop motor
    digitalWrite(motor_in1_pin, LOW);
    digitalWrite(motor_in2_pin, LOW);
  }

  else if ((switchState == LOW && dir == 0) || (rotateTime > 1900 && dir == 0))
  {
    // stop motor
    digitalWrite(motor_in1_pin, LOW);
    digitalWrite(motor_in2_pin, LOW);

    delay(1000);

    // thank you msg
    audio.setVolume(4);
    digitalWrite(amp_sd_pin, HIGH);
    audio.play("ty.wav");
    delay(4000);

    // play random secret
    audio.setVolume(5);
    randomSeed(A0);
    int rand = int(random(1,recordingCount-1));
    fileName = String(rand) + ".wav";
    int charLength = fileName.length()+1;
    char charFileName[charLength];
    fileName.toCharArray(charFileName, charLength);

    File file = SD.open(charFileName);
    int duration = file.size()/16000 * 1000 + 6000;
    file.close();

    audio.play(charFileName);
    delay(duration);    
    digitalWrite(amp_sd_pin, LOW);

    // rotate cw back to ear side, I4 HIGH, I3 LOW
    switchStart = millis();
    digitalWrite(motor_in1_pin, LOW);
    digitalWrite(motor_in2_pin, HIGH);
    analogWrite(motor_pwm_pin,80);
    switchStart = millis();
    dir = 1;
    btnDisabled = false;
    delay(200);
  }
  
  prevBtnState = buttonState;
  delay(200);
}

// output error message and flash LED
void outputError(String error_msg) {
  // Serial.println(error_msg);
  while (true) {
   digitalWrite(led_pin, HIGH);
   delay(250);
   digitalWrite(led_pin, LOW);
   delay(250);
  }
}

// look for the last recording on the SD card for file name indexing
int getRecordingCount(File dir){
  int maxCount = 0;

  while (true) {
    File entry =  dir.openNextFile();

    if (! entry) {
      // no more files
      return maxCount + 1;
    }

    String fileName = String(entry.name());
    int count = fileName.toInt();
    if (count > maxCount){
      maxCount = count;
    }

    entry.close();
  }
}
