#pragma once

#include "../../../Utils/Fifos/SpectrumAnalyzerFifo.h"
#include <JuceHeader.h>
#include <array>

class PluginProcessor;

struct AnalyzerBuffer {
  static constexpr int MAX_SAMPLES = 4096;
  std::array<float, MAX_SAMPLES> samples = {};
  int numSamples = 0;
};

class SpectrumAnalyzer : public juce::Component, public juce::Timer {

public:
  SpectrumAnalyzer(PluginProcessor &p);
  ~SpectrumAnalyzer();

  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  PluginProcessor &audioProcessor;

  static constexpr float MIN_DB = -24.0f;
  static constexpr float MAX_DB = 24.0f;
  static constexpr float MIN_FREQ = 20.0f;
  static constexpr float MAX_FREQ = 20000.0f;

  const std::vector<std::pair<float, juce::String>> markers = {
      {50.0f, "50"},   {100.0f, "100"}, {250.0f, "250"}, {500.0f, "500"},
      {1000.0f, "1k"}, {2000.0f, "2k"}, {4000.0f, "4k"}, {10000.0f, "10k"}};

  juce::dsp::FFT fft;
  juce::dsp::WindowingFunction<float> window;
  static constexpr int fftOrder = 11; // 2^11 = 2048 samples
  static constexpr int fftSize = 1 << fftOrder;

  int currentFifoIndex = 0;
  std::vector<float> fifoBuffer;   // Accumulates incoming samples
  std::vector<float> fftData;      // Input to FFT (size = fftSize * 2)
  std::vector<float> scopeData;    // Processed FFT magnitudes for display
  std::vector<float> smoothedData; // Smoothed spectrum with attack/release

  static constexpr float ATTACK = 0.2f;
  static constexpr float RELEASE = 0.95f;
  static constexpr float TILT = 30.0f; // 3db per octave

  SpectrumAnalyzerFifo<AnalyzerBuffer> &analyzerFifo;

  bool filterUpdated = true;
  juce::dsp::IIR::Coefficients<float>::Ptr cachedCoefficients;

  class FilterParameterListener : public juce::AudioProcessorParameter::Listener {
  public:
    FilterParameterListener(SpectrumAnalyzer &parent) : parent(parent) {}
    void parameterValueChanged(int, float) override {
      parent.filterUpdated = true;
    }
    void parameterGestureChanged(int, bool) override {}

  private:
    SpectrumAnalyzer &parent;
  };
  std::unique_ptr<FilterParameterListener> filterListener;

  void timerCallback() override;
  void drawFilterCurve(juce::Graphics &g, juce::Rectangle<int> bounds);
  void drawSpectrum(juce::Graphics &g, juce::Rectangle<int> bounds);
  void drawFrequencyMarkers(juce::Graphics &g, juce::Rectangle<int> bounds);

  juce::dsp::IIR::Coefficients<float>::Ptr computeFilterCoefficients(
      int mode, float freq, float quality, float gain, double sampleRate);
};
