#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>

Lfo   lfo;
MultiStageEnvelope env;
AudioSynthWaveformModulated osc;
Braids osc2;
AudioOutputI2S  dac;
AudioAmplifier amp;
AudioEffectMultiply amp2;
AudioMixer4 mixer;

AudioConnection con3(lfo, 0, osc2, 0);
//AudioConnection con4(amp, 0, osc, 0);
AudioConnection con5(osc, 0, amp2, 0);
AudioConnection con6(env, 0, amp2, 1);
AudioConnection con7(amp2, 0, mixer, 0);
AudioConnection con8(osc2, 0, mixer, 1);
AudioConnection con1(mixer, 0, dac, 0);
AudioConnection con2(mixer, 0, dac, 1);

elapsedMillis timer = 0;
bool on = 0;

void setup() {
  AudioNoInterrupts();
  Serial.begin(115200);
  while(!Serial) {}
  AudioInterrupts();
  AudioMemory(12);
  amp.gain(0.1);
  osc.begin(0.4, 100, WAVEFORM_SINE);
  mixer.gain(0, 0.5);
  mixer.gain(1, 0.1);
}

void loop() {
  osc2.timbre(analogRead(14) << 5);
  if(timer >= 10000) {
    if(!on) {
      env.trigger();
      on = 1;
    }
    else {
      env.decay();
      on = 0;
    }
    timer = 0; 
  }
  Serial.print(AudioProcessorUsage());
  Serial.print("\t");
  Serial.println(AudioMemoryUsage());
}