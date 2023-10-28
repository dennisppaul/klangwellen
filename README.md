# KlangWellen

*KlangWellen* is a C++ library for processing audio signals. each processor ( or class ) is designed around a central `process` method that can either receive and/or emitt samples.

## processor interface

*KlangWellen* refrains from implementing `process` interfaces with the know C++ techniques ( i.e virtual functions ). however, most processors supply a *conceptual* interface for processing signals in one or more of the following `process` methods:

1. [ ] `float process()` :: generate single **mono** signal
2. [ ] `float process(float)‌` :: single **mono** signal
3. [ ] `void process(Signal&)` :: single **stereo** signal
4. [ ] `void process(float*, uint32_t)` :: block of **mono** signals
5. [ ] `void process(float*, float*, uint32_t)` :: block of **stereo** signal

note, that *1. `float process()`* only generates a signal. *‌2. `float process(float)‌`* is encouraged to respect the input signal. however, in some cases ( in stereo signal generators ) it may overwrite the input signal regardless.

developers are encouraged to only implement the variants that make sense for a specific processors. developers are also encouraged to add a comment to the head of a processor showing which `process` methods are available.

```
/**
 * PROCESSOR INTERFACE
 *
 * - [ ] float process()
 * - [ ] float process(float)‌
 * - [ ] void process(Signal&)
 * - [ ] void process(float*, uint32_t)
 * - [ ] void process(float*, float*, uint32_t)
 */
```