#include "AudioMeter.h"

AudioMeter::AudioMeter(InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo)
    :inputOutputLevelFifo(inputOutputLevelFifo) {
  startTimerHz(60);
}

void AudioMeter::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText(juce::String(juce::Decibels::gainToDecibels(level [0])) +
                 " dB",
             getLocalBounds(), juce::Justification::centred);
}

void AudioMeter::resized() {}

void AudioMeter::timerCallback() {
  // Pull level from fifo
  std::vector<float> newLevel;
  while (inputOutputLevelFifo.pull(newLevel)) {
    level = newLevel;
  };

  repaint();
};
