// ----------------------------------------------
// Validate SD card and speaker connection
// An applause sound clip will play on power up
// Make sure testing.wav is saved on the SD card
// ----------------------------------------------

#include <SD.h> 
#include <SPI.h> 
#include <TMRpcm.h>

#define sd_cs_pin 4
#define amp_sd_pin 10
#define speaker_pin 9

TMRpcm audio;  

void setup() {
  pinMode(amp_sd_pin, OUTPUT);
  audio.speakerPin = speaker_pin;

  if (!SD.begin(sd_cs_pin)) {
    return;
  }

  //tmrpcm.setVolume(2);    //   0 to 7. Set volume level
  audio.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal

  digitalWrite(amp_sd_pin, HIGH);
  audio.play("testing.wav");
  delay(5000);
  digitalWrite(amp_sd_pin, LOW);
}

void loop() {

}
