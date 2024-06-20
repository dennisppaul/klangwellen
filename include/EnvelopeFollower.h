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
* - [ ] void process(Signal&)
* - [ ] void process(float*, uint32_t)
* - [ ] void process(float*, float*, uint32_t)
*/

#pragma once

#include <iostream>
#include <cmath>

#include "KlangWellen.h"

namespace klangwellen {
    class EnvelopeFollower {
    public:
        EnvelopeFollower(float attackTime  = 0.01,
                         float releaseTime = 0.1,
                         float sampleRate  = KlangWellen::DEFAULT_SAMPLING_RATE)
            : attackTime(attackTime), releaseTime(releaseTime), sampleRate(sampleRate) {
            setAttackTime(attackTime);
            setReleaseTime(releaseTime);
            envelope = 0.0f;
        }

        void setAttackTime(float attackTime) {
            this->attackTime = attackTime;
            attackCoef       = std::exp(-1.0f / (attackTime * sampleRate));
        }

        void setReleaseTime(float releaseTime) {
            this->releaseTime = releaseTime;
            releaseCoef       = std::exp(-1.0f / (releaseTime * sampleRate));
        }

        float process(float input) {
            float absInput = std::fabs(input);
            if (absInput > envelope) {
                // Attack
                envelope = attackCoef * (envelope - absInput) + absInput;
            } else {
                // Release
                envelope = releaseCoef * (envelope - absInput) + absInput;
            }
            return envelope;
        }

        float get_current() const {
            return envelope;
        }

    private:
        float attackTime;
        float releaseTime;
        float sampleRate;
        float attackCoef;
        float releaseCoef;
        float envelope;
    };
} // namespace klangwellen