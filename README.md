# TMS Express 🗣
TMS Express generates bitstreams for the TMS5220 Voice Synthesis Processor. It
also includes a TMS5220 emulator for synthesizing bitstreams as audio files.

![TMS Express GUI Screenshot](doc/screenshot.png)

The TMS5220 hardware and its software analogues (i.e. Arduino Talkie) alike may
be driven by the output of TMS Express. The program accepts audio files in a
variety of formats, applies linear predictive coding (LPC) to compress the data,
and outputs either an ASCII bitstream or C header with the encoded data.

Compared to existing encoders, TMS Express has the following advantages:
- Implements the original Levinson-Durbin recursion to determine LPC
  coefficients
- Supports repeat frames for enhanced audio compression
- Automatically downsamples and mixes audio files of any format
- Performs batch encoding of multiple files

# Installation

## Linux & Windows

Linux and Windows (via WSL) users can download the latest TMS Express binary
from the [Releases](https://github.com/tornupnegatives/TMS-Express/releases)
tab in GitHub. Please note that Windows is not officially supported and may not
function correctly in all cases.

## macOS & Linux

Dynamically linked binaries for macOS and Linux may be obtained using the
Homebrew package manager. Statically linked binaries are provided in the
[Releases](https://github.com/tornupnegatives/TMS-Express/releases) tab on
GitHub.

```shell
$ brew tap tornupnegatives/tap && brew install tmsexpress
```

## Compile from Source

TMS Express may be compiled as either a command-line (CLI) application or a
graphical (GUI) application, depending on the user's needs. The CLI version
has fewer dependencies and will compile on a wider range of systems.

### Installing Dependencies

```shell
# macOS #######################################################################

# CLI-only build
$ brew install cmake libsamplerate libsndfile pkg-config

# GUI build
$ brew install cmake libsamplerate libsndfile pkg-config qt

# Linux (Ubuntu) ##############################################################

# CLI-only build
$ sudo apt install cmake libsndfile1-dev libsamplerate0-dev pkg-config

# GUI build
$ sudo apt install cmake libsndfile1-dev libsamplerate0-dev pkg-config \
  qt6-base-dev qt6-multimedia-dev libgl1-mesa-dev
```

### Compilation
```shell
$ cmake -B build # For CLI only, pass -DTMSEXPRESS_BUILD_GUI=OFF
$ cmake --build build -j
```

## Usage
## GUI
To launch the TMS Express GUI frontend, simply invoke the program with no
arguments

```shell
$ tmsexpress
```

## The Encode Command
The `encode` command accepts audio file(s) and a variety of parameters which
affect how they are processed, analyzed, and formatted for output. TMS Express
automatically detects when the input path is a directory and performs a batch
job.

```shell
$ tmsexpress encode [OPTIONS] input output
```

### Explanation of Options
- `window`: Speech data is separated into small windows/segments, each of which
  are analyzed individually. This is because small enough segments of speech
  data are roughly periodic and their behavior may be generalized. An ideal
  window width is between 22.5-25 ms
  - Values above and below the recommendation will artificially speed up and
    slow down speech, respectively
- `highpass` and `lowpass`: Speech data occupies a relatively small frequency
  band compared to what digital audio files are capable of representing.
  Filtering out unnecessary frequencies may lead to more accurate LPC analysis
  - Lowering the highpass filter cutoff will improve the bass response of the
    audio
  - Adjusting the lowpass cutoff may have minor effects of pitch estimation
- `alpha`: While the pitch of speech is characterized by the lower frequency
  band, LPC algorithms which characterize the upper vocal tract benefit from an
  exaggeration of high frequency data. A pre-emphasis filter will exaggerate
  this part of the spectrum and lead to crisper, more accurate synthesis
- `format`: ASCII format is ideal for testing and visualization of single
  files. The C and Arduino format produce C headers for use with TMS5220
  emulations
- `no-stop-frame`: An explicit stop frame signals to the TMS5220 that the
  Speak External command has finished executing
- `gain`: Increases the gain of the synthesized signal by adjusting the index
  of the coding table element. Gain offsets greater than the max size of the
  coding table will hit the ceiling
- `max-voiced-gain`: Specifies the maximum gain (dB) of the output signal for
  voiced frames (vowels)
- `max-unvoiced-gain`: Specifies the maximum gain (dB) of the output signal for
  unvoiced frames (consonants)
  - Ensuring that this value hovers around `0.8 * max-voiced-gain` will result
    in the most accurate synthesis of consonant sounds
- `use-repeat-frames`: Detect repeat frames to reduce the size of the bitstream
- `max-frq`: Specifies the maximum representable pitch frequency of the output
  signal
- `min-frq`: Specifies the minimum representable pitch frequency of the output
  signal
