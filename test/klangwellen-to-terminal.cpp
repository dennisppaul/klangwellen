#include "KlangWellen.h"

#include <stdint.h>
#include <stdio.h>

#include <iostream>

#include "Wavetable.h"

klangwellen::Wavetable fWavetable(1024, klangwellen::KlangWellen::DEFAULT_SAMPLING_RATE);
const float            mNumSamples = 512;

int main() {
    std::cout << "+++ SAMPLING_RATE .... : " << klangwellen::KlangWellen::DEFAULT_SAMPLING_RATE << std::endl;

    klangwellen::Wavetable::sine(fWavetable.get_wavetable(), fWavetable.get_wavetable_size());

    for (size_t i = 0; i < mNumSamples; i++) {
        std::cout << fWavetable.process() << ", ";
    }
    std::cout << std::endl;

    return 0;
}
