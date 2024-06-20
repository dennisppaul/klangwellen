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

#include <vector>
#include <cmath>

namespace klangwellen {
    class RootMeanSquare {
    public:
        RootMeanSquare(size_t windowSize = 16) : windowSize(windowSize), sum(0.0f), bufferIndex(0) {
            buffer.resize(windowSize, 0.0f);
            rms = 0.0f;
        }

        float process(float sample) {
            // Remove the oldest sample's contribution
            sum -= buffer[bufferIndex] * buffer[bufferIndex];

            // Add the new sample to the buffer
            buffer[bufferIndex] = sample;
            bufferIndex         = (bufferIndex + 1) % windowSize;

            // Add the new sample's contribution
            sum += sample * sample;

            // Calculate the RMS value
            if (sum > 0) {
                rms = std::sqrt(sum / windowSize);
            } else {
                rms = 0.0f;
            }
            return rms;
        }

        float get_current() const {
            return rms;
        }

    private:
        const size_t       windowSize;
        std::vector<float> buffer;
        size_t             bufferIndex;
        float              sum;
        float              rms;
    };
} // namespace klangwellen