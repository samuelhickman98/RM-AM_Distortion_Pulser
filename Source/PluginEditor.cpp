/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RMandAMDistortionAndPulserEditor::RMandAMDistortionAndPulserEditor (RMandAMDistortionAndPulser& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    mAmRmSwitch.addItem("AM", 1);
    mAmRmSwitch.addItem("RM", 2);
    mAmRmSwitch.setSelectedId(rm);
    addAndMakeVisible(&mAmRmSwitch);
    mAmRmSwitch.addListener(this);
    
    mAmRmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mAmRmSlider.setRange(0.0f, 1000.0f);
    mAmRmSlider.setValue(audioProcessor.mPulserLfoSliderValue);
    addAndMakeVisible(&mAmRmSlider);
    mAmRmSlider.addListener(this);
    
    addAndMakeVisible(&mAmRmLabel);
    mAmRmLabel.setText("AM / RM", juce::dontSendNotification);
    mAmRmLabel.attachToComponent(&mAmRmSlider, true);
    mAmRmLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
    mAmRmLabel.setJustificationType(juce::Justification::right);
    
    mOverdriveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mOverdriveSlider.setRange(1, 10);
    mOverdriveSlider.setValue(audioProcessor.mOverdriveSliderValue);
    addAndMakeVisible(&mOverdriveSlider);
    mOverdriveSlider.addListener(this);
    
    addAndMakeVisible(&mDistortionLabel);
    mDistortionLabel.setText("Distortion", juce::dontSendNotification);
    mDistortionLabel.attachToComponent(&mOverdriveSlider, true);
    mDistortionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mDistortionLabel.setJustificationType(juce::Justification::right);
    
    mDistTypeBox.addItem("Soft", 1);
    mDistTypeBox.addItem("Hard", 2);
    mDistTypeBox.setSelectedId(soft);
    addAndMakeVisible(&mDistTypeBox);
    mDistTypeBox.addListener(this);
    
    mPulserLfoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mPulserLfoSlider.setRange(0.0f, 20.0f);
    mPulserLfoSlider.setValue(audioProcessor.mPulserLfoSliderValue);
    addAndMakeVisible(&mPulserLfoSlider);
    mPulserLfoSlider.addListener(this);
    
    addAndMakeVisible(&mPulserLfoLabel);
    mPulserLfoLabel.setText("Amp Pulser", juce::dontSendNotification);
    mPulserLfoLabel.attachToComponent(&mPulserLfoSlider, true);
    mPulserLfoLabel.setColour(juce::Label::textColourId, juce::Colours::magenta);
    mPulserLfoLabel.setJustificationType(juce::Justification::right);
    
    setResizable(true, true);
    setResizeLimits(250, 250, 850, 550);
    setSize (700, 500);
    
    std::printf("GUI constructor\n");
}

RMandAMDistortionAndPulserEditor::~RMandAMDistortionAndPulserEditor()
{
    mPulserLfoSlider.removeListener(this);
//     std::printf("GUI destructor\n");
}

//==============================================================================
void RMandAMDistortionAndPulserEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &mAmRmSlider)
    {
        audioProcessor.mAmRmSliderValue = mAmRmSlider.getValue();
        
        auto cyclesPerSample = audioProcessor.mAmRmSliderValue / audioProcessor.mSampleRate;
        
        audioProcessor.mAmRmAngleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::twoPi;
        
        std::printf("AM RM: %f\n", audioProcessor.mAmRmSliderValue);
    }
    else if(slider == &mPulserLfoSlider)
    {
    //    double cyclesPerSample;
        
        audioProcessor.mPulserLfoSliderValue = mPulserLfoSlider.getValue();
        
        auto cyclesPerSample = audioProcessor.mPulserLfoSliderValue / audioProcessor.mSampleRate;
        
        audioProcessor.mPulserAngleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::twoPi;
    }
    else if(slider == &mOverdriveSlider)
    {
        audioProcessor.mOverdriveSliderValue = mOverdriveSlider.getValue();
    }
}

void RMandAMDistortionAndPulserEditor::comboBoxChanged(juce::ComboBox *comboBox)
{
    switch (mAmRmSwitch.getSelectedId())
    {
        case am:
            audioProcessor.mAmRmFlag = true;
            break;
        case rm:
            audioProcessor.mAmRmFlag = false;
            break;
        default:
            break;
    }
    
    switch (mDistTypeBox.getSelectedId())
    {
        case soft:
            audioProcessor.mDistTypeBoxValue = soft;
            break;
        case hard:
            audioProcessor.mDistTypeBoxValue = hard;
        default:
            break;
    }
}

//==============================================================================
void RMandAMDistortionAndPulserEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::black);
    g.setFont (30.0f);
    g.drawFittedText ("AM/RM Ring Modulator, Distortion, and Amp Pulser", getLocalBounds(), juce::Justification::centred, 1);
}

void RMandAMDistortionAndPulserEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
   // mPulserLfoSlider.setBounds(getWidth() * 0.1, getHeight() * 0.4, getWidth() * 0.8, getHeight() * 0.6);
    
    mAmRmSlider.setBounds(100, 25, 300, 50);
    mAmRmSwitch.setBounds(150, 300, 75, 50);
    
    mOverdriveSlider.setBounds(200, 100, 300, 50);
    mDistTypeBox.setBounds(350, 300, 75, 50);
    
    mPulserLfoSlider.setBounds(300, 150, 300, 50);
}
