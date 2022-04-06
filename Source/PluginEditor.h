/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RMandAMDistortionAndPulserEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:
    RMandAMDistortionAndPulserEditor (RMandAMDistortionAndPulser&);
    ~RMandAMDistortionAndPulserEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RMandAMDistortionAndPulser& audioProcessor;
    
    enum modTypes
    {
        am = 1,
        
        rm
    };
    
    juce::Slider mAmRmSlider;
    juce::Label mAmRmLabel;
    juce::ComboBox mAmRmSwitch;
    
    juce::Slider mOverdriveSlider;
    juce::ComboBox mDistTypeBox;
    juce::Label mDistortionLabel;
    
    juce::Slider mPulserLfoSlider;
    juce::Label mPulserLfoLabel;
    
    double linearToExp(double value);
    double expToLinear(double value);
    
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RMandAMDistortionAndPulserEditor)
};
