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

void InitailizeADC();
void ProcessADC();

bool hardClip = true;

Switch footswitch;
float distortionAmount = 1.0f

    void
    AudioCallback(AudioHandle::InputBuffer in,
                  AudioHandle::OutputBuffer out,
                  size_t size)
{
    float distortedSignal;

    ProcessADC();

    for (size_t i = 0; i < size; i++)
    {
        distortedSignal = in[0][i] * distortionAmount;

        if (distortedSignal >= 1.0f)
            distortedSignal = hardClip ? 1.0f : 2.0f / 3.0f;
        else if (distortedSignal <= -1.0f)
            distortedSignal = hardClip ? -1.0f : -2.0f / -3.0f;
        else
        {
            float cubedSignal = distortedSignal * distortedSignal * distortedSignal;
            distortedSignal = hardClip ? distortedSignal : distortedSignal - (cubedSignal / 3);
        }
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartAudio(AudioCallback);
    float sampleRate = hw.AudioSampleRate();

    InitailizeADC();

    while (1)
    {
    }
}

void InitailizeADC()
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
    hardClip ^= footswitch.RisingEdge();

    distortionAmount = fmap(hw.adc.GetFloat(KnobOne), 1.0f, 20.0f, Mapping::EXP);
}
