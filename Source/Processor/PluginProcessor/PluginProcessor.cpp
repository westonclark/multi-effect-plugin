#include "PluginProcessor.h"
#include "../../GUI/PluginEditor/PluginEditor.h"

// DSP OPTIONS
//==============================================================================
static const std::map<DspOption, juce::String> DspOptionNamesMap = {
    {DspOption::Phase, "Phaser"},    {DspOption::Chorus, "Chorus"},
    {DspOption::OverDrive, "Drive"}, {DspOption::LadderFilter, "Ladder Filter"},
    {DspOption::Filter, "Filter"},
};

juce::String
PluginProcessor::getDspNameFromOption(DspOption dspOption) {
  for (const auto &[option, optionName] : DspOptionNamesMap) {
    if (option == dspOption) {
      return optionName;
    }
  }
  return "None Selected";
}

DspOption PluginProcessor::getDspOptionFromName(
    const juce::String &name) {
  for (const auto &[option, optionName] : DspOptionNamesMap) {
    if (optionName == name) {
      return option;
    }
  }
  return DspOption::END_OF_LIST;
}

// AUDIO PROCESSOR
//==============================================================================
PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
#else
    :
#endif
      parameters(*this), dsp(parameters, *this) {

  for (size_t i = 0; i < dspOrder.size(); ++i) {
    dspOrder[i] = static_cast<DspOption>(i);
  }

  // Initialize DSP order in ValueTree if it doesn't exist
  if (!parameters.apvts.state.getChildWithName("DspOrder").isValid()) {
    juce::ValueTree dspOrderTree("DspOrder");
    for (int i = 0; i < static_cast<int>(DspOption::END_OF_LIST); ++i) {
      auto dspOption = static_cast<DspOption>(i);
      dspOrderTree.setProperty("Position_" + juce::String(i),
                               getDspNameFromOption(dspOption), nullptr);
    }
    parameters.apvts.state.appendChild(dspOrderTree, nullptr);
  }
}

PluginProcessor::~PluginProcessor() {}

// PLUGIN INFO
//==============================================================================
const juce::String PluginProcessor::getName() const {
  return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const {
  return 0.0;
}

// PROGRAMS
//==============================================================================
int PluginProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are
            // 0 programs, so this should be at least 1, even if you're not
            // really implementing programs.
}

int PluginProcessor::getCurrentProgram() { return 0; }

void PluginProcessor::setCurrentProgram(int index) {}

const juce::String PluginProcessor::getProgramName(int index) {
  return {};
}

void PluginProcessor::changeProgramName(
    int index, const juce::String &newName) {}

// PREPARE / RELEASE CONFIG
//==============================================================================
void PluginProcessor::prepareToPlay(double sampleRate,
                                                    int samplesPerBlock) {

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;

  spec.numChannels = 1;
  dsp.prepareToPlay(spec);

  spec.numChannels = 2;
  inputGain.prepare(spec);
  outputGain.prepare(spec);
  inputGain.setRampDurationSeconds(0.05);
  outputGain.setRampDurationSeconds(0.05);

  parameters.prepareToPlay(sampleRate);
}

void PluginProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported(
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

#if !JucePlugin_IsSynth
  // This checks if the input layout matches the output layout
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void PluginProcessor::processBlock(
    juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // Update DSP order from the Fifo
  DspOrder newDspOrder;
  while (dspOrderFifo.pull(newDspOrder)) {
    dspOrder = newDspOrder;
  }

  auto block = juce::dsp::AudioBlock<float>(buffer);

  inputGain.setGainDecibels(parameters.inputGain->get());
  inputGain.process(juce::dsp::ProcessContextReplacing<float>(block));

  float inputRmsLeft = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
  float inputRmsRight = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
  inputLevelFifo.push({inputRmsLeft, inputRmsRight});

  parameters.updateSmoothers(buffer.getNumSamples(),
                             Parameters::SmootherUpdateMode::updateExisting);

  auto leftBlock = block.getSingleChannelBlock(0);
  auto rightBlock = block.getSingleChannelBlock(1);
  dsp.processBlock(leftBlock, rightBlock, dspOrder);

  outputGain.setGainDecibels(parameters.outputGain->get());
  outputGain.process(juce::dsp::ProcessContextReplacing<float>(block));

  float outputRmsLeft = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
  float outputRmsRight = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
  outputLevelFifo.push({outputRmsLeft, outputRmsRight});
}

// EDITOR
//==============================================================================
bool PluginProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PluginProcessor::createEditor() {
  return new PluginEditor(*this);
  // return new juce::GenericAudioProcessorEditor(*this);
}

// STATE SAVING METHODS
//==============================================================================
void PluginProcessor::saveDspOrderToState(
    const DspOrder &order) {
  auto dspOrderTree = parameters.apvts.state.getChildWithName("DspOrder");
  if (!dspOrderTree.isValid()) {
    dspOrderTree = juce::ValueTree("DspOrder");
    parameters.apvts.state.appendChild(dspOrderTree, nullptr);
  }

  for (int i = 0; i < order.size(); ++i) {
    dspOrderTree.setProperty("Position_" + juce::String(i),
                             getDspNameFromOption(order[i]), nullptr);
  }
}

DspOrder PluginProcessor::getDspOrderFromState() const {
  DspOrder order;
  auto dspOrderTree = parameters.apvts.state.getChildWithName("DspOrder");

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

void PluginProcessor::saveSelectedTabToState(
    const DspOption &selectedTab) {
  parameters.apvts.state.setProperty(
      "SelectedTab", getDspNameFromOption(selectedTab), nullptr);
}

DspOption PluginProcessor::getSelectedTabFromState() const {
  auto tabName = parameters.apvts.state.getProperty("SelectedTab", "");
  auto option = getDspOptionFromName(tabName);

  if (option == DspOption::END_OF_LIST) {
    return DspOption::Phase;
  } else {
    return option;
  }
}

// STATE MANAGEMENT
//==============================================================================
void PluginProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  juce::MemoryOutputStream memoryStream(destData, false);
  parameters.apvts.state.writeToStream(memoryStream);
}

void PluginProcessor::setStateInformation(const void *data,
                                                          int sizeInBytes) {
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if (tree.isValid()) {
    parameters.apvts.replaceState(tree);

    auto order = getDspOrderFromState();
    dspOrderFifo.push(order);
  }
}

// PLUGIN INSTANTIATION
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new PluginProcessor();
}
