#include "SpectrumAnalyzer.h"
#include "../../../Processor/PluginProcessor/PluginProcessor.h"
#include "../../LookAndFeel.h"

SpectrumAnalyzer::SpectrumAnalyzer(PluginProcessor &audioProcessor)
    : audioProcessor(audioProcessor), fft(fftOrder),
      window(fftSize, juce::dsp::WindowingFunction<float>::hann),
      analyzerFifo(audioProcessor.analyzerFifo) {
  fifoBuffer.resize(fftSize, 0.0f);
  fftData.resize(fftSize * 2, 0.0f);
  scopeData.resize(fftSize / 2, 0.0f);
  smoothedData.resize(fftSize / 2, -100.0f);
  startTimerHz(60);
};

void SpectrumAnalyzer::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds();
  LookAndFeel::drawBorder(g, getLookAndFeel(), bounds);

  SpectrumAnalyzer::drawSpectrum(g, bounds);
  SpectrumAnalyzer::drawFilterCurve(g, bounds);
  SpectrumAnalyzer::drawFrequencyMarkers(g, bounds);
}

void SpectrumAnalyzer::resized() { repaint(); };

void SpectrumAnalyzer::drawFilterCurve(juce::Graphics &g,
                                       juce::Rectangle<int> bounds) {

  auto sampleRate = audioProcessor.getSampleRate();
  auto coefficients = audioProcessor.dsp.getFilterCoefficients();
  auto isBypassed = audioProcessor.parameters.filterBypass->get();

  juce::Path responseCurve;
  float width = bounds.getWidth();
  auto height = bounds.getHeight();

  for (int x = 0; x < width; ++x) {
    auto normalizedX = x / width;
    auto currentFreq = MIN_FREQ * std::pow(MAX_FREQ / MIN_FREQ, normalizedX);

    auto magnitude =
        coefficients->getMagnitudeForFrequency(currentFreq, sampleRate);
    float magnitudeDb = juce::Decibels::gainToDecibels(magnitude);

    auto normalizedY = juce::jmap(magnitudeDb, MIN_DB, MAX_DB, 1.0f, 0.0f);
    auto y = bounds.getY() + normalizedY * height;

    if (x == 0) {
      responseCurve.startNewSubPath(x, y);
    } else {
      responseCurve.lineTo(x, y);
    }
  }

  // Draw the curve
  auto activeColor = juce::Colour(LookAndFeel::HIGHLIGHT);
  auto color =
      isBypassed ? LookAndFeel::getBypassedColour(activeColor) : activeColor;
  g.setColour(color);
  g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
};

void SpectrumAnalyzer::timerCallback() {
  // Drain FIFO and accumulate samples into fifoBuffer
  while (analyzerFifo.pull(analyzerSamples)) {
    for (float sample : analyzerSamples) {
      fifoBuffer[currentFifoIndex] = sample;

      // Once Buffer is full, perform FFT
      if (currentFifoIndex + 1 == fftSize) {
        std::copy(fifoBuffer.begin(), fifoBuffer.end(), fftData.begin());
        window.multiplyWithWindowingTable(fftData.data(), fftSize);
        fft.performFrequencyOnlyForwardTransform(fftData.data());

        for (int i = 0; i < scopeData.size(); ++i) {
          float magnitude = fftData[i] * 12.0f / (float)fftSize;
          scopeData[i] = juce::Decibels::gainToDecibels(magnitude, -100.0f);
        }
        currentFifoIndex = 0;
      } else {
        currentFifoIndex++;
      }
    }
  }

  // Attack/release smoothing
  for (int i = 0; i < scopeData.size(); ++i) {
    if (scopeData[i] > smoothedData[i]) {
      smoothedData[i] =
          scopeData[i] * ATTACK + smoothedData[i] * (1.0f - ATTACK);
    } else {
      smoothedData[i] =
          scopeData[i] * (1.0f - RELEASE) + smoothedData[i] * RELEASE;
    }
  }

  repaint();
};

void SpectrumAnalyzer::drawSpectrum(juce::Graphics &g,
                                    juce::Rectangle<int> bounds) {
  auto width = bounds.getWidth();
  auto height = bounds.getHeight();
  auto sampleRate = audioProcessor.getSampleRate();
  auto freqBinWidth = sampleRate / (float)fftSize;

  juce::Path spectrumPath;

  for (int x = 0; x < width; ++x) {
    // Map frequency to x position
    auto normalizedX = (float)x / width;
    auto freq = MIN_FREQ * std::pow(MAX_FREQ / MIN_FREQ, normalizedX);

    // Map frequency to FFT bin
    float targetBin = freq / freqBinWidth;
    int lowerBin = (int)targetBin;
    float binOffset = targetBin - lowerBin;

    // Blend magnitude between adjacent bins for smoother display
    float magnitude = MIN_DB;
    if (lowerBin >= 0 && lowerBin < smoothedData.size() - 1) {
      magnitude = juce::jmap(binOffset, smoothedData[lowerBin],
                             smoothedData[lowerBin + 1]);
    } else if (lowerBin >= 0 && lowerBin < smoothedData.size()) {
      magnitude = smoothedData[lowerBin];
    }

    // Apply frequency tilt
    float freqNormalized = juce::jmap(std::log10(freq), std::log10(MIN_FREQ),
                                      std::log10(MAX_FREQ), 0.0f, 1.0f);
    float tiltCompensation = freqNormalized * TILT;
    magnitude += tiltCompensation;

    // Map magnitude to y position
    auto level = juce::jmap(magnitude, MIN_DB, MAX_DB,
                            (float)bounds.getBottom(), (float)bounds.getY());

    if (x == 0)
      spectrumPath.startNewSubPath(x, level);
    else
      spectrumPath.lineTo(x, level);
  }

  g.setColour(juce::Colour(LookAndFeel::TEXT).withAlpha(0.5f));
  g.strokePath(spectrumPath, juce::PathStrokeType(1.0f));
}

void SpectrumAnalyzer::drawFrequencyMarkers(juce::Graphics &g,
                                            juce::Rectangle<int> bounds) {

  auto width = (float)bounds.getWidth();
  auto color = juce::Colour(LookAndFeel::TEXT).withAlpha(0.15f);
  g.setColour(color);
  g.setFont(10.0f);

  for (auto &[freq, label] : markers) {
    float x = juce::jmap(std::log10(freq), std::log10(MIN_FREQ),
                         std::log10(MAX_FREQ), 0.0f, width);

    g.drawVerticalLine((int)x, (float)bounds.getY(), (float)bounds.getBottom());
    g.drawText(label, (int)x + 3, bounds.getBottom() - 14, 30, 12,
               juce::Justification::left);
  }
}
