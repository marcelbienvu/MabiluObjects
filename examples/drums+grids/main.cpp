#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>

Grids               seq;
Kick                kick;
Hihat               hh;
Snare               sd;
AudioMixer4         mixer;
AudioOutputI2S      dac;

AudioConnection con0(seq, 2, hh, 0);
AudioConnection con1(seq, 1, sd, 0);
AudioConnection con2(seq, 0, kick, 0);
AudioConnection con3(kick, 0, mixer, 0);
AudioConnection con4(hh, 0, mixer, 1);
AudioConnection con5(sd, 0, mixer, 2);
AudioConnection con6(mixer, 0, dac, 0);
AudioConnection con7(mixer, 0, dac, 1);

void setup() {
  Serial.begin(115200);
  AudioMemory(12);
  mixer.gain(0, 0.3f);
  mixer.gain(1, 0.3f);
  mixer.gain(2, 0.3f);
}

void loop() {
  seq.density(1, analogRead(14) >> 2);
  Serial.print(AudioProcessorUsage());
  Serial.print("\t");
  Serial.println(AudioMemoryUsage());
}