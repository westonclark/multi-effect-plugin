#include "PluginProcessor.h"
#include "PluginEditor.h"

// DSP ORDER HELPERS
//==============================================================================
juce::String
MultieffectpluginAudioProcessor::getDSPOptionName(DSP_Option dspOption) {
  switch (dspOption) {
  case DSP_Option::Phase:
    return "Phaser";
  case DSP_Option::Chorus:
    return "Chorus";
  case DSP_Option::OverDrive:
    return "Drive";
  case DSP_Option::LadderFilter:
    return "Ladder Filter";
  case DSP_Option::Filter:
    return "Filter";
  case DSP_Option::END_OF_LIST:
    jassertfalse;
    break;
  }
  return "None Selected";
}

MultieffectpluginAudioProcessor::DSP_Option
MultieffectpluginAudioProcessor::getDSPOptionFromName(
    const juce::String &name) {
  if (name == "Phaser")
    return DSP_Option::Phase;
  if (name == "Chorus")
    return DSP_Option::Chorus;
  if (name == "Drive")
    return DSP_Option::OverDrive;
  if (name == "Ladder Filter")
    return DSP_Option::LadderFilter;
  if (name == "Filter")
    return DSP_Option::Filter;

  return DSP_Option::END_OF_LIST;
}

void MultieffectpluginAudioProcessor::saveDspOrderToState(
    const DSP_Order &order) {
  auto dspOrderTree = apvts.state.getChildWithName("DSP_Order");
  if (!dspOrderTree.isValid()) {
    dspOrderTree = juce::ValueTree("DSP_Order");
    apvts.state.appendChild(dspOrderTree, nullptr);
  }

  for (int i = 0; i < order.size(); ++i) {
    dspOrderTree.setProperty("Position_" + juce::String(i),
                             getDSPOptionName(order[i]), nullptr);
  }
}

MultieffectpluginAudioProcessor::DSP_Order
MultieffectpluginAudioProcessor::loadDspOrderFromState() const {
  DSP_Order order;
  auto dspOrderTree = apvts.state.getChildWithName("DSP_Order");

  // Return default order if not found
  if (!dspOrderTree.isValid()) {
    for (int i = 0; i < order.size(); ++i) {
      order[i] = static_cast<DSP_Option>(i);
    }
  } else {
    for (int i = 0; i < order.size(); ++i) {
      juce::String name =
          dspOrderTree.getProperty("Position_" + juce::String(i));
      order[i] = getDSPOptionFromName(name);
    }
  }

  return order;
}

// PARAMETER IDS
//==============================================================================
namespace Parameters {
namespace Phaser {
inline constexpr const char *rate = "Phaser Rate";
inline constexpr const char *depth = "Phaser Depth";
inline constexpr const char *centerFreq = "Phaser Center Freq";
inline constexpr const char *feedback = "Phaser Feedback";
inline constexpr const char *mix = "Phaser Mix";
inline constexpr const char *bypass = "Phaser Bypass";
} // namespace Phaser

namespace Chorus {
inline constexpr const char *rate = "Chorus Rate";
inline constexpr const char *depth = "Chorus Depth";
inline constexpr const char *centerDelay = "Chorus Center Delay";
inline constexpr const char *feedback = "Chorus Feedback";
inline constexpr const char *mix = "Chorus Mix";
inline constexpr const char *bypass = "Chorus Bypass";
} // namespace Chorus

namespace Overdrive {
inline constexpr const char *saturation = "OverDrive";
inline constexpr const char *bypass = "Overdrive Bypass";
} // namespace Overdrive

namespace LadderFilter {
inline constexpr const char *mode = "Ladder Filter Mode";
inline constexpr const char *cutoff = "Ladder Filter Cutoff";
inline constexpr const char *resonance = "Ladder Filter Resonance";
inline constexpr const char *drive = "Ladder Filter Drive";
inline constexpr const char *bypass = "Ladder Filter Bypass";

inline const juce::StringArray modeChoices{"LPF12", "HPF12", "BPF12",
                                           "LPF24", "HPF24", "BPF24"};
} // namespace LadderFilter

namespace Filter {
inline constexpr const char *mode = "Filter Mode";
inline constexpr const char *freq = "Filter Freq";
inline constexpr const char *quality = "Filter Quality";
inline constexpr const char *gain = "Filter Gain";
inline constexpr const char *bypass = "Filter Bypass";

inline const juce::StringArray modeChoices{"Peak", "Bandpass", "Notch",
                                           "Allpass"};
} // namespace Filter
} // namespace Parameters

// AUDIO PROCESSOR
//==============================================================================
MultieffectpluginAudioProcessor::MultieffectpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{

  for (size_t i = 0; i < dspOrder.size(); ++i) {
    dspOrder[i] = static_cast<DSP_Option>(i);
  }

  // Initialize parameters from Value Tree
  auto floatParamInitializers = std::vector<FloatParamInitializer>{
      {&phaserRate, Parameters::Phaser::rate},
      {&phaserCenterFreq, Parameters::Phaser::centerFreq},
      {&phaserDepth, Parameters::Phaser::depth},
      {&phaserFeedback, Parameters::Phaser::feedback},
      {&phaserMix, Parameters::Phaser::mix},
      {&chorusRate, Parameters::Chorus::rate},
      {&chorusDepth, Parameters::Chorus::depth},
      {&chorusCenterDelay, Parameters::Chorus::centerDelay},
      {&chorusFeedback, Parameters::Chorus::feedback},
      {&chorusMix, Parameters::Chorus::mix},
      {&overdriveSaturation, Parameters::Overdrive::saturation},
      {&ladderFilterCutoff, Parameters::LadderFilter::cutoff},
      {&ladderFilterResonance, Parameters::LadderFilter::resonance},
      {&ladderFilterDrive, Parameters::LadderFilter::drive},
      {&filterFreq, Parameters::Filter::freq},
      {&filterQuality, Parameters::Filter::quality},
      {&filterGain, Parameters::Filter::gain},
  };

  auto choiceParamInitializers = std::vector<ChoiceParamInitializer>{
      {&ladderFilterMode, Parameters::LadderFilter::mode},
      {&filterMode, Parameters::Filter::mode},
  };

  auto boolParamInitializers = std::vector<BoolParamInitializer>{
      {&phaserBypass, Parameters::Phaser::bypass},
      {&chorusBypass, Parameters::Chorus::bypass},
      {&overdriveBypass, Parameters::Overdrive::bypass},
      {&ladderFilterBypass, Parameters::LadderFilter::bypass},
      {&filterBypass, Parameters::Filter::bypass},
  };

  initCachedParams<juce::AudioParameterFloat *>(floatParamInitializers);
  initCachedParams<juce::AudioParameterChoice *>(choiceParamInitializers);
  initCachedParams<juce::AudioParameterBool *>(boolParamInitializers);

  // Initialize parameter smoothers
  paramSmootherPairs = {
      {phaserRate, &phaserRateSmoother},
      {phaserDepth, &phaserDepthSmoother},
      {phaserCenterFreq, &phaserCenterFreqSmoother},
      {phaserFeedback, &phaserFeedbackSmoother},
      {phaserMix, &phaserMixSmoother},
      {chorusRate, &chorusRateSmoother},
      {chorusDepth, &chorusDepthSmoother},
      {chorusCenterDelay, &chorusCenterDelaySmoother},
      {chorusFeedback, &chorusFeedbackSmoother},
      {chorusMix, &chorusMixSmoother},
      {overdriveSaturation, &overdriveSaturationSmoother},
      {ladderFilterCutoff, &ladderFilterCutoffSmoother},
      {ladderFilterResonance, &ladderFilterResonanceSmoother},
      {ladderFilterDrive, &ladderFilterDriveSmoother},
      {filterFreq, &filterFreqSmoother},
      {filterQuality, &filterQualitySmoother},
      {filterGain, &filterGainSmoother},
  };

  // Initialize DSP order in ValueTree if it doesn't exist
  if (!apvts.state.getChildWithName("DSP_Order").isValid()) {
    juce::ValueTree dspOrderTree("DSP_Order");
    for (int i = 0; i < static_cast<int>(DSP_Option::END_OF_LIST); ++i) {
      auto dspOption = static_cast<DSP_Option>(i);
      dspOrderTree.setProperty("Position_" + juce::String(i),
                               getDSPOptionName(dspOption), nullptr);
    }
    apvts.state.appendChild(dspOrderTree, nullptr);
  }
}

MultieffectpluginAudioProcessor::~MultieffectpluginAudioProcessor() {}

// PLUGIN INFO
//==============================================================================
const juce::String MultieffectpluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool MultieffectpluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool MultieffectpluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool MultieffectpluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double MultieffectpluginAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

// PROGRAMS
//==============================================================================
int MultieffectpluginAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are
            // 0 programs, so this should be at least 1, even if you're not
            // really implementing programs.
}

int MultieffectpluginAudioProcessor::getCurrentProgram() { return 0; }

void MultieffectpluginAudioProcessor::setCurrentProgram(int index) {}

const juce::String MultieffectpluginAudioProcessor::getProgramName(int index) {
  return {};
}

void MultieffectpluginAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

// PREPARE / RELEASE CONFIG
//==============================================================================
void MultieffectpluginAudioProcessor::prepareToPlay(double sampleRate,
                                                    int samplesPerBlock) {

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = 1;

  leftChannel.prepare(spec);
  rightChannel.prepare(spec);

  for (const auto &pair : paramSmootherPairs) {
    pair.smoother->reset(sampleRate, 0.05);
  }
  updateSmoothers(1, SmootherUpdateMode::initialize);
}

void MultieffectpluginAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultieffectpluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

// PARAMETER LAYOUT
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
MultieffectpluginAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  const int versionHint = 1;
  juce::String name;
  juce::StringArray choices;

  // Phaser
  name = Parameters::Phaser::rate;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"));

  name = Parameters::Phaser::depth;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = Parameters::Phaser::centerFreq;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 1000.f, "Hz"));

  name = Parameters::Phaser::feedback;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"));

  name = Parameters::Phaser::mix;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = Parameters::Phaser::bypass;
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Chorus
  name = Parameters::Chorus::rate;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"));

  name = Parameters::Chorus::depth;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = Parameters::Chorus::centerDelay;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 7.f, "ms"));

  name = Parameters::Chorus::feedback;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"));

  name = Parameters::Chorus::mix;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = Parameters::Chorus::bypass;
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Drive
  name = Parameters::Overdrive::saturation;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""));

  name = Parameters::Overdrive::bypass;
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Ladder Filter
  name = Parameters::LadderFilter::mode;
  choices = Parameters::LadderFilter::modeChoices;
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{name, versionHint}, name, choices, 0));

  name = Parameters::LadderFilter::cutoff;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 1.f), 20000.f, "Hz"));

  name = Parameters::LadderFilter::resonance;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f), 0.f, ""));

  name = Parameters::LadderFilter::drive;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""));

  name = Parameters::LadderFilter::bypass;
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Filter
  name = Parameters::Filter::mode;
  choices = Parameters::Filter::modeChoices;
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{name, versionHint}, name, choices, 0));

  name = Parameters::Filter::freq;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 1000.f, "Hz"));

  name = Parameters::Filter::quality;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.1f, 10.f, .05f, 1.f), 5.f, ""));

  name = Parameters::Filter::gain;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-24.f, 24.f, .5f, 1.f), 0.f, "dB"));

  name = Parameters::Filter::bypass;
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  return layout;
}

// PARAMETER SMOOTHING
//==============================================================================
void MultieffectpluginAudioProcessor::updateSmoothers(
    int samplesToSkip, SmootherUpdateMode smootherMode) {
  for (const auto &pair : paramSmootherPairs) {
    if (smootherMode == SmootherUpdateMode::initialize) {
      pair.smoother->setCurrentAndTargetValue(pair.param->get());
    } else {
      pair.smoother->setTargetValue(pair.param->get());
    }
    pair.smoother->skip(samplesToSkip);
  }
}

// DSP EFFECTS
//==============================================================================
void MultieffectpluginAudioProcessor::DspEffects::prepare(
    const juce::dsp::ProcessSpec &spec) {
  jassert(spec.numChannels == 1);

  std::vector<juce::dsp::ProcessorBase *> dsp{&phaser, &chorus, &overdrive,
                                              &ladderFilter, &filter};

  for (auto processor : dsp) {
    processor->prepare(spec);
    processor->reset();
  }
}

void MultieffectpluginAudioProcessor::DspEffects::update() {
  // Phaser
  phaser.dsp.setRate(processor.phaserRateSmoother.getCurrentValue());
  phaser.dsp.setCentreFrequency(
      processor.phaserCenterFreqSmoother.getCurrentValue());
  phaser.dsp.setDepth(processor.phaserDepthSmoother.getCurrentValue());
  phaser.dsp.setFeedback(processor.phaserFeedbackSmoother.getCurrentValue());
  phaser.dsp.setMix(processor.phaserMixSmoother.getCurrentValue());
  // Chorus
  chorus.dsp.setRate(processor.chorusRateSmoother.getCurrentValue());
  chorus.dsp.setDepth(processor.chorusDepthSmoother.getCurrentValue());
  chorus.dsp.setCentreDelay(
      processor.chorusCenterDelaySmoother.getCurrentValue());
  chorus.dsp.setFeedback(processor.chorusFeedbackSmoother.getCurrentValue());
  chorus.dsp.setMix(processor.chorusMixSmoother.getCurrentValue());
  // Drive
  overdrive.dsp.setDrive(
      processor.overdriveSaturationSmoother.getCurrentValue());
  // Ladder Filter
  ladderFilter.dsp.setMode(static_cast<juce::dsp::LadderFilterMode>(
      processor.ladderFilterMode->getIndex()));
  ladderFilter.dsp.setCutoffFrequencyHz(
      processor.ladderFilterCutoffSmoother.getCurrentValue());
  ladderFilter.dsp.setResonance(
      processor.ladderFilterResonanceSmoother.getCurrentValue());
  ladderFilter.dsp.setDrive(
      processor.ladderFilterDriveSmoother.getCurrentValue());
  // Filter
  auto currentFilterFreq = processor.filterFreqSmoother.getCurrentValue();
  auto currentFilterQuality = processor.filterQualitySmoother.getCurrentValue();
  auto currentFilterGain = processor.filterGainSmoother.getCurrentValue();
  auto currentFilterMode = processor.filterMode->getIndex();

  // Only update filter coefficients if mode changes or if values are changing
  bool modeChanged = (currentFilterMode != cachedFilterMode);
  bool paramsChanging = (currentFilterFreq != cachedFilterFreq) ||
                        (currentFilterQuality != cachedFilterQuality) ||
                        (currentFilterGain != cachedFilterGain);

  if (modeChanged || paramsChanging) {
    cachedFilterMode = static_cast<FilterMode>(currentFilterMode);
    cachedFilterFreq = currentFilterFreq;
    cachedFilterQuality = currentFilterQuality;
    cachedFilterGain = currentFilterGain;

    auto sampleRate = processor.getSampleRate();
    juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

    switch (cachedFilterMode) {
    case FilterMode::Peak: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
          sampleRate, cachedFilterFreq, cachedFilterQuality,
          juce::Decibels::decibelsToGain(cachedFilterGain));
      break;
    };
    case FilterMode::Bandpass: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    }
    case FilterMode::Notch: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    };
    case FilterMode::Allpass: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    }
    case FilterMode::END_OF_LIST: {
      jassertfalse;
      break;
    }
    }

    if (coefficients != nullptr) {
      *filter.dsp.coefficients = *coefficients;
    }
  }
}

void MultieffectpluginAudioProcessor::DspEffects::process(
    juce::dsp::AudioBlock<float> block, const DSP_Order &dspOrder) {
  // Convert dspOrder into pointers
  DSP_Pointers dspPointers;
  dspPointers.fill({});

  for (size_t i = 0; i < dspPointers.size(); ++i) {
    switch (dspOrder[i]) {
    case DSP_Option::Phase:
      dspPointers[i].processor = &phaser;
      dspPointers[i].bypassed = processor.phaserBypass->get();
      break;
    case DSP_Option::Chorus:
      dspPointers[i].processor = &chorus;
      dspPointers[i].bypassed = processor.chorusBypass->get();
      break;
    case DSP_Option::OverDrive:
      dspPointers[i].processor = &overdrive;
      dspPointers[i].bypassed = processor.overdriveBypass->get();
      break;
    case DSP_Option::LadderFilter:
      dspPointers[i].processor = &ladderFilter;
      dspPointers[i].bypassed = processor.ladderFilterBypass->get();
      break;
    case DSP_Option::Filter:
      dspPointers[i].processor = &filter;
      dspPointers[i].bypassed = processor.filterBypass->get();
      break;
    case DSP_Option::END_OF_LIST:
      jassertfalse;
      break;
    }
  }

  // Process
  auto context = juce::dsp::ProcessContextReplacing<float>(block);
  for (size_t i = 0; i < dspPointers.size(); ++i) {
    if (dspPointers[i].processor != nullptr && !dspPointers[i].bypassed) {
      juce::ScopedValueSetter<bool> svs(context.isBypassed,
                                        dspPointers[i].bypassed);
      dspPointers[i].processor->process(context);
    }
  }
}

void MultieffectpluginAudioProcessor::processBlock(
    juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // Clear buffer of garbage
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Check if there's a new DSP order from the GUI
  auto newDSPOrder = DSP_Order();
  while (dspOrderFifo.pull(newDSPOrder)) {
    if (newDSPOrder != DSP_Order()) {
      dspOrder = newDSPOrder;
    }
  }

  // Process audio in chunks for better parameter smoothing resolution
  const int maxChunkSize = 64;
  const auto numSamples = buffer.getNumSamples();
  auto block = juce::dsp::AudioBlock<float>(buffer);

  for (size_t startSample = 0; startSample < numSamples;) {
    auto samplesThisChunk =
        juce::jmin(maxChunkSize, static_cast<int>(numSamples - startSample));

    // Update smoothers for this chunk
    updateSmoothers(samplesThisChunk, SmootherUpdateMode::updateExisting);

    // Update DSP parameters from smoothed values
    leftChannel.update();
    rightChannel.update();

    // Process this chunk
    auto subBlock = block.getSubBlock(startSample, samplesThisChunk);
    leftChannel.process(subBlock.getSingleChannelBlock(0), dspOrder);
    rightChannel.process(subBlock.getSingleChannelBlock(1), dspOrder);

    startSample += samplesThisChunk;
  }
}

// EDITOR
//==============================================================================
bool MultieffectpluginAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MultieffectpluginAudioProcessor::createEditor() {
  return new MultieffectpluginAudioProcessorEditor(*this);
  // return new juce::GenericAudioProcessorEditor(*this);
}

// STATE MANAGEMENT
//==============================================================================
template <>
struct juce::VariantConverter<MultieffectpluginAudioProcessor::DSP_Order> {
  static MultieffectpluginAudioProcessor::DSP_Order
  fromVar(const juce::var &variable) {
    using T = MultieffectpluginAudioProcessor::DSP_Order;
    T dspOrder;
    jassert(variable.isBinaryData());

    if (variable.isBinaryData() == false) {
      dspOrder.fill(MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST);
    } else {

      auto memoryBlock = *variable.getBinaryData();

      juce::MemoryInputStream stream(memoryBlock, false);
      std::vector<int> array;
      while (!stream.isExhausted()) {
        array.push_back(stream.readInt());
      }

      jassert(array.size() == dspOrder.size());
      for (size_t i = 0; i < dspOrder.size(); ++i) {
        dspOrder[i] =
            static_cast<MultieffectpluginAudioProcessor::DSP_Option>(array[i]);
      }
    }
    return dspOrder;
  };

  static juce::var toVar(const MultieffectpluginAudioProcessor::DSP_Order &t) {
    juce::MemoryBlock memoryBlock;
    {
      juce::MemoryOutputStream stream(memoryBlock, false);
      for (const auto &value : t) {
        stream.writeInt(static_cast<int>(value));
      }
    }
    return memoryBlock;
  };
};

void MultieffectpluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  juce::MemoryOutputStream memoryStream(destData, false);
  apvts.state.writeToStream(memoryStream);
}

void MultieffectpluginAudioProcessor::setStateInformation(const void *data,
                                                          int sizeInBytes) {
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if (tree.isValid()) {
    apvts.replaceState(tree);

    auto order = loadDspOrderFromState();
    dspOrderFifo.push(order);
  }
}

// PLUGIN INSTANTIATION
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MultieffectpluginAudioProcessor();
}
