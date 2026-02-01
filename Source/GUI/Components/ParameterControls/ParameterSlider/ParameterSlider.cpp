#include "ParameterSlider.h"

ParameterSlider::ParameterSlider(const Parameter &parameter,
                                 juce::AudioProcessorValueTreeState &apvts,
                                 juce::Component *component)
    : parameter(parameter), component(component) {

  if (juce::String(parameter.suffix) == "%") {
    slider = std::make_unique<PercentSlider>();
  } else {
    slider = std::make_unique<juce::Slider>();
  }

  slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
  slider->setColour(juce::Slider::textBoxOutlineColourId,
                    juce::Colours::transparentBlack);
  slider->setTextValueSuffix(parameter.suffix);

  sliderAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          apvts, parameter.id, *slider);

  slider->updateText();
  component->addAndMakeVisible(*slider);

  label = std::make_unique<juce::Label>();
  label->setText(parameter.displayName, juce::dontSendNotification);
  label->setJustificationType(juce::Justification::centred);
  component->addAndMakeVisible(*label);
}

void ParameterSlider::setBounds(juce::Rectangle<int> bounds) {
  bounds.removeFromTop(10);
  bounds.removeFromBottom(10);
  label->setBounds(bounds.removeFromTop(20));
  slider->setBounds(bounds);
}
