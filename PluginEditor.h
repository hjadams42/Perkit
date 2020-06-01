/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PerkitAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    PerkitAudioProcessorEditor (PerkitAudioProcessor&);
    ~PerkitAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PerkitAudioProcessor& processor;
    void TrigSender();
    void Bypasser(bool state);
    void sliderValueChanged (Slider* slider) override;
    //void buttonClicked (Button* button) override;
    //void buttonStateChanged(Button* button) override;
    Slider dist_slider[3]; //noise, amp, thresh
    Label dist_label[3];
    Slider filt_slider[2]; //decay, cutoff
    Label filt_label[2];
    TextButton triggerButton;
    Label triggerLabel;
    TextButton bypassButton;
    Label bypassLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PerkitAudioProcessorEditor)
};
