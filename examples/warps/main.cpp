#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include <ResponsiveAnalogRead.h>

ResponsiveAnalogRead pot;

AudioSynthWaveform  osc, osc2;
Warps               processor;
AudioOutputI2S      dac;
AudioAmplifier      amp1, amp2;

AudioConnection con5(osc, 0, processor, 1);
AudioConnection con6(osc2, 0, processor, 0);
AudioConnection con(processor, 0, amp1, 0);
AudioConnection con2(processor, 1, amp2, 0);
AudioConnection con3(amp1, 0, dac, 0);
AudioConnection con4(amp2, 0, dac, 1);

elapsedMillis timer = 0;
uint8_t counter = 0;

void setup() {
  analogReference(0);
  pot.begin(15, false);
  AudioNoInterrupts();
  osc.begin(WAVEFORM_SINE);
  osc.frequency(220);
  osc.amplitude(0.1);

  osc2.begin(WAVEFORM_SINE);
  osc2.frequency(440);
  osc2.amplitude(0.1);

  amp1.gain(0.3);
  amp2.gain(0.3);
  Serial.begin(115200);
  while(!Serial) {}
  AudioInterrupts();

  AudioMemory(200);
}

void loop() {
  if(timer >= 1000 && random(10) < 1 ) {
    delay(30);
    osc.frequency(random(120));
    osc2.frequency(random(60) * random(10));
    timer = 0;
  }
  Serial.println(AudioProcessorUsage());
}