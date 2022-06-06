# TMS-Express 🗣
TMS Express performs linear predictive coding (LPC) analysis on speech data and generates bitstreams compatible with the TMS5220 Voice Synthesis Processor

# Dependencies
TMS Express relies on `libsndfile` and `libsamplerate` for importing and resampling audio input. The command-line interface is provided by `Boost`, and `googletest` is used for unit testing. Please ensure these libraries are present on the system before invoking the following build commands:

```shell
$ cmake -S . -b build
$ cd build && make -j
```

## Usage
At minimum, TMS Express requires an input audio file (any format supported by `libsndfile`) and an output path for the bitstream.

```shell
$ TMS_Express --input <path/to/input/audio> --output <path/to/output/bistream>
```

### Additional Options
- `width`: Speech data is separated into small windows/segments, each of which are analyzed individually. This is because small enough segments of speech data are roughly periodic and their behavior may be generalized. An ideal window width is between 22.5-25 ms
- `highpass` and `lowpass`: Speech data occupies a relatively small frequency band compared to what digital audio files are capable of representing. Filtering out unnecessary frequencies may lead to more accurated LPC analysis
- `alpha`: While the pitch of speech is characterized by the lower frequency band, LPC algorithms which characterize the upper vocal tract benefit from an exaggeration of high frequency data. A pre-emphasis filter will exaggerate this part of the spectrum and lead to crisper, more accurate synthesis
- `include-prefix`: Prefixes each hex byte of the bitstream with `0x`
- `separator`: Specifies the character for delimiting hex bytes in the bitstream
- `gain`: Increases the gain of the synthesized signal by adjusting the index of the coding table element. Gain offsets greater than the max size of the coding table will hit the ceiling
- `max-voiced-gain`: Specifies the maximum gain (dB) of the output signal for voiced frames (vowels)
- `max-unvoiced-gain`: Specifies the maximum gain (dB) of the output signal for unvoiced frames (consonants)
- `max-frq`: Specifies the maximum representable pitch frequency of the output signal
- `min-frq`: Specifies the minimum representable pitch frequency of the output signal
