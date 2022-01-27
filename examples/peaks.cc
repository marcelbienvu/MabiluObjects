#include <Arduino.h>
#include <Audio.h>
#include <MabiluObjects.h>
#include <ResponsiveAnalogRead.h>
#include <CD74HC4067.h>

DMAMEM uint16_t reverb_buffer[ELEMENTS_BUFFER_SIZE];

CD74HC4067 mux(8, 9, 10, 11);
ResponsiveAnalogRead pot, pot2, pot3;

Lfo                 lfo;
AudioSynthWaveformModulated  osc, osc2;
Warps               processor;
AudioOutputI2S      dac;
Elements            resonator(reverb_buffer);
AudioAmplifier      amp1, amp2, amp3;

AudioConnection con5(lfo, 0, amp3, 0);
AudioConnection con10(amp3, 0, osc, 0);
AudioConnection con15(osc, 0, resonator, 0);
AudioConnection con7(resonator, 0, processor, 1);
AudioConnection con6(osc, 0, processor, 0);
AudioConnection con(processor, 0, amp1, 0);
AudioConnection con2(processor, 1, amp2, 0);
AudioConnection con3(amp1, 0, dac, 0);
AudioConnection con4(amp2, 0, dac, 1);

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

uint8_t counter = 0;

void setup() {
  pot.begin(0, true);
  pot2.begin(0, true);
  pot3.begin(0, true);
  delay(2000);

  AudioNoInterrupts();
  osc.begin(WAVEFORM_SINE);
  osc.frequency(220);
  osc.amplitude(0.1);
  amp3.gain(0.1);
  osc2.begin(WAVEFORM_SINE);
  osc2.frequency(440);
  osc2.amplitude(0.1);

  amp1.gain(0.6f);
  amp2.gain(0.6f);
  Serial.begin(115200);
  AudioInterrupts();

  AudioMemory(200);
}

void loop() {
  mux.channel(0);
  delayMicroseconds(20);
  pot.update(analogRead(14));
  processor.channel_drive(0, pot.getValue() / 1023.0);
  mux.channel(1);
  delayMicroseconds(20);
  pot2.update(analogRead(14));
  lfo.set_rate(pot2.getValue() << 6);
  mux.channel(2);
  delayMicroseconds(20);
  pot3.update(analogRead(14));
  lfo.set_shape(pot3.getValue() << 6);

  if(timer >= 1000 && random(20) < 1 ) {
    resonator.gate(0);
    delay(30);
//    resonator.note(chords[0][random(11)]);
    osc2.frequency(random(60) * random(10));
    resonator.gate(1);
    timer = 0;
  }
  Serial.println(AudioProcessorUsage());
}