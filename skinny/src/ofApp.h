#pragma once

#include "MidiController.h"
#include "Show.h"
#include "meta.h"
#include "ofMain.h"

namespace skinny {

class ofApp : public ofBaseApp
{
public:
  explicit ofApp();

  void setup() override;
  void update() override;
  void draw() override;
  void exit() override;

  void keyReleased(ofKeyEventArgs& key) override;
  void keyReleasedGui(ofKeyEventArgs& args);

private:
  bool reload();

  std::shared_ptr<Show> show_;
  std::shared_ptr<ShowDescription> showDescription_;
  std::shared_ptr<MidiController> midiController_;
};

} // namespace skinny
