#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorEditor *FazedAudioProcessor::createEditor() {
    return new FazedAudioProcessorEditor(*this);
};

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new FazedAudioProcessor();
}
