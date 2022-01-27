#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include <ArduinoJson.h>
#include "ui.h"


DMAMEM uint8_t block_mem[118784];
DMAMEM uint8_t block_ccm[65536 - 128];
Lfo                 lfo;

AudioAmplifier                amp3;
AudioSynthWaveformModulated   osc;
Clouds              processor(block_mem, sizeof(block_mem), block_ccm, sizeof(block_ccm));
AudioOutputI2S                dac;

AudioConnection con1(lfo, 0, osc, 0);
AudioConnection con2(amp3, 0, osc, 0);
AudioConnection con3(osc, 0, processor, 0);
//AudioConnection con4(osc, 0, processor, 1);
AudioConnection con5(processor, 0, dac, 0);
AudioConnection con6(processor, 0, dac, 1);

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

Ui ui;

StaticJsonDocument<5000> doc;

void setup() {
  AudioNoInterrupts();
  Serial.begin(115200);
  while(!Serial) {}
  osc.begin(WAVEFORM_SINE);
  osc.frequency(220);
  osc.amplitude(0.5f);
  amp3.gain(0.1f);
  AudioInterrupts();
  AudioMemory(200);
}

void jsonConfig();

void loop() {
  ui.update();
  AudioNoInterrupts();
  osc.frequencyModulation(ui.get(16) * 3.0f);
  osc.frequency(ui.get(17) * 1000);
  lfo.set_shape(ui.get(23));
  lfo.set_rate(ui.get(21));
  lfo.set_parameter(ui.get(24));
  processor.reverb(ui.get(0));
  processor.dry_wet(ui.get(1));
  processor.feedback(ui.get(2));
  processor.position(ui.get(6));
  processor.size(ui.get(7));
  processor.texture(ui.get(8));
  processor.density(ui.get(9));
  processor.pitch(ui.get(10) * 48.0f - 48.0f);
  processor.stereo_spread(ui.get(3));
  AudioInterrupts();

  if(timer >= 2000 && random(10) < 1 ) {
    jsonConfig();
//    serializeJson(doc, Serial);
//    Serial.println();
//    processor.trigger(0);
//    delay(2000);
//    processor.trigger(1);

    timer = 0;
  }
  Serial.printf("PROC: %1.3f\n", AudioProcessorUsage());
}

void jsonConfig() {
  doc["osc"]["frequency"] = ui.get(17) * 1000.0f;
  doc["osc"]["modulation amplitude"] = ui.get(16) * 3.0f;
  doc["lfo"]["shape"] = ui.get(23);
  doc["lfo"]["rate"] = ui.get(21);
  doc["lfo"]["parameter"] = ui.get(24);
  doc["clouds"]["reverb"] = ui.get(0);
  doc["clouds"]["dry"] = ui.get(1);
  doc["clouds"]["feedback"] = ui.get(2);
  doc["clouds"]["position"] = ui.get(6);
  doc["clouds"]["size"] = ui.get(7);
  doc["clouds"]["texture"] = ui.get(8);
  doc["clouds"]["density"] = ui.get(9);
  doc["clouds"]["pitch"] = ui.get(10) * 20.0f;
  doc["clouds"]["stereo_spread"] = ui.get(3);
}