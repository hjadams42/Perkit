/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define SLIDER_SPACING  190
#define SLIDER_WIDTH    100
#define PANEL_HEIGHT    600
#define PANEL_WIDTH     600
#define V_MARGIN        50
#define H_MARGIN        60
//==============================================================================
PerkitAudioProcessorEditor::PerkitAudioProcessorEditor (PerkitAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (PANEL_WIDTH, PANEL_HEIGHT);
    
    
    //two buttons
    
    bypassButton.setClickingTogglesState(true);
    addAndMakeVisible(bypassButton);
    bypassButton.onClick = [this] { Bypasser (bypassButton.getState());   };
    //bypassButton.addListener(this);
    bypassLabel.setText("Bypass Envelope", dontSendNotification);
    bypassLabel.attachToComponent(&bypassButton, false);
    addAndMakeVisible(bypassLabel);
    
    addAndMakeVisible(triggerButton);
    //triggerButton.addListener(this);
    triggerButton.onClick = [this] { TrigSender(); };
    triggerLabel.setText("Trigger Envelope", dontSendNotification);
    triggerLabel.attachToComponent(&triggerButton, false);
    addAndMakeVisible(triggerLabel);
    
    // two sliders related to envelope
    filt_slider[0].setSliderStyle (Slider::LinearBarVertical);
    filt_slider[0].setRange(60.0, 20000.);
    filt_slider[0].setTextBoxStyle(Slider::TextBoxBelow, true, SLIDER_SPACING, 20);
    filt_slider[0].setValue(8000);
    filt_slider[0].setSkewFactorFromMidPoint (500);
    filt_label[0].setText("Freq", dontSendNotification);
    
    filt_slider[1].setSliderStyle (Slider::LinearBarVertical);
    filt_slider[1].setRange(0.1, 5.);
    filt_slider[1].setTextBoxStyle(Slider::TextBoxBelow, true, SLIDER_SPACING, 20);
    filt_slider[1].setValue(0.5);
    filt_slider[1].setSkewFactorFromMidPoint (1.);
    filt_label[1].setText("Decay", dontSendNotification);
    
    for (int i=0; i<2; i++)
    {
        addAndMakeVisible(&filt_slider[i]);
        filt_label[i].attachToComponent(&filt_slider[i], false);
        addAndMakeVisible(filt_label[i]);
        filt_slider[i].addListener(this);
    }
    
    // three distortion knobs
    for (int i=0; i<3; i++)
    {
        dist_slider[i].setSliderStyle (Slider::RotaryHorizontalDrag);
        dist_slider[i].setTextBoxStyle(Slider::TextBoxBelow, true, SLIDER_SPACING, 20);
    }
    
    dist_slider[0].setRange(0., 1.);
    dist_slider[0].setValue(0.);
    dist_slider[0].setSkewFactorFromMidPoint(0.3);
    dist_label[0].setText("Noise", dontSendNotification);
    
    dist_slider[1].setRange(1.0, 10.);
    dist_slider[1].setValue(2.);
    dist_slider[1].setSkewFactorFromMidPoint (3.);
    dist_label[1].setText("Gain", dontSendNotification);
    
    dist_slider[2].setRange(0.01, 5.);
    dist_slider[2].setValue(0.5);
    dist_slider[2].setSkewFactorFromMidPoint (1.);
    dist_label[2].setText("Folding", dontSendNotification);
    
    for (int i=0; i<3; i++)
       {
           addAndMakeVisible(&dist_slider[i]);
           dist_label[i].attachToComponent(&dist_slider[i], false);
           addAndMakeVisible(dist_label[i]);
           dist_slider[i].addListener(this);
       }
    
    getLookAndFeel().setColour(Label::textColourId, Colours::black);
    getLookAndFeel().setColour(Slider::textBoxTextColourId, Colours::black);
    //getLookAndFeel().setColour(ToggleButton::buttonColourId, Colours::blue);
    //getLookAndFeel().setColour(ToggleButton::buttonOnColourId, Colours::darkgreen);
}

PerkitAudioProcessorEditor::~PerkitAudioProcessorEditor()
{
}

//==============================================================================
void PerkitAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    Image background = ImageCache::getFromMemory (BinaryData::perkit_img_jpg, BinaryData::perkit_img_jpgSize);
    g.drawImageAt (background, 0, 0);
    g.setColour (Colours::darkblue);
    g.setFont (30.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
    g.drawText ("PERKIT", 0, PANEL_HEIGHT*2/3., PANEL_WIDTH/3., PANEL_HEIGHT/3-V_MARGIN, Justification::centred);
    
}

void PerkitAudioProcessorEditor::resized()
{
    for (int i=0; i<3; i++)
    {
        dist_slider[i].setBounds(H_MARGIN+SLIDER_SPACING*i, V_MARGIN, SLIDER_WIDTH, PANEL_HEIGHT/3.-V_MARGIN*2);
    }
    for (int i=0; i<2; i++)
    {
        filt_slider[i].setBounds(H_MARGIN + SLIDER_SPACING*(i+1), PANEL_HEIGHT/3.+V_MARGIN, SLIDER_WIDTH/3., 2*PANEL_HEIGHT/3.-V_MARGIN);
    }
    
    bypassButton.setBounds(H_MARGIN, PANEL_HEIGHT/3. + V_MARGIN, SLIDER_WIDTH/2, SLIDER_WIDTH/2);
    triggerButton.setBounds(H_MARGIN+SLIDER_SPACING/2., PANEL_HEIGHT/3. + V_MARGIN, SLIDER_WIDTH/2., SLIDER_WIDTH/2.);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void PerkitAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
     if (slider == &filt_slider[0])
     {
         processor.filter.set_freq(slider->getValue());
     }
    else if (slider == &filt_slider[1])
    {
        processor.filter.set_decay(slider->getValue());
    }
    else if (slider == &dist_slider[0])
    {
        processor.distorter.set_noise(slider->getValue());
    }
    else if (slider == &dist_slider[1])
    {
        processor.distorter.set_amp(slider->getValue());
    }
    else if (slider == &dist_slider[2])
    {
        processor.distorter.set_thresh(slider->getValue());
    }
}

void PerkitAudioProcessorEditor::TrigSender()
{
    processor.filter.set_status(1);
}

void PerkitAudioProcessorEditor::Bypasser(bool state)
{
    if (state)
    {
        processor.filter.set_status(3);
    }
    else
    {
        processor.filter.set_status(-1);
    }
}
