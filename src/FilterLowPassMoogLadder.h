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
 * - [ ] float process()
 * - [x] float process(float)
 * - [ ] void process(AudioSignal&)
 * - [x] void process(float*, uint32_t)
 * - [ ] void process(float*, float*, uint32_t)
 */

#pragma once

#include <stdint.h>

#include <algorithm>
#include <cmath>

#include "KlangWellen.h"
#include "AudioSignal.h"

/**
 * low-pass filter implementing the <em>Moog Ladder</em>.
 */
namespace klangwellen {
    class FilterLowPassMoogLadder {
        /*
         * Low Pass Filter ( Moog Ladder )
         * <p>
         * Ported from soundpipe
         * <p>
         * Original author(s) : Victor Lazzarini, John ffitch (fast tanh), Bob Moog
         */

        const float fSampleRate;
        float       fCutoffFrequency;
        float       fDelay[6];
        float       fOldAcr;
        float       fOldFreq;
        float       fOldRes;
        float       fOldTune;
        float       fResonance;
        float       fTanhstg[3];

    public:
        FilterLowPassMoogLadder() : FilterLowPassMoogLadder(KlangWellen::DEFAULT_SAMPLE_RATE) {}

        explicit FilterLowPassMoogLadder(uint32_t sample_rate) : fSampleRate(sample_rate) {
            fResonance       = 0.4f;
            fCutoffFrequency = 1000.0f;

            for (uint8_t i = 0; i < 6; i++) {
                fDelay[i]       = 0.0f;
                fTanhstg[i % 3] = 0.0f;
            }

            fOldFreq = 0.0f;
            fOldRes  = -1.0f;
        }

        void process(float*         signal_buffer,
                     const uint32_t length = KlangWellen::DEFAULT_SAMPLE_RATE) {
            for (uint32_t i = 0; i < length; i++) {
                signal_buffer[i] = process(signal_buffer[i]);
            }
        }

        float process(float signal) {
            const float freq = fCutoffFrequency;
            const float res  = std::max(fResonance, 0.0f);
            float       res4;
            float       stg[4];
            float       acr, tune;
            const float THERMAL = 0.000025f;

            if (fOldFreq != freq || fOldRes != res) {
                float f, fc, fc2, fc3, fcr;
                fOldFreq = freq;
                fc       = (freq / fSampleRate);
                f        = 0.5f * fc;
                fc2      = fc * fc;
                fc3      = fc2 * fc2;

                fcr  = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
                acr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
                tune = ((1.0f - std::exp(-((2 * M_PI) * f * fcr))) / THERMAL);

                fOldRes  = res;
                fOldAcr  = acr;
                fOldTune = tune;
            } else {
                //            res = mOldRes;
                acr  = fOldAcr;
                tune = fOldTune;
            }

            res4 = 4.0f * res * acr;

            for (uint8_t j = 0; j < 2; j++) {
                signal -= res4 * fDelay[5];
                fDelay[0] = stg[0] = fDelay[0] + tune * (my_tanh(signal * THERMAL) - fTanhstg[0]);
                for (uint8_t k = 1; k < 4; k++) {
                    signal    = stg[k - 1];
                    stg[k]    = fDelay[k] + tune * ((fTanhstg[k - 1] = my_tanh(signal * THERMAL)) - (k != 3 ? fTanhstg[k] : my_tanh(fDelay[k] * THERMAL)));
                    fDelay[k] = stg[k];
                }
                fDelay[5] = (stg[3] + fDelay[4]) * 0.5f;
                fDelay[4] = stg[3];
            }
            return fDelay[5];
        }

        float get_frequency() {
            return fCutoffFrequency;
        }

        /**
         * @param pCutoffFrequency cutoff frequency in Hz
         */
        void set_frequency(float pCutoffFrequency) {
            fCutoffFrequency = pCutoffFrequency;
        }

        float get_resonance() {
            return fResonance;
        }

        /**
         * @param pResonance resonance factor [0.0, 1.0] ( becomes unstable close to 1.0 )
         */
        void set_resonance(float pResonance) {
            fResonance = pResonance;
        }

    private:
        float my_tanh(float x) {
            int8_t sign = 1;
            if (x < 0) {
                sign = -1;
                x    = -x;
                return x * sign;
            } else if (x >= 4.0f) {
                return sign;
            } else if (x < 0.5f) {
                return x * sign;
            }
            return sign * (float) tanh(x);
        }
    };
} // namespace klangwellen