/******************************************************************************
Code for book project tech component
******************************************************************************/
#include <UTFT.h>   // for screen
#include <SD.h> 
#include <SPI.h> 
#include <TMRpcm.h> //for auido

const int FSR1_PIN = A0; // Pin connected to force sensors
const int FSR2_PIN = A1;
#define SD_ChipSelectPin 10 
const int AMP_SD = 7;
const int SPEAKER_PIN = 9;
extern uint8_t GroteskNumbers[]; // custom large font for timer
extern uint8_t GroteskLetters[];
bool seated = false;
int show_count = 0;
long timer_start_time = 0;
long seated_start_time = 0;


UTFT myGLCD(ITDB28,A5,A4,A3,A2); // screen
TMRpcm tmrpcm;   // speaker

void setup() 
{
  //Serial.begin(9600);
  
  pinMode(FSR1_PIN, INPUT);
  pinMode(FSR2_PIN, INPUT);
  pinMode(AMP_SD, OUTPUT);
  tmrpcm.speakerPin = SPEAKER_PIN;

  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(GroteskNumbers);
  myGLCD.print("000:00:00", CENTER, 88);

  if (!SD.begin(SD_ChipSelectPin)) {
    return;
  }

  tmrpcm.setVolume(2);    //   0 to 7. Set volume level
  tmrpcm.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal
}

void loop() 
{
  int fsr1Reading = analogRead(FSR1_PIN);
  int fsr2Reading = analogRead(FSR2_PIN);
  randomSeed(fsr1Reading);
  
  if ((fsr1Reading >= 350) && (fsr2Reading >= 350)) // Both chairs occupied
  {
    if (!seated){
      seated = true;
      seated_start_time = millis();
      myGLCD.clrScr();
      myGLCD.print("000:00:00", CENTER, 56);
      myGLCD.setFont(GroteskLetters);
      show_count = show_count + 1;
      myGLCD.print("SHOW", 32, 120);
      myGLCD.setFont(GroteskNumbers);
      myGLCD.print(String(show_count), 192, 120);
    }
    
    //Serial.println("fsr1Reading");
    delay(500);
  }
  else{   // keep counting timer
    if (seated){
      seated = false;
      
      int seated_duration = (millis() - seated_start_time) / 1000;
      play_audio(seated_duration);
      
      timer_start_time = millis();
      myGLCD.clrScr();
      myGLCD.print("000:00:00", CENTER, 88);
    }
    
    long elapsed_seconds = (millis() - timer_start_time) / 1000;
    myGLCD.print(get_output_time(elapsed_seconds), CENTER, 88);
  }
}

String get_output_time(long elapsed_seconds){
  
  int timer_hour = elapsed_seconds / 3600;
  int timer_minute = ((elapsed_seconds / 60)%60);
  int timer_sec = elapsed_seconds % 60;

  String output_time = "";
  if (timer_hour < 10) {
    output_time = "00" + String(timer_hour) + ":";
  }
  else if (timer_hour < 100){
    output_time = "0" + String(timer_hour) + ":";
  }
  else{
    output_time = String(timer_hour) + ":";
  }
  
  if (timer_minute < 10) {
    output_time = output_time + "0" + String(timer_minute) + ":";
  }
  else{
    output_time = output_time + String(timer_minute) + ":";
  }
  
  if (timer_sec < 10) {
    output_time = output_time + "0" + String(timer_sec);
  }
  else{
    output_time = output_time + String(timer_sec);
  }
  return output_time;
}

void play_audio(long seated_duration){
  digitalWrite(AMP_SD, HIGH);
  if (seated_duration < 4){
    tmrpcm.setVolume(4);
    int select = int(random(2));
    if (select == 0){
      tmrpcm.play("BOO1.WAV");
      delay(2000);
    }
    else {
      tmrpcm.play("BOO2.wav");
      delay(2000);
    }
  }
  else if (seated_duration < 300) {
    tmrpcm.setVolume(5);
    int select = int(random(10));
    if (select == 1){
      tmrpcm.play("BIG_APPL.WAV");
      delay(5000);
    }
    else if (select == 3){
      tmrpcm.play("SMALL_~1.WAV");
      delay(3000);
    }
    else if (select == 8){
      tmrpcm.play("SMALL_~2.WAV");
      delay(3000);
    }
  }
  else{
    tmrpcm.setVolume(5);
    int select = int(random(5));
    if (select == 1){
      tmrpcm.play("BIG_APPL.WAV");
      delay(5000);
    }
    else if (select == 2){
      tmrpcm.play("SMALL_~1.WAV");
      delay(3000);
    }
    else if (select == 3){
      tmrpcm.play("CHEERI~1.WAV");
      delay(6000);
    }
  }
  digitalWrite(AMP_SD, HIGH);
}
