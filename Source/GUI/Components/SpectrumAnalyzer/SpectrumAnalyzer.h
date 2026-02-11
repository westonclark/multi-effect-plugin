#pragma once

#include <JuceHeader.h>

// Forward declaration
class PluginProcessor;

class SpectrumAnalyzer : public juce::Component, public juce::Timer {

public:
  SpectrumAnalyzer(PluginProcessor &p);

  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  PluginProcessor &audioProcessor;

  static constexpr float MIN_DB = -24.0f;
  static constexpr float MAX_DB = 24.0f;
  static constexpr float MIN_FREQ = 20.0f;
  static constexpr float MAX_FREQ = 20000.0f;

  void timerCallback() override;
  void drawFilterCurve(juce::Graphics &g, juce::Rectangle<int> bounds);
};
