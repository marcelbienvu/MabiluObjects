#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include <ArduinoJson.h>
#include "ui.h"
#include <Bounce2.h>

Ui ui;

Bounce button = Bounce();
Bounce button2 = Bounce();

DMAMEM uint16_t reverb_buffer[32768];
DMAMEM uint8_t block_mem[237568];
DMAMEM uint8_t block_ccm[131072 - 256];

Clouds              granular(block_mem, sizeof(block_mem), block_ccm, sizeof(block_ccm));
Marbles             randomizer;
Rings               synth(reverb_buffer);
Tides               lfo;
//Warps               fx;
AudioOutputI2S      dac;
AudioAmplifier      amp, amp2;
AudioMixer4         mixer;
AudioOutputUSB      usb;


AudioConnection con1(randomizer, 4, synth, 1);
AudioConnection con2(synth, 0, amp, 0);
AudioConnection con3(amp, 0, granular, 0);
AudioConnection con4(granular, 0, dac, 1);
AudioConnection con5(granular, 1, dac, 0);

void setup() {
  AudioNoInterrupts();
  Serial.begin(115200);
  //while(!Serial) {}
  pinMode(16, INPUT_PULLUP);
  button.attach(16);
  amp.gain(1.0f);
  AudioInterrupts();
  AudioMemory(200);
}

void loop() {
  ui.update();
  AudioNoInterrupts();
    button.update();
    if(button.fell()) granular.trigger(1);
    if(button.rose()) granular.trigger(0);

    synth.structure(ui.get(6));
    synth.brightness(ui.get(7));
    synth.damping(ui.get(8));
    synth.position(ui.get(9));
    synth.note(ui.get(10));
    synth.cv_amp(ui.get(5));

    randomizer.t_rate(ui.get(4));
    randomizer.t_bias(ui.get(1));
    randomizer.t_jitter(ui.get(3));
    randomizer.x_spread(ui.get(0));
    randomizer.x_bias(ui.get(2));
    randomizer.x_steps(ui.get(1));

    granular.position(ui.get(11));
    granular.density(ui.get(12));
    granular.pitch(ui.get(13) * 48.0f);
    granular.dry_wet(ui.get(14));
    granular.size(ui.get(15));
    granular.texture(ui.get(16));
    granular.feedback(ui.get(17));
    granular.reverb(ui.get(18));
    granular.stereo_spread(ui.get(19));
  AudioInterrupts();
  //Serial.printf("Temp: %1.3f\tProc: %1.3f\n", tempmonGetTemp(), AudioProcessorUsage());
}