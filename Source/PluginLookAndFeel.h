#pragma once
#include <JuceHeader.h>

class FazedLookAndFeel : public juce::LookAndFeel_V4 {

public:
    FazedLookAndFeel() {
        setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
            juce::Colour(0.52f, 0.07f, 0.15f, 1.0f));
        setColour(juce::Slider::ColourIds::trackColourId,
            juce::Colour(0.18f, 0.22f, 0.20f, 1.0f));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
            juce::Colour(0.18f, 0.42f, 0.60f, 1.0f));
    }

    void drawRotarySlider(
        juce::Graphics &g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider &slider) {

        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = radius * 0.1f;
        auto arcRadius = radius - lineW * 0.5f;

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(
            bounds.getCentreX(), bounds.getCentreY(),
            arcRadius, arcRadius,
            0.0f,
            rotaryStartAngle, rotaryEndAngle,
            true
        );

        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path valueArc;
        valueArc.addCentredArc(
            bounds.getCentreX(), bounds.getCentreY(),
            arcRadius, arcRadius,
            0.0f,
            rotaryStartAngle, toAngle,
            true);

        g.setColour(findColour(juce::Slider::ColourIds::rotarySliderFillColourId).withBrightness(0.5f + 0.5f * sliderPos));
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(findColour(juce::Slider::ColourIds::rotarySliderFillColourId).withBrightness(1.0f));
        g.setFont(juce::Font("Geneva", 0.4 * (sqrtf(bounds.getHeight()) + (0.5 * bounds.getHeight())), juce::Font::FontStyleFlags::plain));
        g.drawText(slider.getTextFromValue(slider.getValue()), bounds, juce::Justification::centred, false);
        g.setFont(bounds.getHeight() / 5.0f);
        g.drawText(slider.getName(), bounds, juce::Justification::centredBottom, false);
    }
};
