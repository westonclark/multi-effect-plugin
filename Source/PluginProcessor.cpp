#include "PluginProcessor.h"
#include "Parameters.h"
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
      {&phaserRate, Parameters::Phaser::rate.id},
      {&phaserCenterFreq, Parameters::Phaser::centerFreq.id},
      {&phaserDepth, Parameters::Phaser::depth.id},
      {&phaserFeedback, Parameters::Phaser::feedback.id},
      {&phaserMix, Parameters::Phaser::mix.id},
      {&chorusRate, Parameters::Chorus::rate.id},
      {&chorusDepth, Parameters::Chorus::depth.id},
      {&chorusCenterDelay, Parameters::Chorus::centerDelay.id},
      {&chorusFeedback, Parameters::Chorus::feedback.id},
      {&chorusMix, Parameters::Chorus::mix.id},
      {&overdriveSaturation, Parameters::Overdrive::saturation.id},
      {&ladderFilterCutoff, Parameters::LadderFilter::cutoff.id},
      {&ladderFilterResonance, Parameters::LadderFilter::resonance.id},
      {&ladderFilterDrive, Parameters::LadderFilter::drive.id},
      {&filterFreq, Parameters::Filter::freq.id},
      {&filterQuality, Parameters::Filter::quality.id},
      {&filterGain, Parameters::Filter::gain.id},
      {&inputGain, Parameters::Input::gain.id},
      {&outputGain, Parameters::Output::gain.id},
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
  initCachedChoiceParams(choiceParamInitializers);
  initCachedBoolParams(boolParamInitializers);

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

  for (const auto &param : Parameters::getAllParameters()) {
    if (param.type == ParameterType::Float) {
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{param.id, versionHint}, param.id,
          juce::NormalisableRange<float>(param.minValue, param.maxValue,
                                         param.step, param.skew),
          param.defaultValue, param.suffix));
    } else if (param.type == ParameterType::Choice) {
      layout.add(std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID{param.id, versionHint}, param.displayName,
          *param.choices, 0, ""));
    } else if (param.type == ParameterType::Bool) {
      layout.add(std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{param.id, versionHint}, param.id, false));
    }
  }

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

  // Set defualt filter settings for overdrive
  overdrive.dsp.setMode(juce::dsp::LadderFilterMode::LPF12);
  overdrive.dsp.setCutoffFrequencyHz(20000.0f);
  overdrive.dsp.setResonance(0);

  inputGain.prepare(spec);
  outputGain.prepare(spec);
  inputGain.setRampDurationSeconds(0.05);
  outputGain.setRampDurationSeconds(0.05);
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

  // Update DSP order from the Fifo
  DspOrder newDspOrder;
  while (dspOrderFifo.pull(newDspOrder)) {
    dspOrder = newDspOrder;
  }

  const auto numSamples = buffer.getNumSamples();
  auto block = juce::dsp::AudioBlock<float>(buffer);
  auto leftBlock = block.getSingleChannelBlock(0);
  auto rightBlock = block.getSingleChannelBlock(1);

  updateSmoothers(numSamples, SmootherUpdateMode::updateExisting);

  leftChannel.inputGain.setGainDecibels(inputGain->get());
  leftChannel.inputGain.process(
      juce::dsp::ProcessContextReplacing<float>(leftBlock));

  rightChannel.inputGain.setGainDecibels(inputGain->get());
  rightChannel.inputGain.process(
      juce::dsp::ProcessContextReplacing<float>(rightBlock));

  leftChannel.update();
  rightChannel.update();

  leftChannel.process(leftBlock, dspOrder);
  rightChannel.process(rightBlock, dspOrder);

  leftChannel.outputGain.setGainDecibels(outputGain->get());
  leftChannel.outputGain.process(
      juce::dsp::ProcessContextReplacing<float>(leftBlock));

  rightChannel.outputGain.setGainDecibels(outputGain->get());
  rightChannel.outputGain.process(
      juce::dsp::ProcessContextReplacing<float>(rightBlock));
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
