#pragma once
/*
  ==============================================================================

    ProfileManager.h

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#ifndef PROFILEMANAGER_H_INCLUDED
#define PROFILEMANAGER_H_INCLUDED

#include <memory>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "LR_IPC_OUT.h"
#include "MIDIProcessor.h"
class CommandMap;
class ControlsModel;
struct MIDI_Message_ID;

class ProfileChangeListener {
public:
    // called when the current profile is changed
  virtual void profileChanged(juce::XmlElement* elem, const juce::String& file_name) = 0;

  virtual ~ProfileChangeListener() {};
};

class ProfileManager final: private juce::AsyncUpdater, public LRConnectionListener {
public:
  ProfileManager(ControlsModel* c_model, CommandMap* const cmap) noexcept;
  virtual ~ProfileManager() {};
  void Init(std::weak_ptr<LR_IPC_OUT>&& out,
    std::shared_ptr<MIDIProcessor>& midi_processor);

  void addListener(ProfileChangeListener *listener);

  // sets the default profile directory and scans its contents for profiles
  void setProfileDirectory(const juce::File& dir);

  // returns an array of profile names
  const std::vector<juce::String>& getMenuItems() const noexcept;

  // switches to a profile defined by an index
  void switchToProfile(int profileIdx);

  // switches to a profile defined by a name
  void switchToProfile(const juce::String& profile);

  // switches to the next profile
  void switchToNextProfile();

  // switches to the previous profile
  void switchToPreviousProfile();

  // MIDICommandListener interface
  virtual void handleMIDI(RSJ::Message);

  // LRConnectionListener interface
  virtual void connected() override;
  virtual void disconnected() override;

private:
  void mapCommand(const MIDI_Message_ID& msg);
  // AsyncUpdate interface
  virtual void handleAsyncUpdate() override;
  enum class SWITCH_STATE {
    NONE,
    PREV,
    NEXT,
  };

  ProfileManager(ProfileManager const&) = delete;
  void operator=(ProfileManager const&) = delete;

  CommandMap* const command_map_{nullptr};
  ControlsModel* controls_model_{nullptr};
  int current_profile_index_{0};
  juce::File profile_location_;
  std::vector<juce::String> profiles_;
  std::vector<ProfileChangeListener *> listeners_;
  std::weak_ptr<LR_IPC_OUT> lr_ipc_out_;
  SWITCH_STATE switch_state_{SWITCH_STATE::NONE};
};

#endif  // PROFILEMANAGER_H_INCLUDED
