#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

#define LED D8
#define POT1 A1
#define POT2 A11
#define POT3 A10
#define FOOTSWITCH D13

bool bypass = true;

static MoogLadder lpf;
static Chorus chorus;
float feedback = 0.3;
float delayMs = 15;
float rate, depth, cutoff;

Switch footswitch;

void MyCallback(float **in, float **out, size_t size) {
  float wet;

  ProcessADC();

  for (size_t i = 0; i < size; i++) {
    wet = lpf.Process(chorus.Process(in[0][i]));


    out[0][i] = bypass ? in[0][i] : ((wet * 0.75) + (in[0][i] * 0.25));
  }
}

void setup() {
  float samplerate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();

  chorus.Init(samplerate);
  chorus.SetDelayMs(delayMs);
  chorus.SetFeedback(feedback);
  lpf.Init(samplerate);
  lpf.SetRes(0);


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  footswitch.Init(1000, true, FOOTSWITCH, INPUT_PULLUP);

  DAISY.begin(MyCallback);
}

void loop() {}
void ProcessADC()
{
  footswitch.Debounce();
  bypass ^= footswitch.RisingEdge();
  digitalWrite(LED, !bypass);

  rate = fmap(analogRead(POT1) / 1023.0, .1, 5, daisysp::Mapping::EXP);
  depth = fmap(analogRead(POT2) / 1023.0, 0, 1, daisysp::Mapping::EXP);
  cutoff = fmap(analogRead(POT3) / 1023.0, 500, 5000, daisysp::Mapping::LOG);

  chorus.SetLfoFreq(rate);
  chorus.SetLfoDepth(depth);
  lpf.SetFreq(cutoff);
}
