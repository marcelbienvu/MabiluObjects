#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include "oled.h"
#include "ui.h"

DMAMEM uint16_t reverb_buffer[ELEMENTS_BUFFER_SIZE];
 char buffer[PLAITS_BUFFER_SIZE];

Ui ui;

Lfo                           lfo;
AudioSynthWaveformModulated   osc;
Warps                         processor;
AudioOutputI2S                dac;
Elements                      resonator(reverb_buffer);
AudioAmplifier                amp1, amp2, amp3, amp4;
Plaits                        osc2(&buffer);

AudioConnection con5(lfo, 0, amp3, 0);
AudioConnection con10(amp3, 0, osc, 0);
AudioConnection con15(osc, 0, resonator, 0);
AudioConnection con7(resonator, 0, processor, 1);
AudioConnection con6(osc2, 0, processor, 0);
AudioConnection con(processor, 0, amp1, 0);
AudioConnection con2(processor, 1, amp2, 0);
AudioConnection con3(amp1, 0, dac, 0);
AudioConnection con4(amp2, 0, dac, 1);

elapsedMillis timer = 0;
uint8_t counter = 0;

void setup() {

  oledInit();

  AudioNoInterrupts();
  osc.begin(WAVEFORM_SINE);
  osc.frequency(220);
  osc.amplitude(0.1);
  amp3.gain(0.1);
  amp4.gain(0.4f);
  amp1.gain(0.6f);
  amp2.gain(0.6f);
  Serial.begin(115200);
  AudioInterrupts();

  AudioMemory(16);
}

void apply();

void loop() {
  ui.update();
  display();
  apply();
  Serial.printf("Proc: %1.2f\tMem: %u\n", AudioProcessorUsage(), AudioMemoryUsage());

  if(timer >= 1000 && random(20) < 1 ) {
    resonator.gate(0);
    delay(300);
    resonator.gate(1);
    timer = 0;
  }
}

void apply() {
  AudioNoInterrupts();
  resonator.strike_level(ui.get(6));
  resonator.strike_timbre(ui.get(7));
  resonator.geometry(ui.get(8));
  resonator.damping(ui.get(9));
  resonator.bow_level(ui.get(10));
  processor.channel_drive(0, ui.get(0));
  osc2.timbre(ui.get(24));
  osc2.harmonics(ui.get(23));
  osc2.morph(ui.get(21));
  lfo.set_rate(ui.get(1));
  lfo.set_shape(ui.get(2));
  AudioInterrupts();
}