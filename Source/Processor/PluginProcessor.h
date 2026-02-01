#pragma once

#include "Parameters.h"
#include "../Utils/Fifo.h"
#include <JuceHeader.h>

// AUDIO PROCESSOR
//==============================================================================
class MultieffectpluginAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                                        public juce::AudioProcessorARAExtension
#endif
{
public:
  // LIFECYCLE
  //==============================================================================
  MultieffectpluginAudioProcessor();
  ~MultieffectpluginAudioProcessor() override;

  // AUDIO PROCESSING
  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  // EDITOR
  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  // PLUGIN INFO
  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  // PROGRAMS
  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  // STATE MANAGEMENT
  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // DSP ORDER
  //==============================================================================
  enum class DspOption {
    Phase,
    Chorus,
    OverDrive,
    LadderFilter,
    Filter,
    END_OF_LIST
  };

  static juce::String getDspNameFromOption(DspOption dspOption);
  static DspOption getDspOptionFromName(const juce::String &name);

  using DspOrder =
      std::array<DspOption, static_cast<size_t>(DspOption::END_OF_LIST)>;

  Fifo<DspOrder> dspOrderFifo;

  void saveDspOrderToState(const DspOrder &order);
  DspOrder getDspOrderFromState() const;

  void saveSelectedTabToState(const DspOption &selectedTab);
  DspOption getSelectedTabFromState() const;

  // PARAMETERS
  //==============================================================================
  static juce::AudioProcessorValueTreeState::ParameterLayout
  createParameterLayout();

  juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters",
                                           createParameterLayout()};
  // Phaser
  juce::AudioParameterFloat *phaserRate = nullptr;
  juce::AudioParameterFloat *phaserDepth = nullptr;
  juce::AudioParameterFloat *phaserCenterFreq = nullptr;
  juce::AudioParameterFloat *phaserFeedback = nullptr;
  juce::AudioParameterFloat *phaserMix = nullptr;
  juce::AudioParameterBool *phaserBypass = nullptr;
  // Chorus
  juce::AudioParameterFloat *chorusRate = nullptr;
  juce::AudioParameterFloat *chorusDepth = nullptr;
  juce::AudioParameterFloat *chorusCenterDelay = nullptr;
  juce::AudioParameterFloat *chorusFeedback = nullptr;
  juce::AudioParameterFloat *chorusMix = nullptr;
  juce::AudioParameterBool *chorusBypass = nullptr;
  // Drive
  juce::AudioParameterFloat *overdriveSaturation = nullptr;
  juce::AudioParameterBool *overdriveBypass = nullptr;
  // Ladder Filter
  juce::AudioParameterChoice *ladderFilterMode = nullptr;
  juce::AudioParameterFloat *ladderFilterCutoff = nullptr;
  juce::AudioParameterFloat *ladderFilterResonance = nullptr;
  juce::AudioParameterFloat *ladderFilterDrive = nullptr;
  juce::AudioParameterBool *ladderFilterBypass = nullptr;
  // Filter
  juce::AudioParameterChoice *filterMode = nullptr;
  juce::AudioParameterFloat *filterFreq = nullptr;
  juce::AudioParameterFloat *filterQuality = nullptr;
  juce::AudioParameterFloat *filterGain = nullptr;
  juce::AudioParameterBool *filterBypass = nullptr;
  // Input Gain
  juce::AudioParameterFloat *inputGain = nullptr;
  // Output Gain
  juce::AudioParameterFloat *outputGain = nullptr;

  // SMOOTHED VALUES
  //==============================================================================
  // Phaser
  juce::SmoothedValue<float> phaserRateSmoother;
  juce::SmoothedValue<float> phaserDepthSmoother;
  juce::SmoothedValue<float> phaserCenterFreqSmoother;
  juce::SmoothedValue<float> phaserFeedbackSmoother;
  juce::SmoothedValue<float> phaserMixSmoother;
  // Chorus
  juce::SmoothedValue<float> chorusRateSmoother;
  juce::SmoothedValue<float> chorusDepthSmoother;
  juce::SmoothedValue<float> chorusCenterDelaySmoother;
  juce::SmoothedValue<float> chorusFeedbackSmoother;
  juce::SmoothedValue<float> chorusMixSmoother;
  // Drive
  juce::SmoothedValue<float> overdriveSaturationSmoother;
  // Ladder Filter
  juce::SmoothedValue<float> ladderFilterCutoffSmoother;
  juce::SmoothedValue<float> ladderFilterResonanceSmoother;
  juce::SmoothedValue<float> ladderFilterDriveSmoother;
  // Filter
  juce::SmoothedValue<float> filterFreqSmoother;
  juce::SmoothedValue<float> filterQualitySmoother;
  juce::SmoothedValue<float> filterGainSmoother;
  enum FilterMode { Peak, Bandpass, Notch, Allpass, END_OF_LIST };

private:
  // DSP ORDER STATE
  //==============================================================================
  DspOrder dspOrder;

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

  // PARAMETER INITIALIZATION
  //==============================================================================
  struct FloatParamInitializer {
    juce::AudioParameterFloat **paramPtr;
    const char *paramName;
  };

  struct ChoiceParamInitializer {
    juce::AudioParameterChoice **paramPtr;
    const Parameter &param;
  };

  struct BoolParamInitializer {
    juce::AudioParameterBool **paramPtr;
    const Parameter &param;
  };

  template <typename ParamType, typename InitStruct>
  void initCachedParams(const std::vector<InitStruct> &paramInitializers) {
    for (const auto &initializer : paramInitializers) {
      *initializer.paramPtr =
          dynamic_cast<ParamType>(apvts.getParameter(initializer.paramName));
      jassert(*initializer.paramPtr != nullptr);
    }
  }

  void initCachedChoiceParams(
      const std::vector<ChoiceParamInitializer> &paramInitializers) {
    for (const auto &initializer : paramInitializers) {
      *initializer.paramPtr = dynamic_cast<juce::AudioParameterChoice *>(
          apvts.getParameter(initializer.param.id));
      jassert(*initializer.paramPtr != nullptr);
    }
  }

  void initCachedBoolParams(
      const std::vector<BoolParamInitializer> &paramInitializers) {
    for (const auto &initializer : paramInitializers) {
      *initializer.paramPtr = dynamic_cast<juce::AudioParameterBool *>(
          apvts.getParameter(initializer.param.id));
      jassert(*initializer.paramPtr != nullptr);
    }
  }

  // PARAMETER SMOOTHING
  //==============================================================================
  struct ParamSmootherPair {
    juce::AudioParameterFloat *param;
    juce::SmoothedValue<float> *smoother;
  };
  std::vector<ParamSmootherPair> paramSmootherPairs;

  enum class SmootherUpdateMode { initialize, updateExisting };
  void updateSmoothers(int samplesToSkip, SmootherUpdateMode smootherMode);

  // DSP EFFECTS
  //==============================================================================
  struct DspEffects {
    DspEffects(MultieffectpluginAudioProcessor &proc) : processor(proc) {}

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
    MultieffectpluginAudioProcessor &processor;
    FilterMode cachedFilterMode = FilterMode::END_OF_LIST;
    float cachedFilterFreq = 0.f, cachedFilterQuality = 0.f,
          cachedFilterGain = -100.f;
  };

  DspEffects leftChannel{*this};
  DspEffects rightChannel{*this};

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultieffectpluginAudioProcessor)
};
