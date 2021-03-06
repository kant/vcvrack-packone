# stoermelder PackOne

<!-- Version and License Badges -->
![Version](https://img.shields.io/badge/version-1.2.0-green.svg?style=flat-square)
![Rack SDK](https://img.shields.io/badge/Rack--SDK-1.1.6-red.svg?style=flat-square)
![License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)
![Language](https://img.shields.io/badge/language-C++-yellow.svg?style=flat-square)

The PackOne plugin gives you some utility modules for [VCV Rack](https://www.vcvrack.com).

![Intro image](./docs/intro.png)

- [4ROUNDS](./docs/FourRounds.md): randomizer for up to 16 input signals to create 15 output signals
- [8FACE](./docs/EightFace.md): preset sequencer for eight presets of any module as an universal expander
- [BOLT](./docs/Bolt.md): polyphonic CV-modulateable boolean functions
- [CV-MAP](./docs/CVMap.md): control 32 knobs/sliders/switches of any module by CV even when the module has no CV input
- [CV-PAM](./docs/CVPam.md): generate CV voltage by observing 32 knobs/sliders/switches of any module
- [INFIX](./docs/Infix.md): insert for polyphonic cables
- [µMAP](./docs/CVMapMicro.md): a single instance of CV-MAP's slots with attenuverters
- [MIDI-CAT](./docs/MidiCat.md): map parameters to midi controllers similar to MIDI-MAP with midi feedback and note mapping
- [ReMOVE Lite](./docs/ReMove.md): a recorder for knob/slider/switch-automation
- [ROTOR Model A](./docs/RotorA.md): spread a carrier signal across 2-16 output channels using CV
- [SIPO](./docs/Sipo.md): serial-in parallel-out shift register with polyphonic output and CV controls
- [STRIP](./docs/Strip.md): manage a group of modules in a patch, providing load, save as, disable and randomize

Stable versions are released in the [VCV Library](https://vcvrack.com/plugins.html#packone). Nightly builds of the latest commit can be downloaded [here](https://github.com/stoermelder/vcvrack-packone/releases/tag/Nightly). Please review the [changelog](./CHANGELOG.md) for this plugin.

Feel free to contact me or create a GitHub issue if you have any problems or questions!
If you like my modules consider donating to https://paypal.me/stoermelder. Thanks for your support!

## Building

Follow the [build instructions](https://vcvrack.com/manual/Building.html#building-rack-plugins) for VCV Rack.

## License

All **source code** is copyright © 2019 Benjamin Dill and is licensed under the [GNU General Public License, version v3.0](./LICENSE.txt).

All files and **graphics** in the `res` and `res-src` directories are licensed under [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/). You may not distribute modified adaptations of these graphics.