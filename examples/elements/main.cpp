#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>

DMAMEM uint16_t reverb_buffer[ELEMENTS_BUFFER_SIZE];

Elements            generator(reverb_buffer);
AudioOutputI2S      dac;

AudioConnection con(generator, 0, dac, 0);
AudioConnection con2(generator, 1, dac, 1);

elapsedMillis timer = 0;

float chords[13][8] = {
    { 0.0f, 4.0f, 5.0f, 7.0f, 8.0f, 11.0f, 16.0f, 17.0f },
    { 0.0f, 1.0f, 5.0f, 7.0f, 8.0f, 12.0f, 13.0f, 17.0f },
    { -12.0f, -0.01f, 0.0f,  0.01f, 0.02f, 11.98f, 11.99f, 12.0f }, // OCT
    { -12.0f, -5.0f,  0.0f,  6.99f, 7.0f,  11.99f, 12.0f,  19.0f }, // 5
    { -12.0f, -5.0f,  0.0f,  5.0f,  7.0f,  11.99f, 12.0f,  17.0f }, // sus4
    { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 12.0f,  19.0f }, // m 
    { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 10.0f,  19.0f }, // m7
    { -12.0f, -5.0f,  0.0f,  3.0f, 14.0f,   3.01f, 10.0f,  19.0f }, // m9
    { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 10.0f,  17.0f }, // m11
    { -12.0f, -5.0f,  0.0f,  2.0f,  7.0f,   9.0f,  16.0f,  19.0f }, // 69
    { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.0f,  14.0f,  19.0f }, // M9
    { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.0f,  10.99f, 19.0f }, // M7
    { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.99f, 12.0f,  19.0f }
};

void setup() {
  Serial.begin(115200);
  AudioMemory(200);
}

void loop() {
  generator.bow_level(analogRead(14) / 1023.0f);

  if(timer >= 2000 && random(10) <=1 ) {
    generator.gate(0);
    delay(20);
    generator.note(12.0f + chords[0][random(7)]);
    generator.gate(1);
    timer = 0;
  }
}