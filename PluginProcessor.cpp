/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PerkitAudioProcessor::PerkitAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (noise = new AudioParameterFloat ("noise", // parameter ID
                                                  "Noise", // parameter name
                                                  0.0f,   // minimum value
                                                  1.0f,   // maximum value
                                                  0.0f)); // default
    addParameter (amp = new AudioParameterFloat ("amp", // parameter ID
                                                 "Amp", // parameter name
                                                 1.0f,   // minimum value
                                                 10.0f,   // maximum value
                                                 2.0f)); //default
    addParameter (fold = new AudioParameterFloat ("fold", // parameter ID
                                                 "Fold", // parameter name
                                                 0.01f,   // minimum value
                                                 5.0f,   // maximum value
                                                 0.5f)); //default
    addParameter (freqMax = new AudioParameterFloat ("freqMax", // parameter ID
                                                 "FreqMax", // parameter name
                                                 1.0f,   // minimum value
                                                 10.0f,   // maximum value
                                                 2.0f)); //default
    addParameter (decay = new AudioParameterFloat ("decay", // parameter ID
                                                   "Decay", // parameter name
                                                   0.1f,   // minimum value
                                                   5.0f,   // maximum value
                                                   0.5f)); //default    
}

PerkitAudioProcessor::~PerkitAudioProcessor()
{
}

//==============================================================================
const String PerkitAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PerkitAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PerkitAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PerkitAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PerkitAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PerkitAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PerkitAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PerkitAudioProcessor::setCurrentProgram (int index)
{
}

const String PerkitAudioProcessor::getProgramName (int index)
{
    return {};
}

void PerkitAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PerkitAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PerkitAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PerkitAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void PerkitAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    filter.set_fs(getSampleRate());
    
    MidiMessage m;
    int time;
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent(m, time);)
    {
        if (m.isNoteOn())
        {
            filter.set_status(1);
        }
    }
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
        const float* pin = buffer.getReadPointer (channel);
        float* output = buffer.getWritePointer (channel);
        int num_frame = buffer.getNumSamples();
        /* make a copy of input buffer, make additional buffers for intermediate steps */
        float input[FRAMES_PER_BUFFER];
        float out_one[FRAMES_PER_BUFFER];
        float noise_buf[FRAMES_PER_BUFFER];
        float disto[FRAMES_PER_BUFFER];
        for (int i=0; i<num_frame; i++)
            input[i] = pin[i];
            
        /* stages of processing */
        
        //noise
        for (int i=0; i<num_frame; i++)
            noise_buf[i] = random.nextFloat()-0.5;
        
        distorter.mix_with_noise(input, out_one, noise_buf, num_frame); //add noise
        //future work: upsample here
        //distortion
        distorter.fold_distorter(out_one, disto, num_frame); //distort from noisy to disto
        //future work: downsample here
        //lowpass filter with cutoff linked to amplitude envelope
        filter.filter_gate(disto, output, num_frame, channel); //filter from disto to output
        
    }
}

//==============================================================================
bool PerkitAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PerkitAudioProcessor::createEditor()
{
    return new PerkitAudioProcessorEditor (*this);
}

//==============================================================================
void PerkitAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PerkitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PerkitAudioProcessor();
}
