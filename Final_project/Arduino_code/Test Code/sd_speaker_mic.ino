// ----------------------------------------------
// Validate SD card, speaker and mic connection
// Type 'r' to record and 's' to stop
// Recording will playback immediately
// Recordings should be saved on the SD card 
// starting from "1.wav" and increment up
// ----------------------------------------------

/*
NOTE : to use TMRpcm recording functions, uncomment 
#define buffSize 128 AND #define ENABLE_RECORDING in pcmConfig.h
https://github.com/TMRh20/TMRpcm/wiki/Advanced-Features#recording-audio
*/

#include <SD.h> 
#include <SPI.h> 
#include <TMRpcm.h> //for audio playing & recording

const int sd_cs_pin = 4;      
const int amp_sd_pin = 1;    // turns amplifier on/off
const int mic_pin = A0;
const int speaker_pin = 9;

TMRpcm audio;
int recordingCount = 0;
String fileName;

void setup() {
  Serial.begin(9600);

  pinMode(amp_sd_pin, OUTPUT);
  audio.speakerPin = speaker_pin;
  audio.CSPin = sd_cs_pin;

  // Initialize SD card 
  if (!SD.begin(sd_cs_pin)) {
    return;
  }

  File dir = SD.open("/");
  recordingCount = getRecordingCount(dir);
  dir.close();

  audio.quality(1); 
  audio.setVolume(3);
}

void loop() {

  if(Serial.available()){     
  int input = Serial.read();
    if (input == 'r') {
      Serial.print("Recording..");

      fileName = String(recordingCount) + ".wav";
      Serial.println(fileName);
      int charLength = fileName.length()+1;
      char charBuf[charLength];
      fileName.toCharArray(charBuf, charLength);

      audio.startRecording(charBuf, 16000, mic_pin); 
    }               
          
    else if (input == 's') {
      Serial.println("Recording stopped");

      fileName = String(recordingCount) + ".wav";
      int charLength = fileName.length()+1;
      char charBuf[charLength];
      fileName.toCharArray(charBuf, charLength);

      audio.stopRecording(charBuf); 
      recordingCount += 1;

      delay(1000);
      digitalWrite(amp_sd_pin, HIGH);
      audio.play(charBuf);
      delay(10000);
      digitalWrite(amp_sd_pin, LOW);
    }
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