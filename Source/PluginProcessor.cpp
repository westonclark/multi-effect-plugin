#include "PluginProcessor.h"
#include "PluginEditor.h"

// DSP OPTIONS
//==============================================================================
static const std::map<MultieffectpluginAudioProcessor::DspOption, juce::String>
    DspOptionNamesMap = {
        {MultieffectpluginAudioProcessor::DspOption::Phase, "Phaser"},
        {MultieffectpluginAudioProcessor::DspOption::Chorus, "Chorus"},
        {MultieffectpluginAudioProcessor::DspOption::OverDrive, "Drive"},
        {MultieffectpluginAudioProcessor::DspOption::LadderFilter,
         "Ladder Filter"},
        {MultieffectpluginAudioProcessor::DspOption::Filter, "Filter"},
};

juce::String
MultieffectpluginAudioProcessor::getDspNameFromOption(DspOption dspOption) {
  for (const auto &[option, optionName] : DspOptionNamesMap) {
    if (option == dspOption) {
      return optionName;
    }
  }
  return "None Selected";
}

MultieffectpluginAudioProcessor::DspOption
MultieffectpluginAudioProcessor::getDspOptionFromName(
    const juce::String &name) {
  for (const auto &[option, optionName] : DspOptionNamesMap) {
    if (optionName == name) {
      return option;
    }
  }
  return DspOption::END_OF_LIST;
}

// STATE SAVING METHODS
//==============================================================================
void MultieffectpluginAudioProcessor::saveDspOrderToState(
    const DspOrder &order) {
  auto dspOrderTree = apvts.state.getChildWithName("DspOrder");
  if (!dspOrderTree.isValid()) {
    dspOrderTree = juce::ValueTree("DspOrder");
    apvts.state.appendChild(dspOrderTree, nullptr);
  }

  for (int i = 0; i < order.size(); ++i) {
    dspOrderTree.setProperty("Position_" + juce::String(i),
                             getDspNameFromOption(order[i]), nullptr);
  }
}

MultieffectpluginAudioProcessor::DspOrder
MultieffectpluginAudioProcessor::getDspOrderFromState() const {
  DspOrder order;
  auto dspOrderTree = apvts.state.getChildWithName("DspOrder");

  if (!dspOrderTree.isValid()) {
    for (int i = 0; i < order.size(); ++i) {
      order[i] = static_cast<DspOption>(i);
    }
  } else {
    for (int i = 0; i < order.size(); ++i) {
      juce::String name =
          dspOrderTree.getProperty("Position_" + juce::String(i));
      order[i] = getDspOptionFromName(name);
    }
  }

  return order;
}

void MultieffectpluginAudioProcessor::saveSelectedTabToState(
    const DspOption &selectedTab) {
  apvts.state.setProperty("SelectedTab", getDspNameFromOption(selectedTab),
                          nullptr);
}

MultieffectpluginAudioProcessor::DspOption
MultieffectpluginAudioProcessor::getSelectedTabFromState() const {
  auto tabName = apvts.state.getProperty("SelectedTab", "");
  auto option = getDspOptionFromName(tabName);

  if (option == DspOption::END_OF_LIST) {
    return DspOption::Phase;
  } else {
    return option;
  }
}

// PARAMETER IDS
//==============================================================================
struct Parameters {
  struct Phaser {
    static inline const char *rate = "Phaser Rate";
    static inline const char *depth = "Phaser Depth";
    static inline const char *centerFreq = "Phaser Center Freq";
    static inline const char *feedback = "Phaser Feedback";
    static inline const char *mix = "Phaser Mix";
    static inline const char *bypass = "Phaser Bypass";
  };

  struct Chorus {
    static inline const char *rate = "Chorus Rate";
    static inline const char *depth = "Chorus Depth";
    static inline const char *centerDelay = "Chorus Center Delay";
    static inline const char *feedback = "Chorus Feedback";
    static inline const char *mix = "Chorus Mix";
    static inline const char *bypass = "Chorus Bypass";
  };

  struct Overdrive {
    static inline const char *saturation = "OverDrive";
    static inline const char *bypass = "Overdrive Bypass";
  };

  struct LadderFilter {
    static inline const char *mode = "Ladder Filter Mode";
    static inline const char *cutoff = "Ladder Filter Cutoff";
    static inline const char *resonance = "Ladder Filter Resonance";
    static inline const char *drive = "Ladder Filter Drive";
    static inline const char *bypass = "Ladder Filter Bypass";

    static inline const juce::StringArray modeChoices{
        "LPF12", "HPF12", "BPF12", "LPF24", "HPF24", "BPF24"};
  };

  struct Filter {
    static inline const char *mode = "Filter Mode";
    static inline const char *freq = "Filter Freq";
    static inline const char *quality = "Filter Quality";
    static inline const char *gain = "Filter Gain";
    static inline const char *bypass = "Filter Bypass";

    static inline const juce::StringArray modeChoices{"Peak", "Bandpass",
                                                      "Notch", "Allpass"};
  };
};

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
    dspOrder[i] = static_cast<DspOption>(i);
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
  if (!apvts.state.getChildWithName("DspOrder").isValid()) {
    juce::ValueTree dspOrderTree("DspOrder");
    for (int i = 0; i < static_cast<int>(DspOption::END_OF_LIST); ++i) {
      auto dspOption = static_cast<DspOption>(i);
      dspOrderTree.setProperty("Position_" + juce::String(i),
                               getDspNameFromOption(dspOption), nullptr);
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
    juce::dsp::AudioBlock<float> block, const DspOrder &dspOrder) {
  // Convert dspOrder into pointers
  DspPointers dspPointers;
  dspPointers.fill({});

  for (size_t i = 0; i < dspPointers.size(); ++i) {
    switch (dspOrder[i]) {
    case DspOption::Phase:
      dspPointers[i].processor = &phaser;
      dspPointers[i].bypassed = processor.phaserBypass->get();
      break;
    case DspOption::Chorus:
      dspPointers[i].processor = &chorus;
      dspPointers[i].bypassed = processor.chorusBypass->get();
      break;
    case DspOption::OverDrive:
      dspPointers[i].processor = &overdrive;
      dspPointers[i].bypassed = processor.overdriveBypass->get();
      break;
    case DspOption::LadderFilter:
      dspPointers[i].processor = &ladderFilter;
      dspPointers[i].bypassed = processor.ladderFilterBypass->get();
      break;
    case DspOption::Filter:
      dspPointers[i].processor = &filter;
      dspPointers[i].bypassed = processor.filterBypass->get();
      break;
    case DspOption::END_OF_LIST:
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
  auto newDSPOrder = DspOrder();
  while (dspOrderFifo.pull(newDSPOrder)) {
    if (newDSPOrder != DspOrder()) {
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

    auto order = getDspOrderFromState();
    dspOrderFifo.push(order);
  }
}

// PLUGIN INSTANTIATION
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MultieffectpluginAudioProcessor();
}
