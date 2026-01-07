/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <Fifo.h>
#include <JuceHeader.h>

//==============================================================================
/**
 */
class MultieffectpluginAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                                        public juce::AudioProcessorARAExtension
#endif
{
public:
  //==============================================================================
  MultieffectpluginAudioProcessor();
  ~MultieffectpluginAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  enum class DSP_Option {
    Phase,
    Chorus,
    OverDrive,
    LadderFilter,
    Filter,
    END_OF_LIST
  };

  static juce::String getDSPOptionName(DSP_Option dspOption);
  static DSP_Option getDSPOptionFromName(const juce::String &name);

  static juce::AudioProcessorValueTreeState::ParameterLayout
  createParameterLayout();

  juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters",
                                           createParameterLayout()};

  using DSP_Order =
      std::array<DSP_Option, static_cast<size_t>(DSP_Option::END_OF_LIST)>;

  SimpleMBComp::Fifo<DSP_Order> dspOrderFifo;

  // Parameter Values

  //  Phaser
  juce::AudioParameterFloat *phaserRate = nullptr;
  juce::AudioParameterFloat *phaserDepth = nullptr;
  juce::AudioParameterFloat *phaserCenterFreq = nullptr;
  juce::AudioParameterFloat *phaserFeedback = nullptr;
  juce::AudioParameterFloat *phaserMix = nullptr;
  juce::AudioParameterBool *phaserBypass = nullptr;

  //  Chorus
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

  // Smoothed Values

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
  DSP_Order dspOrder;

  template <typename DSP> struct DSP_Choice : juce::dsp::ProcessorBase {
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

  struct FloatParamInitializer {
    juce::AudioParameterFloat **paramPtr;
    juce::String (*nameFunc)();
  };

  struct ChoiceParamInitializer {
    juce::AudioParameterChoice **paramPtr;
    juce::String (*nameFunc)();
  };

  struct BoolParamInitializer {
    juce::AudioParameterBool **paramPtr;
    juce::String (*nameFunc)();
  };

  template <typename ParamType, typename InitStruct>
  void initCachedParams(const std::vector<InitStruct> &paramInitializers) {
    for (const auto &initializer : paramInitializers) {
      *initializer.paramPtr =
          dynamic_cast<ParamType>(apvts.getParameter(initializer.nameFunc()));
      jassert(*initializer.paramPtr != nullptr);
    }
  }

  struct MonoChannelDSP {
    MonoChannelDSP(MultieffectpluginAudioProcessor &proc) : processor(proc) {}

    DSP_Choice<juce::dsp::DelayLine<float>> delay;
    DSP_Choice<juce::dsp::Phaser<float>> phaser;
    DSP_Choice<juce::dsp::Chorus<float>> chorus;
    DSP_Choice<juce::dsp::LadderFilter<float>> overdrive, ladderFilter;
    DSP_Choice<juce::dsp::IIR::Filter<float>> filter;

    void prepare(const juce::dsp::ProcessSpec &spec);
    void update();
    void process(juce::dsp::AudioBlock<float> block, const DSP_Order &dspOrder);

  private:
    MultieffectpluginAudioProcessor &processor;
    FilterMode cachedFilterMode = FilterMode::END_OF_LIST;
    float cachedFilterFreq = 0.f, cachedFilterQuality = 0.f,
          cachedFilterGain = -100.f;
  };

  MonoChannelDSP leftChannel{*this};
  MonoChannelDSP rightChannel{*this};

  struct ProcessorState {
    juce::dsp::ProcessorBase *processor = nullptr;
    bool bypassed = false;
  };

  using DSP_Pointers =
      std::array<ProcessorState, static_cast<size_t>(DSP_Option::END_OF_LIST)>;

  struct ParamSmootherPair {
    juce::AudioParameterFloat *param;
    juce::SmoothedValue<float> *smoother;
  };
  std::vector<ParamSmootherPair> paramSmootherPairs;

  enum class SmootherUpdateMode { initialize, updateExisting };
  void updateSmoothers(int samplesToSkip, SmootherUpdateMode smootherMode);
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultieffectpluginAudioProcessor)
};
