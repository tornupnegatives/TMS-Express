# TMS-Express 🗣
TMS Express performs linear predictive coding (LPC) analysis on speech data and generates bitstreams compatible with the TMS5220 Voice Synthesis Processor

# Dependencies
TMS Express is built with `cmake` and relies on `libsndfile` and `libsamplerate` for importing and resampling audio input. The command-line interface is provided by `Boost`, and `googletest` is used for unit testing. Please ensure these libraries are present on the system (`googletest` will be installed locally via `cmake`, and `libsndfile` will be installed as a dependency of `libsamplerate`).


## On macOS
```shell
$ brew install cmake libsamplerate boost
```

## On Debian/Ubuntu
```shell
$ sudo apt install cmake libsamplerate-dev libboost-all-dev
```

## Compiling
```shell
$ cmake -S . -b build
$ cd build && make -j
```

## Usage
At minimum, TMS Express requires an input audio file (any format supported by `libsndfile`) and an output path for the bitstream.

```shell
$ tmsexpress --input <path/to/input/audio> --output <path/to/output/bistream>
```

### Additional Options
- `width`: Speech data is separated into small windows/segments, each of which are analyzed individually. This is because small enough segments of speech data are roughly periodic and their behavior may be generalized. An ideal window width is between 22.5-25 ms
  - Values above and below the recommendation will artificially speed up and slow down speech, respectively
- `highpass` and `lowpass`: Speech data occupies a relatively small frequency band compared to what digital audio files are capable of representing. Filtering out unnecessary frequencies may lead to more accurated LPC analysis
  - Lowering the highpass filter cutoff will improve the bass response of the audio
  - Adjusting the lowpass cutoff may have minor effects of pitch estimation
- `alpha`: While the pitch of speech is characterized by the lower frequency band, LPC algorithms which characterize the upper vocal tract benefit from an exaggeration of high frequency data. A pre-emphasis filter will exaggerate this part of the spectrum and lead to crisper, more accurate synthesis
- `hex-prefix`: Prefixes each hex byte of the bitstream with `0x`
- `separator`: Specifies the character for delimiting hex bytes in the bitstream
- `stop-frame`: Include an explicit stop frame to signal to the TMS5220 that the Speak External command has finished executing
- `gain`: Increases the gain of the synthesized signal by adjusting the index of the coding table element. Gain offsets greater than the max size of the coding table will hit the ceiling
- `max-voiced-gain`: Specifies the maximum gain (dB) of the output signal for voiced frames (vowels)
- `max-unvoiced-gain`: Specifies the maximum gain (dB) of the output signal for unvoiced frames (consonants)
  - Ensuring that this value hovers around `0.8 * max-voiced-gain` will result in the most accurate synthesis of consonant sounds
- `repeat`: Detect repeat frames to reduce the size of the bitstream
- `max-frq`: Specifies the maximum representable pitch frequency of the output signal
- `min-frq`: Specifies the minimum representable pitch frequency of the output signal
- `verbose`: Output frame data to the console
