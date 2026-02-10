#pragma once

#include "../../../Utils/Fifos/InputOutputLevelFifo.h"
#include <JuceHeader.h>

class AudioMeter : public juce::Component, juce::Timer {
public:
  AudioMeter(InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo;
  std::vector<float> rawLevel = {0.0f, 0.0f};
  std::vector<float> smoothedLevel = {0.0f, 0.0f};

  void timerCallback() override;

  static constexpr int NUM_SEGMENTS = 24;
  static constexpr float MIN_DB = -48.0f;
  static constexpr float MAX_DB = 0.0f;

  static constexpr float ATTACK = 0.9f;
  static constexpr float RELEASE = 0.92f;
};
