#pragma once

#include "../ParameterComponent.h"

class ParameterComboBox : public ParameterComponent {
public:
  ParameterComboBox(const Parameter &parameter,
                    juce::AudioProcessorValueTreeState &apvts,
                    juce::Component *component);

  void setBounds(juce::Rectangle<int> bounds) override;

private:
  Parameter parameter;
  juce::Component *component;

  std::unique_ptr<juce::ComboBox> comboBox;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      comboBoxAttachment;
  std::unique_ptr<juce::Label> label;
};
