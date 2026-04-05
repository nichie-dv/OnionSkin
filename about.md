# Onion Skin

This mod brings the popular onion skin animation tool to Geometry Dash using editor layers.

## Features
* Animation Playback
* Lower editor layers turn red
* Higher editor layers turn blue
* Layer opacity decreases based on distance from current editor layer
* Customizable colors and opacity values
* Many other QOL settings

To use, simply toggle the onion button found next to the editor layer controls, and toggle `enable`.

## Animation Playback
Animation playback controls can be found in the onion skin menu located in the editor. The settings are:
* `playback` - toggles animation playback
* `playback range` - [`start`]  [`end`] - specifies which layers will play. These can be in reverse too. The `current` button will change `end` to be the frame you are currently on, or `start` + 1 if you're below `start`.
* `fps` - frames per second of playback

If enabled, a label will appear in the top left corner of the screen containing the current frame and time in animation:
* `{current} / {total}` - frame in animation
* `{0.00}s` - time in animation

This label can be disabled in the settings.

## Custom Keybinds
Keybinds can be changed from the mods settings. Te base keybinds are:
* `Shift + F7` - toggle onion skin
* `Shift + F8` - toggle playback
* `Shift + F9` - toggle previous frames
* `Shift + F10` - toggle future frames

These keybinds can be changed from the Mod settings.