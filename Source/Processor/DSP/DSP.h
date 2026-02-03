#pragma once

#include "../Parameters/Parameters.h"
#include <JuceHeader.h>

// Forward declaration
class MultieffectpluginAudioProcessor;

enum class DspOption {
  Phase,
  Chorus,
  OverDrive,
  LadderFilter,
  Filter,
  END_OF_LIST
};

enum FilterMode { Peak, Bandpass, Notch, Allpass, END_OF_LIST };

using DspOrder =
    std::array<DspOption, static_cast<size_t>(DspOption::END_OF_LIST)>;

class DSP {
public:
  DSP(Parameters &params, juce::AudioProcessor &processor);

  void prepareToPlay(const juce::dsp::ProcessSpec &spec);
  void processBlock(juce::dsp::AudioBlock<float> leftBlock,
                    juce::dsp::AudioBlock<float> rightBlock,
                    const DspOrder &dspOrder);

private:
  // HELPER TYPES
  //==============================================================================
  template <typename DSP> struct DspChoice : juce::dsp::ProcessorBase {
    void prepare(const juce::dsp::ProcessSpec &spec) override {
      dsp.prepare(spec);
    }
    void
    process(const juce::dsp::ProcessContextReplacing<float> &context) override {
      dsp.process(context);
    }
    void reset() override { dsp.reset(); }

    DSP dsp;
  };

  struct ProcessorState {
    juce::dsp::ProcessorBase *processor = nullptr;
    bool bypassed = false;
  };

  using DspPointers =
      std::array<ProcessorState, static_cast<size_t>(DspOption::END_OF_LIST)>;

  // DSP CHANNEL
  //==============================================================================
  struct DspChannel {
    DspChannel(Parameters &params, juce::AudioProcessor &processor);

    DspChoice<juce::dsp::Phaser<float>> phaser;
    DspChoice<juce::dsp::Chorus<float>> chorus;
    DspChoice<juce::dsp::LadderFilter<float>> overdrive, ladderFilter;
    DspChoice<juce::dsp::IIR::Filter<float>> filter;

    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;

    void prepare(const juce::dsp::ProcessSpec &spec);
    void update();
    void process(juce::dsp::AudioBlock<float> block, const DspOrder &dspOrder);

  private:
    Parameters &parameters;
    juce::AudioProcessor &processor;
    FilterMode cachedFilterMode = FilterMode::END_OF_LIST;
    float cachedFilterFreq = 0.f, cachedFilterQuality = 0.f,
          cachedFilterGain = -100.f;
  };

  DspChannel leftChannel;
  DspChannel rightChannel;
  Parameters &parameters;
};
