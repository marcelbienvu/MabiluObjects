#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include <ArduinoJson.h>
#include "ui.h"

char buffer[PLAITS_BUFFER_SIZE];

Lfo             lfo;
AudioAmplifier  amp, amp2;
Plaits          osc(&buffer);
AudioFilterLadder vcf;
AudioOutputI2S  dac;

AudioConnection con1(lfo, 0, amp, 0);
AudioConnection con2(amp, 0, osc, 0);
AudioConnection con3(osc, 0, vcf, 0);
AudioConnection con6(vcf, 0, amp2, 0);
AudioConnection con4(amp2, 0, dac, 0);
AudioConnection con5(amp2, 0, dac, 1);

elapsedMillis timer = 0;

Ui ui;

StaticJsonDocument<5000> doc;

void setup() {
  AudioNoInterrupts();
  Serial.begin(115200);
  while(!Serial) {}
  amp.gain(0.0f);
  amp2.gain(0.0f);
  AudioInterrupts();

  AudioMemory(16);
}

void jsonConfig();

void loop() {
  ui.update();
  
  AudioNoInterrupts();
  vcf.frequency(ui.get(17) * 4000);
  vcf.resonance(ui.get(20) * 2.0f);

  osc.engine(static_cast<int>(ui.get(6) * 7.0));
  osc.note(static_cast<int>(ui.get(7) * 119.0f));
  osc.timbre(ui.get(8));
  osc.harmonics(ui.get(9));
  osc.morph(ui.get(10));

  amp2.gain(ui.get(5));

//  amp.gain(ui.get(1) - 0.1f);

  lfo.set_shape(ui.get(23));
  lfo.set_rate(ui.get(21));
  lfo.set_parameter(ui.get(24));

  AudioInterrupts();

  if(timer >= 1000 && random(10) < 1 ) {
    jsonConfig();
    serializeJsonPretty(doc, Serial);
    Serial.println();
    timer = 0;
  }
//  Serial.printf("PROC: %1.3f\n", AudioProcessorUsage());
}

void jsonConfig() {
  doc["cpu"]["instant"] = AudioProcessorUsage();
  doc["cpu"]["max"] = AudioProcessorUsageMax();
  doc["mem"] = AudioMemoryUsage();
  doc["vcf"]["frequency"] = ui.get(17) * 4000.0f;
  doc["vcf"]["resonance"] = ui.get(20) * 2.0f;
  doc["osc"]["engine"] = static_cast<int>(ui.get(6) * 7.0);
  doc["osc"]["note"] = static_cast<int>(ui.get(7) * 119.0f);
  doc["osc"]["timbre"] = ui.get(8);
  doc["osc"]["harmonics"] = ui.get(9);
  doc["clouds"]["morph"] = ui.get(10);
}