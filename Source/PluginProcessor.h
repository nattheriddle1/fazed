#pragma once
#include <random>
#include <JuceHeader.h>
#include <JucePluginDefines.h>

class FazedAudioProcessor :
    public juce::AudioProcessor,
    public juce::AudioProcessorParameter::Listener {

public:
    FazedAudioProcessor() :
        juce::AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        ) {
        addParameter(delay_mantissa_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("delay_mantissa", 1), "Mantissa",
            delay_mantissa_minimum, delay_mantissa_maximum, delay_mantissa_default));
        addParameter(delay_exponent_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("delay_exponent", 1), "Exponent",
            delay_exponent_minimum, delay_exponent_maximum, delay_exponent_default));
        addParameter(noise_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("noise", 1), "Noise",
            noise_minimum, noise_maximum, noise_default));
        addParameter(lfo_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("lfo", 1), "LFO",
            lfo_minimum, lfo_maximum, lfo_default));
        addParameter(voices_parameter = new juce::AudioParameterInt(
            juce::ParameterID("voices", 1), "Voices",
            voices_minimum, voices_maximum, voices_default));
    };

    ~FazedAudioProcessor() override {
    };

    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
        cache.setSize(2, sampleRate);
        cache.clear();
    };
    void releaseResources() override {
    };

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused(layouts);
        return true;
#else
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;
#if ! JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
#endif
        return true;
#endif
    };
#endif

    int mod(int dividend, int divisor) {
        return ((dividend % divisor) + divisor) % divisor;
    };

    int cycle(int delta) {
        return mod(cache.getNumSamples() + delta, cache.getNumSamples());
    }

    void processBlock(juce::AudioBuffer<float> &audio_buffer, juce::MidiBuffer &midi_buffer) override {
        for (int channel = 0; channel < audio_buffer.getNumChannels(); channel++) {
            int channel_position = cache_position;
            float *buffer_pointer = audio_buffer.getWritePointer(channel);
            float *cache_pointer = cache.getWritePointer(channel);

            for (int i = 0; i < audio_buffer.getNumSamples(); i++) {
                cache_pointer[cycle(channel_position)] = buffer_pointer[i];
                for (int voice = voices_minimum; voice <= *voices_parameter; voice++) {
                    coef = cycle((
                        powf(*delay_mantissa_parameter * abs(buffer_pointer[i]), *delay_exponent_parameter) * \
                        (1.0f + powf(*delay_mantissa_parameter, 1.0f-*delay_exponent_parameter))) + \
                        (*noise_parameter * (rand() / (float) RAND_MAX)) + \
                        (*lfo_parameter * (i % 2))
                    ) / voice;
                    buffer_pointer[i] += cache_pointer[cycle(channel_position - coef)];
                    buffer_pointer[i] /= 2;
                }
                channel_position = cycle(channel_position + 1);
            }
        }
        cache_position = cycle(cache_position + audio_buffer.getNumSamples());
    };

    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override {
        return true;
    };
    const juce::String getName() const override {
        return JucePlugin_Name;
    };
    bool acceptsMidi() const override {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    };
    bool producesMidi() const override {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    };
    bool isMidiEffect() const override {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    };
    double getTailLengthSeconds() const override {
        return 0.0;
    };
    int getNumPrograms() override {
        return 1;
    };
    int getCurrentProgram() override {
        return 0;
    };
    void setCurrentProgram(int index) override {
    };
    const juce::String getProgramName(int index) override {
        return {};
    };
    void changeProgramName(int index, const juce::String &newName) override {
    };

    void getStateInformation(juce::MemoryBlock &destData) override {
        juce::MemoryOutputStream stream(destData, true);
        stream.writeFloat(delay_mantissa_parameter->get());
        stream.writeFloat(delay_exponent_parameter->get());
        stream.writeFloat(noise_parameter->get());
        stream.writeFloat(lfo_parameter->get());
        stream.writeInt(voices_parameter->get());
        stream.flush();
    };
    void setStateInformation(const void *data, int sizeInBytes) override {
        juce::MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);
        *delay_mantissa_parameter = stream.readFloat();
        *delay_exponent_parameter = stream.readFloat();
        *noise_parameter = stream.readFloat();
        *lfo_parameter = stream.readFloat();
        *voices_parameter = stream.readInt();
    };

    void parameterValueChanged(int parameterIndex, float newValue) override {
    };
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
    };

    juce::AudioParameterFloat *delay_mantissa_parameter;
    const float delay_mantissa_minimum = 0;
    const float delay_mantissa_maximum = 2048;
    const float delay_mantissa_interval = 0.1f;
    const float delay_mantissa_default = 0;

    juce::AudioParameterFloat *delay_exponent_parameter;
    const float delay_exponent_minimum = 0.01f;
    const float delay_exponent_maximum = 1.0f;
    const float delay_exponent_interval = 0.01f;
    const float delay_exponent_default = 1.0f;

    juce::AudioParameterFloat *noise_parameter;
    const float noise_minimum = 0;
    const float noise_maximum = 50;
    const float noise_interval = 0.1;
    const float noise_default = 0;

    juce::AudioParameterFloat *lfo_parameter;
    const float lfo_minimum = 0;
    const float lfo_maximum = 50;
    const float lfo_interval = 0.1;
    const float lfo_default = 0;
        
    juce::AudioParameterInt *voices_parameter;
    const int voices_minimum = 1;
    const int voices_maximum = 10;
    const int voices_interval = 1;
    const int voices_default = 1;

    int coef = 0;

private:
    juce::AudioBuffer<float> cache;
    int cache_position = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FazedAudioProcessor)
};
