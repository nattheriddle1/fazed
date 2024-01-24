#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginLookAndFeel.h"

class FazedAudioProcessorEditor :
    public juce::AudioProcessorEditor,
    public juce::Slider::Listener,
    public juce::Timer {

public:
    FazedAudioProcessorEditor(FazedAudioProcessor &p) :
        AudioProcessorEditor(&p), audioProcessor(p) {
        startTimerHz(30);
        setLookAndFeel(&lookAndFeel);
        setSize(310, 500);

        addAndMakeVisible(delay_mantissa_slider);
        delay_mantissa_slider.setRange(p.delay_mantissa_minimum, p.delay_mantissa_maximum, p.delay_mantissa_interval);
        delay_mantissa_slider.addListener(this);
        delay_mantissa_slider.setName(p.delay_mantissa_parameter->getName(20));
        delay_mantissa_slider.setDoubleClickReturnValue(true, p.delay_mantissa_default);
        delay_mantissa_slider.setRotaryParameters((8.0f / 6.0f) * M_PI, (16.0f / 6.0f) * M_PI, true);
        delay_mantissa_slider.setSkewFactor(0.5, false);

        addAndMakeVisible(delay_exponent_slider);
        delay_exponent_slider.setRange(p.delay_exponent_minimum, p.delay_exponent_maximum, p.delay_exponent_interval);
        delay_exponent_slider.addListener(this);
        delay_exponent_slider.setName(p.delay_exponent_parameter->getName(20));
        delay_exponent_slider.setDoubleClickReturnValue(true, p.delay_exponent_default);
        delay_exponent_slider.setRotaryParameters((8.0f / 6.0f) * M_PI, (16.0f / 6.0f) * M_PI, true);

        addAndMakeVisible(noise_slider);
        noise_slider.setRange(p.noise_minimum, p.noise_maximum, p.noise_interval);
        noise_slider.addListener(this);
        noise_slider.setName(p.noise_parameter->getName(20));
        noise_slider.setDoubleClickReturnValue(true, p.noise_default);
        noise_slider.setRotaryParameters((8.0f / 6.0f) * M_PI, (16.0f / 6.0f) * M_PI, true);

        addAndMakeVisible(lfo_slider);
        lfo_slider.setRange(p.lfo_minimum, p.lfo_maximum, p.lfo_interval);
        lfo_slider.addListener(this);
        lfo_slider.setName(p.lfo_parameter->getName(20));
        lfo_slider.setDoubleClickReturnValue(true, p.lfo_default);
        lfo_slider.setRotaryParameters((8.0f / 6.0f) * M_PI, (16.0f / 6.0f) * M_PI, true);

        addAndMakeVisible(voices_slider);
        voices_slider.setRange(p.voices_minimum, p.voices_maximum, p.voices_interval);
        voices_slider.addListener(this);
        voices_slider.setName(p.voices_parameter->getName(20));
        voices_slider.setDoubleClickReturnValue(true, p.voices_default);
        voices_slider.setRotaryParameters((8.0f / 6.0f) * M_PI, (16.0f / 6.0f) * M_PI, true);
    };

    ~FazedAudioProcessorEditor() override {
        setLookAndFeel(nullptr);
    };

    void paint(juce::Graphics &g) override {
        g.setGradientFill(
            juce::ColourGradient(
            findColour(juce::ResizableWindow::backgroundColourId),
            juce::Point(0.0f, 0.33f * getHeight()),
            findColour(juce::ResizableWindow::backgroundColourId).darker(0.1f),
            juce::Point((float) getWidth(), 0.67f * getHeight()),
            true
        )
        );
        g.fillAll();
    };

    void resized() override {
        noise_slider.setBounds(30, 30, 80, 80);
        lfo_slider.setBounds(120, 30, 80, 80);
        voices_slider.setBounds(210, 30, 80, 80);
        delay_exponent_slider.setBounds(110, 119, 100, 100);
        delay_mantissa_slider.setBounds(30, 225, 250, 250);
    };

    void timerCallback() override {
        noise_slider.setValue(audioProcessor.noise_parameter->get(),
            juce::NotificationType::dontSendNotification);
        lfo_slider.setValue(audioProcessor.lfo_parameter->get(),
            juce::NotificationType::dontSendNotification);
        voices_slider.setValue(audioProcessor.voices_parameter->get(),
            juce::NotificationType::dontSendNotification);
        delay_exponent_slider.setValue(audioProcessor.delay_exponent_parameter->get(),
            juce::NotificationType::dontSendNotification);
        delay_mantissa_slider.setValue(audioProcessor.delay_mantissa_parameter->get(),
            juce::NotificationType::dontSendNotification);
    }

    void sliderValueChanged(juce::Slider *slider) override {
        if (slider == &noise_slider) {
            audioProcessor.noise_parameter->setValueNotifyingHost(
                audioProcessor.noise_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &lfo_slider) {
            audioProcessor.lfo_parameter->setValueNotifyingHost(
                audioProcessor.lfo_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &voices_slider) {
            audioProcessor.voices_parameter->setValueNotifyingHost(
                audioProcessor.voices_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &delay_exponent_slider) {
            audioProcessor.delay_exponent_parameter->setValueNotifyingHost(
                audioProcessor.delay_exponent_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &delay_mantissa_slider) {
            audioProcessor.delay_mantissa_parameter->setValueNotifyingHost(
                audioProcessor.delay_mantissa_parameter->convertTo0to1(slider->getValue()));
        }
    }

private:
    juce::Slider delay_mantissa_slider{ juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox };
    juce::Slider delay_exponent_slider{ juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox };
    juce::Slider noise_slider{ juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox };
    juce::Slider lfo_slider{ juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox };
    juce::Slider voices_slider{ juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox };
    FazedLookAndFeel lookAndFeel;
    FazedAudioProcessor &audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FazedAudioProcessorEditor)
};
