/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// make this global so both the processor and editor can use it

enum distType
{
    soft = 1,
    hard,
    none
}; 

//==============================================================================
/**
*/
class RMandAMDistortionAndPulser  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RMandAMDistortionAndPulser();
    ~RMandAMDistortionAndPulser() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    double mSampleRate;
    
    double mAmRmSliderValue;
    double mAmRmAngleDelta;
    double mAmRmCurrentAngle[2];
    bool mAmRmFlag;
    
    double mPulserLfoSliderValue;
    double mPulserAngleDelta;
    double mPulserCurrentAngle[2];
    
    distType mDistTypeBoxValue;
    double mOverdriveSliderValue;

private:
    //==============================================================================
    int getSign(double value);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RMandAMDistortionAndPulser)
};
