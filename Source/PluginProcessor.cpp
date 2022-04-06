/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RMandAMDistortionAndPulser::RMandAMDistortionAndPulser()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mAmRmSliderValue = 100.0f;
    mAmRmFlag = true;
    mPulserLfoSliderValue = 10.0f;
    mOverdriveSliderValue = 5.0f;
}

RMandAMDistortionAndPulser::~RMandAMDistortionAndPulser()
{
}

int RMandAMDistortionAndPulser::getSign(double value)
{
    int sign;
    
    if(value > 0.0)
    {
        sign = 1;
    }
    else if (value < 0.0)
    {
        sign = -1;
    }
    else
    {
        sign = 0;
    }
    
    return sign;
}

//==============================================================================
const juce::String RMandAMDistortionAndPulser::getName() const
{
    return JucePlugin_Name;
}

bool RMandAMDistortionAndPulser::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RMandAMDistortionAndPulser::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RMandAMDistortionAndPulser::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RMandAMDistortionAndPulser::getTailLengthSeconds() const
{
    return 0.0;
}

int RMandAMDistortionAndPulser::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RMandAMDistortionAndPulser::getCurrentProgram()
{
    return 0;
}

void RMandAMDistortionAndPulser::setCurrentProgram (int index)
{
}

const juce::String RMandAMDistortionAndPulser::getProgramName (int index)
{
    return {};
}

void RMandAMDistortionAndPulser::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RMandAMDistortionAndPulser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mSampleRate = sampleRate;
    mPulserAngleDelta = 0.0f;
    mPulserCurrentAngle[0] = 0.0f;
    mPulserCurrentAngle[1] = juce::MathConstants<double>::pi;
    
    mAmRmCurrentAngle[0] = 0.0f;
    mAmRmCurrentAngle[1] = 0.0f;
}

void RMandAMDistortionAndPulser::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RMandAMDistortionAndPulser::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RMandAMDistortionAndPulser::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        double inputGain;
        
        inputGain = buffer.getMagnitude(channel, 0, buffer.getNumSamples());
        
        for(int i=0; i<buffer.getNumSamples(); i++)
        {
            double AmRmSignal;
            
            if(mAmRmSliderValue!= 0.0)
            {
                AmRmSignal = std::cos(mAmRmCurrentAngle[channel]);
                
                AmRmSignal += 1.0;
                AmRmSignal *= 0.5;
                
                channelData[i] *= AmRmSignal;
                
                mAmRmCurrentAngle[channel] += mAmRmAngleDelta;
                
                mAmRmCurrentAngle[channel] = std::fmod(mAmRmCurrentAngle[channel], juce::MathConstants<double>::twoPi);
                
            }
            
        }
        
        if(mDistTypeBoxValue != none)
            buffer.applyGain(channel, 0, buffer.getNumSamples(), mOverdriveSliderValue);
        
        for(int i=0; i<buffer.getNumSamples(); i++)
        {
            if(mDistTypeBoxValue==soft)
            {
                channelData[i] = std::tanh(channelData[i]);
            }
            else if(mDistTypeBoxValue==hard)
            {
                if(std::fabs(channelData[i]) > 1.0)
                    channelData[i] = getSign(channelData[i]);
            }
        }
        
        for(int i=0; i<buffer.getNumSamples(); i++)
        {
            double lfoSignal;
            
            if(mPulserLfoSliderValue == 0.0)
            
                lfoSignal = 1.0;
            
            else
                lfoSignal = std::cos(mPulserCurrentAngle[channel]);
            
            //need to offset and scale the LFO
            lfoSignal += 1.0;
            lfoSignal *= 0.5;
            
            // apply the LFO signal to the input
            channelData[i] *= lfoSignal;
        
            // imcrement and wrap the main mPulserCurrentAngle
            mPulserCurrentAngle[channel] += mPulserAngleDelta;
            mPulserCurrentAngle[channel] = std::fmod(mPulserCurrentAngle[channel], juce::MathConstants<double>::twoPi);
        }
    }
}

//==============================================================================
bool RMandAMDistortionAndPulser::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RMandAMDistortionAndPulser::createEditor()
{
    return new RMandAMDistortionAndPulserEditor (*this);
}

//==============================================================================
void RMandAMDistortionAndPulser::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RMandAMDistortionAndPulser::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RMandAMDistortionAndPulser();
}
