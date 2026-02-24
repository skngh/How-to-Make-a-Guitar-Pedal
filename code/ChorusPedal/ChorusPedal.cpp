#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;

enum AdcChannel
{
    knobOne,
    knobTwo,
    knobThree,
    NUM_ADC_CHANNELS
};

void InitializeADC();
void ProcessADC();

#define LED D8
#define POT1 A1
#define POT2 A11
#define POT3 A10
#define FOOTSWITCH D13

bool bypass = true;

OnePole lpf;
Chorus chorus;
float feedback = 0.3;
float delayMs = 15;
float rate, depth, cutoff;
GPIO led;

Switch footswitch;

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
    float wet;

    ProcessADC();

    for (size_t i = 0; i < size; i++)
    {
        wet = lpf.Process(chorus.Process(in[0][i]));

        if (bypass)
            out[0][i] = in[0][i];
        else
            out[0][i] = (wet * 0.75) + (in[0][i] * 0.25);
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    float sampleRate = hw.AudioSampleRate();

    InitializeADC();

    chorus.Init(sampleRate);
    chorus.SetFeedback(feedback);
    chorus.SetDelayMs(delayMs);

    lpf.Init();
    lpf.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf.SetFrequency(cutoff);

    hw.StartAudio(AudioCallback);

    while (1)
    {
    }
}

void InitializeADC()
{
    AdcChannelConfig adc_config[NUM_ADC_CHANNELS];
    adc_config[knobOne].InitSingle(POT1);
    adc_config[knobTwo].InitSingle(POT2);
    adc_config[knobThree].InitSingle(POT3);

    led.Init(LED, GPIO::Mode::OUTPUT, GPIO::Pull::NOPULL);
    footswitch.Init(FOOTSWITCH);
    hw.SetLed(!bypass);

    hw.adc.Init(adc_config, NUM_ADC_CHANNELS);
    hw.adc.Start();
}

void ProcessADC()
{
    footswitch.Debounce();
    bypass ^= footswitch.RisingEdge();
    led.Write(!bypass);

    rate = fmap(hw.adc.GetFloat(knobOne), .1, 5, daisysp::Mapping::EXP);
    depth = fmap(hw.adc.GetFloat(knobTwo), 0, 1, daisysp::Mapping::EXP);
    cutoff = fmap(hw.adc.GetFloat(knobThree), 500, 15000, daisysp::Mapping::LOG);

    chorus.SetLfoFreq(rate);
    chorus.SetLfoDepth(depth);
    lpf.SetFrequency(cutoff);
}
