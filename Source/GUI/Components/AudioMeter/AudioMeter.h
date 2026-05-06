#pragma once

#include "../../../Utils/Fifos/AudioMeterFifo.h"
#include <JuceHeader.h>

struct MeterLevel {
  float left = 0.0f;
  float right = 0.0f;
};

class AudioMeter : public juce::Component, juce::Timer {
public:
  AudioMeter(AudioMeterFifo<MeterLevel> &inputOutputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  AudioMeterFifo<MeterLevel> &inputOutputLevelFifo;
  MeterLevel rawLevel = {0.0f, 0.0f};
  MeterLevel smoothedLevel = {0.0f, 0.0f};

  void timerCallback() override;

  static constexpr int NUM_SEGMENTS = 24;
  static constexpr float MIN_DB = -48.0f;
  static constexpr float MAX_DB = 0.0f;

  static constexpr float ATTACK = 0.9f;
  static constexpr float RELEASE = 0.92f;

  std::array<std::array<juce::Rectangle<float>, NUM_SEGMENTS>, 2> segments;
};
