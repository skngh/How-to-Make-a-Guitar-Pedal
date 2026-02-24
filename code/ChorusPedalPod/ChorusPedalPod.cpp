#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

#define POT1 A6
#define POT2 A0
#define SWITCH D27

enum AdcChannel
{
    KnobOne,
    KnobTwo,
    NUM_ADC_CHANNELS
};

DaisySeed hw;

void InitializeADC();
void ProcessADC();

bool bypass = true;

OnePole lpf;
Chorus chorus;
float feedback = 0.01f;
float delayMs = 15.0f;
float rate = 0.8f;
float depth = 0.3f;
float cutoff = 10000.0f;

Switch footswitch;

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
    float sig;

    ProcessADC();

    for (size_t i = 0; i < size; i++)
    {
        sig = in[0][i];
        sig = lpf.Process(sig);
        sig = chorus.Process(sig);
        if (bypass)
            out[0][i] = in[0][i];
        else
            out[0][i] = sig;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartAudio(AudioCallback);
    float sampleRate = hw.AudioSampleRate();

    InitializeADC();

    chorus.Init(sampleRate);
    lpf.Init();
    chorus.SetFeedback(feedback);
    chorus.SetDelayMs(delayMs);
    chorus.SetLfoDepth(depth);
    chorus.SetLfoFreq(rate);

    lpf.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf.SetFrequency(cutoff);

    while (1)
    {
    }
}

void InitializeADC()
{
    AdcChannelConfig adcChannelConfig[NUM_ADC_CHANNELS];
    adcChannelConfig[KnobOne].InitSingle(POT1);
    adcChannelConfig[KnobTwo].InitSingle(POT2);

    hw.SetLed(true);

    footswitch.Init(SWITCH);

    hw.adc.Init(adcChannelConfig, NUM_ADC_CHANNELS);
    hw.adc.Start();
}

void ProcessADC()
{
    footswitch.Debounce();

    bypass ^= footswitch.RisingEdge();
    hw.SetLed(!bypass);

    rate = fmap(hw.adc.GetFloat(KnobOne), 0.1f, 5.0f, Mapping::EXP);
    depth = fmap(hw.adc.GetFloat(KnobTwo), 0.0f, 1.0f, Mapping::EXP);

    chorus.SetLfoFreq(rate);
    chorus.SetLfoDepth(depth);
}
