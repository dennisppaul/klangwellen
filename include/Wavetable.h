/*
 * KlangWellen
 *
 * This file is part of the *KlangWellen* library (https://github.com/dennisppaul/klangwellen).
 * Copyright (c) 2024 Dennis P Paul
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * PROCESSOR INTERFACE
 *
 * - [x] float process()
 * - [ ] float process(float)
 * - [ ] void process(Signal&)
 * - [x] void process(float*, uint32_t) *overwrite*
 * - [ ] void process(float*, float*, uint32_t)
 */

#pragma once

#include "KlangWellen.h"

/**
 * plays back a chunk of samples ( i.e arbitrary, single-cycle waveform like sine, triangle, saw or square waves ) at
 * different frequencies and amplitudes.
 */
namespace klangwellen {
    class Wavetable {
    public:
        Wavetable() : Wavetable(KlangWellen::DEFAULT_WAVETABLE_SIZE, KlangWellen::DEFAULT_SAMPLE_RATE) {}
        Wavetable(uint32_t wavetable_size) : Wavetable(wavetable_size, KlangWellen::DEFAULT_SAMPLE_RATE) {}
        Wavetable(uint32_t wavetable_size, uint32_t sample_rate) : Wavetable(new float[wavetable_size], wavetable_size, sample_rate) {
            fDeleteWavetable = true;
        }

        Wavetable(float*   wavetable,
                  uint32_t wavetable_size,
                  uint32_t sample_rate) : fWavetableSize(wavetable_size),
                                          fSampleRate(sample_rate),
                                          fFrequency(0) {
            fWavetable                = wavetable;
            fDeleteWavetable          = false;
            fArrayPtr                 = 0;
            fJitterRange              = 0.0f;
            fEnableJitter             = false;
            fAmplitude                = M_DEFAULT_AMPLITUDE;
            fPhaseOffset              = 0.0f;
            fInterpolationType        = KlangWellen::WAVESHAPE_INTERPOLATE_NONE;
            fDesiredAmplitude         = 0.0f;
            fDesiredAmplitudeFraction = 0.0f;
            fDesiredAmplitudeSteps    = 0;
            set_frequency(M_DEFAULT_FREQUENCY);
        }

        ~Wavetable() {
            if (fDeleteWavetable) {
                delete[] fWavetable;
            }
        }

        static void fill(float* wavetable, uint32_t wavetable_size, uint8_t waveform) {
            // @TODO(add some more interesting waveforms like pulse )
            switch (waveform) {
                case KlangWellen::WAVEFORM_SINE:
                    sine(wavetable, wavetable_size);
                    break;
                case KlangWellen::WAVEFORM_TRIANGLE:
                    triangle(wavetable, wavetable_size);
                    break;
                case KlangWellen::WAVEFORM_SAWTOOTH:
                    sawtooth(wavetable, wavetable_size);
                    break;
                case KlangWellen::WAVEFORM_SQUARE:
                    square(wavetable, wavetable_size);
                    break;
                case KlangWellen::WAVEFORM_NOISE:
                    noise(wavetable, wavetable_size);
                    break;
                default:
                    sine(wavetable, wavetable_size);
            }
        }

        static void fill(float* wavetable, uint32_t wavetable_size, uint8_t harmonics, uint8_t waveform) {
            switch (waveform) {
                case KlangWellen::WAVEFORM_TRIANGLE:
                    triangle(wavetable, wavetable_size, harmonics);
                    break;
                case KlangWellen::WAVEFORM_SAWTOOTH:
                    sawtooth(wavetable, wavetable_size, harmonics);
                    break;
                case KlangWellen::WAVEFORM_SQUARE:
                    square(wavetable, wavetable_size, harmonics);
                    break;
                default:
                    sine(wavetable, wavetable_size);
            }
        }

        static void noise(float* wavetable, uint32_t wavetable_size) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = KlangWellen::random() * 2.0f - 1.0f;
            }
        }

        static void pulse(float* wavetable, uint32_t wavetable_size, float pulse_width) {
            const uint32_t mThreshold = (uint32_t) (wavetable_size * pulse_width);
            for (uint32_t i = 0; i < wavetable_size; i++) {
                if (i < mThreshold) {
                    wavetable[i] = 1.0f;
                } else {
                    wavetable[i] = -1.0f;
                }
            }
        }

        static float* sawtooth(float* wavetable, uint32_t wavetable_size, uint8_t harmonics) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = 0.0f;
            }
            float amps[harmonics];
            for (uint8_t i = 0; i < harmonics; i++) {
                amps[i] = 1.f / (i + 1);
            }
            return fourier_table(wavetable, wavetable_size, harmonics, amps, -0.25f);
        }

        static void sawtooth_ramp(float* wavetable, uint32_t wavetable_size, bool is_ramp_up) {
            const float mSign = is_ramp_up ? -1.0f : 1.0f;
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = mSign * (2.0f * ((float) i / (float) (wavetable_size - 1)) - 1.0f);
            }
        }

        static void sawtooth(float* wavetable, uint32_t wavetable_size) {
            sawtooth(wavetable, wavetable_size, true);
        }

        static void sine(float* wavetable, uint32_t wavetable_size) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = KlangWellen::fast_sin(2.0f * PIf * ((float) i / (float) (wavetable_size)));
            }
        }

        static float* square(float* wavetable, uint32_t wavetable_size, uint8_t harmonics) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = 0.0f;
            }
            float amps[harmonics];
            for (uint8_t i = 0; i < harmonics; i += 2) {
                amps[i] = 1.f / (i + 1);
            }
            return fourier_table(wavetable, wavetable_size, harmonics, amps, -0.25f);
        }

        static void square(float* wavetable, uint32_t wavetable_size) {
            for (uint32_t i = 0; i < wavetable_size / 2; i++) {
                wavetable[i]                      = 1.0f;
                wavetable[i + wavetable_size / 2] = -1.0f;
            }
        }

        static float* triangle(float* wavetable, uint32_t wavetable_size, uint8_t harmonics) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = 0.0f;
            }

            float amps[harmonics];
            for (uint8_t i = 0; i < harmonics; i += 2) {
                amps[i] = 1.f / ((i + 1) * (i + 1));
            }
            return fourier_table(wavetable, wavetable_size, harmonics, amps, 0);
        }

        static void triangle(float* wavetable, uint32_t wavetable_size) {
            const uint32_t q  = wavetable_size / 4;
            const float    qf = wavetable_size * 0.25f;
            for (uint32_t i = 0; i < q; i++) {
                wavetable[i] = i / qf;
                // noinspection PointlessArithmeticExpression
                wavetable[i + (q * 1)] = (qf - i) / qf;
                wavetable[i + (q * 2)] = -i / qf;
                wavetable[i + (q * 3)] = -(qf - i) / qf;
            }
        }

        void set_waveform(uint8_t waveform) {
            fill(fWavetable, fWavetableSize, waveform);
        }

        void set_waveform(uint8_t waveform, uint8_t harmonics) {
            fill(fWavetable, fWavetableSize, harmonics, waveform);
        }

        float get_frequency() {
            return fFrequency;
        }

        void set_frequency(float frequency) {
            const float mNewFrequency = fabs(frequency);
            if (fFrequency != mNewFrequency) {
                fFrequency = mNewFrequency;
                fStepSize  = computeStepSize();
            }
        }

        /**
         * alternative version of `set_frequency` which takes a second paramter that specifies the duration in samples from
         * current to new value. this can prevent unwanted artifacts ( e.g when quickly changing values ). it can also be
         * used to create glissando or portamento effects.
         *
         * @param frequency                         destination frequency
         * @param interpolation_duration_in_samples duration of interpolation in samples
         */
        void set_frequency(float frequency, uint16_t interpolation_duration_in_samples) {
            if (interpolation_duration_in_samples > 0) {
                fDesiredFrequency           = frequency;
                fDesiredFrequencySteps      = interpolation_duration_in_samples;
                const float mFrequencyDelta = fDesiredFrequency - fFrequency;
                fDesiredFrequencyFraction   = mFrequencyDelta / interpolation_duration_in_samples;
            } else {
                set_frequency(frequency);
            }
        }

        float get_offset() {
            return fOffset;
        }

        void set_offset(float offset) {
            fOffset = offset;
        }

        float get_amplitude() {
            return fAmplitude;
        }

        /**
         * @param amplitude amplitude
         */

        void set_amplitude(float amplitude) {
            fAmplitude             = amplitude;
            fDesiredAmplitudeSteps = 0;
        }

        /**
         * alternative version of `set_amplitude` which takes a second paramter that specifies the duration in samples from
         * current to new value. this can prevents unwanted artifacts ( e.g crackling noise when changing amplitude quickly
         * especially on smoother wave shape like sine waves ). it can also be used to create glissando or portamento
         * effects.
         *
         * @param amplitude                         destination amplitude
         * @param interpolation_duration_in_samples duration of interpolation in samples
         */
        void set_amplitude(float amplitude, uint32_t interpolation_duration_in_samples) {
            if (interpolation_duration_in_samples > 0) {
                fDesiredAmplitude           = amplitude;
                fDesiredAmplitudeSteps      = interpolation_duration_in_samples;
                const float mAmplitudeDelta = fDesiredAmplitude - fAmplitude;
                fDesiredAmplitudeFraction   = mAmplitudeDelta / interpolation_duration_in_samples;
            } else {
                set_amplitude(amplitude);
            }
        }

        float* get_wavetable() {
            return fWavetable;
        }

        uint32_t get_wavetable_size() const {
            return fWavetableSize;
        }

        void enable_jitter(bool enable_jitter) {
            fEnableJitter = enable_jitter;
        }

        float get_phase_offset() {
            return fPhaseOffset;
        }

        void set_phase_offset(float phase_offset) {
            fPhaseOffset = phase_offset < 0 ? 1 + phase_offset : phase_offset;
        }

        float get_jitter_range() {
            return fJitterRange;
        }

        void set_jitter_range(float jitter_range) {
            fJitterRange = jitter_range;
        }

        void reset() {
            fSignal   = 0.0f;
            fArrayPtr = 0.0f;
        }

        float current() {
            return fSignal;
        }

        void set_interpolation(int interpolation_type) {
            fInterpolationType = interpolation_type;
        }

        /**
         * set speed of oscillator in seconds per phase
         *
         * @param pFrequency oscillation speed on seconds per phase ( or Hz )
         */
        void set_oscillation_speed(float frequency) {
            set_frequency(1.0f / frequency);
        }

        /**
         * set output value range of oscillator in minimum and maximum. this method affects the oscillator’s amplitude and
         * offset. this methods can be particularily helpful when using an oscillator for non-audio applications ( e.g as an
         * LFO that controlls the frequency ).
         *
         * @param pMin minimum value of oscillator
         * @param pMax maximum value of oscillator
         */
        void set_oscillation_range(float min, float max) {
            const float mDelta = max - min;
            set_amplitude(mDelta * 0.5f);
            set_offset(mDelta * 0.5f + min);
        }

        float process() {
            if (fDesiredAmplitudeSteps > 0) {
                fDesiredAmplitudeSteps--;
                if (fDesiredAmplitudeSteps == 0) {
                    fAmplitude = fDesiredAmplitude;
                } else {
                    fAmplitude += fDesiredAmplitudeFraction;
                }
            }

            if (fDesiredFrequencySteps > 0) {
                fDesiredFrequencySteps--;
                if (fDesiredFrequencySteps == 0) {
                    set_frequency(fDesiredFrequency);
                } else {
                    set_frequency(fFrequency + fDesiredFrequencyFraction);
                }
            }

            switch (fInterpolationType) {
                case KlangWellen::WAVESHAPE_INTERPOLATE_LINEAR:
                    fSignal = next_sample_interpolate_linear();
                    break;
                case KlangWellen::WAVESHAPE_INTERPOLATE_CUBIC:
                    fSignal = next_sample_interpolate_cubic();
                    break;
                default:
                    fSignal = next_sample();
            }

            fSignal *= fAmplitude;
            fSignal += fOffset;
            return fSignal;
        }

        void process(float* signal_buffer, const uint32_t buffer_length = KlangWellen::DEFAULT_AUDIOBLOCK_SIZE) {
            for (uint16_t i = 0; i < buffer_length; i++) {
                signal_buffer[i] = process();
            }
        }

    private:
        static constexpr float PIf                 = (float) PI;
        static constexpr float TWO_PIf             = (float) TWO_PI;
        static constexpr float M_DEFAULT_AMPLITUDE = 0.75f;
        static constexpr float M_DEFAULT_FREQUENCY = 220.0f;
        const uint32_t         fWavetableSize;
        const uint32_t         fSampleRate;
        float*                 fWavetable;
        bool                   fDeleteWavetable;
        float                  fAmplitude;
        float                  fArrayPtr;
        float                  fDesiredAmplitude;
        float                  fDesiredAmplitudeFraction;
        uint16_t               fDesiredAmplitudeSteps;
        float                  fDesiredFrequency;
        float                  fDesiredFrequencyFraction;
        uint16_t               fDesiredFrequencySteps;
        bool                   fEnableJitter;
        float                  fFrequency;
        int                    fInterpolationType;
        float                  fJitterRange;
        float                  fOffset;
        float                  fPhaseOffset;
        float                  fSignal;
        float                  fStepSize;

        static float* fourier_table(float* pWavetable, uint32_t wavetable_size, uint8_t pHarmonics, float* pAmps, float pPhase) {
            float  a;
            double w;
            pPhase *= PIf * 2;
            for (uint8_t i = 0; i < pHarmonics; i++) {
                for (uint32_t n = 0; n < wavetable_size; n++) {
                    a = pAmps[i];
                    // a = (pAmps != nullptr) ? pAmps[i] : 1.f;
                    w = (i + 1) * (n * 2 * PI / wavetable_size);
                    pWavetable[n] += (float) (a * KlangWellen::cos((float) w + pPhase));
                }
            }
            normalise_table(pWavetable, wavetable_size);
            return pWavetable;
        }

        static void normalise_table(float* pWavetable, uint32_t wavetable_size) {
            uint32_t n;
            float    max = 0.f;
            for (n = 0; n < wavetable_size; n++) {
                max = std::max(pWavetable[n], max);
            }
            if (max > 0) {
                for (n = 0; n < wavetable_size; n++) {
                    pWavetable[n] /= max;
                }
            }
        }

        void advance_array_ptr() {
            // mArrayPtr += mStepSize * (mEnableJitter ? (klangwellen::KlangWellen::random() * mJitterRange + 1.0f) : 1.0f);
            // mArrayPtr += mStepSize;
            fArrayPtr += fStepSize * (fEnableJitter ? (KlangWellen::random() * fJitterRange + 1.0f) : 1.0f);
            while (fArrayPtr >= fWavetableSize) {
                fArrayPtr -= fWavetableSize;
            }
            while (fArrayPtr < 0) {
                fArrayPtr += fWavetableSize;
            }
        }

        float computeStepSize() {
            return fFrequency * ((float) fWavetableSize / (float) fSampleRate);
        }

        float next_sample() {
            const float mOutput = fWavetable[(int) (fArrayPtr)];
            advance_array_ptr();
            return mOutput;
        }

        float next_sample_interpolate_cubic() {
            const uint32_t mSampleOffset   = (int) (fPhaseOffset * fWavetableSize) % fWavetableSize;
            const float    mArrayPtrOffset = fArrayPtr + mSampleOffset;
            /* cubic interpolation */
            const float    frac   = mArrayPtrOffset - (int) mArrayPtrOffset;
            const float    a      = (int) mArrayPtrOffset > 0 ? fWavetable[(int) mArrayPtrOffset - 1] : fWavetable[fWavetableSize - 1];
            const float    b      = fWavetable[((int) mArrayPtrOffset) % fWavetableSize];
            const uint32_t p1     = (uint32_t) mArrayPtrOffset + 1;
            const float    c      = fWavetable[p1 >= fWavetableSize ? p1 - fWavetableSize : p1];
            const uint32_t p2     = (uint32_t) mArrayPtrOffset + 2;
            const float    d      = fWavetable[p2 >= fWavetableSize ? p2 - fWavetableSize : p2];
            const float    tmp    = d + 3.0f * b;
            const float    fracsq = frac * frac;
            const float    fracb  = frac * fracsq;
            const float    mOutput =
                (fracb * (-a - 3.f * c + tmp) / 6.f + fracsq * ((a + c) / 2.f - b) + frac * (c + (-2.f * a - tmp) / 6.f) + b);
            advance_array_ptr();
            return mOutput;
        }

        float next_sample_interpolate_linear() {
            const uint32_t mSampleOffset   = (uint32_t) (fPhaseOffset * fWavetableSize) % fWavetableSize;
            const float    mArrayPtrOffset = fArrayPtr + mSampleOffset;
            /* linear interpolation */
            const float    mFrac   = mArrayPtrOffset - (int) mArrayPtrOffset;
            const float    a       = fWavetable[(int) mArrayPtrOffset];
            const uint32_t p1      = (uint32_t) mArrayPtrOffset + 1;
            const float    b       = fWavetable[p1 >= fWavetableSize ? p1 - fWavetableSize : p1];
            const float    mOutput = a + mFrac * (b - a);
            advance_array_ptr();
            return mOutput;
        }
    };
} // namespace klangwellen
