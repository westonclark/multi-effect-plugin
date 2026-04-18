#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
  static constexpr juce::uint32 BACKGROUND = 0xff111110;  // warmBlack
  static constexpr juce::uint32 TEXT = 0xfff0ede6;        // creamWhite
  static constexpr juce::uint32 HIGHLIGHT = 0xffbf9e59;   // mutedGold
  static constexpr juce::uint32 METER_GREEN = 0xff1f6b47; // forestGreen
  static constexpr juce::uint32 METER_RED =
      0xffef4444; // red-500 (clip indicator)
  static constexpr juce::uint32 METER_OFF =
      0xff1e1e1c; // darkCharcoal

  LookAndFeel() {
    auto scheme = LookAndFeel_V4::ColourScheme(
        juce::Colour(BACKGROUND),                 // windowBackground
        juce::Colour(BACKGROUND).brighter(0.1f),  // widgetBackground
        juce::Colour(BACKGROUND).brighter(0.05f), // menuBackground
        juce::Colour(TEXT).withAlpha(0.3f),       // outline
        juce::Colour(TEXT),                       // defaultText
        juce::Colour(HIGHLIGHT),                  // defaultFill
        juce::Colour(BACKGROUND),                 // highlightedText
        juce::Colour(HIGHLIGHT),                  // highlightedFill
        juce::Colour(TEXT)                        // menuText
    );
    setColourScheme(scheme);
  }

  static juce::Colour getBypassedColour(juce::Colour baseColour) {
    return baseColour.withMultipliedSaturation(0.3f).withMultipliedBrightness(
        0.6f);
  }

  static void drawBorder(juce::Graphics &g, juce::LookAndFeel &lnf,
                         juce::Rectangle<int> bounds) {
    g.setColour(lnf.findColour(juce::ResizableWindow::backgroundColourId)
                    .brighter(0.15f));
    g.drawRect(bounds.reduced(1), 1.0f);
  }

  void drawTabButton(juce::TabBarButton &button, juce::Graphics &g,
                     bool isMouseOver, bool isMouseDown) override {
    auto activeArea = button.getActiveArea();
    auto bounds = activeArea.toFloat();

    const bool isFrontTab = button.isFrontTab();
    auto bgColour = button.getTabBackgroundColour();

    if (isFrontTab) {
      const float cornerSize = 4.0f;
      juce::Path p;
      p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(),
                            bounds.getHeight() + cornerSize, cornerSize,
                            cornerSize, true, true, false, false);
      g.setColour(bgColour);
      g.fillPath(p);
    } else {
      const float cornerSize = 4.0f;
      juce::Path p;
      p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(),
                            bounds.getHeight(), cornerSize, cornerSize, true,
                            true, false, false);
      g.setColour(bgColour);
      g.fillPath(p);

      if (isMouseOver) {
        g.setColour(juce::Colour(TEXT).withAlpha(0.05f));
        g.fillPath(p);
      }
    }

    auto textColour =
        isFrontTab ? juce::Colour(TEXT) : juce::Colour(TEXT).withAlpha(0.7f);

    g.setColour(textColour);
    g.setFont(13.0f);

    auto textArea = bounds.reduced(4.0f, 0.0f);
    g.drawText(button.getButtonText(), textArea.toNearestInt(),
               juce::Justification::centred, true);
  }

  int getTabButtonBestWidth(juce::TabBarButton &button, int tabDepth) override {
    return 140;
  }
};
