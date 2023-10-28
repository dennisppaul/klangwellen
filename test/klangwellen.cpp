#include "KlangWellen.h"

#include <stdint.h>
#include <stdio.h>

#include <iostream>

#include "ADSR.h"
#include "Gain.h"
#include "Wavetable.h"

klangwellen::ADSR      fADSR;
klangwellen::Gain      fGain;
klangwellen::Wavetable fWavetable(new float[11024], 1024, 48000);

float mAudioBuffer[2][1024];

int main() {
    std::cout << "# KlangWellen" << std::endl
              << std::endl;
    std::cout << "SAMPLING_RATE: " << klangwellen::KlangWellen::DEFAULT_SAMPLING_RATE << std::endl;
    std::cout << "ADSR         : " << fADSR.output() << std::endl;
    std::cout << "buffer size  : " << (sizeof(mAudioBuffer) / sizeof(float)) << std::endl;

    for (size_t i = 0; i < fWavetable.get_wavetable_size(); i++) {
        fWavetable.get_wavetable()[i] = klangwellen::KlangWellen::random();
    }

    klangwellen::Wavetable::sine(fWavetable.get_wavetable(), fWavetable.get_wavetable_size());

    fGain.process(mAudioBuffer[0], mAudioBuffer[1]);
    fWavetable.output();
    fADSR.output();

    fADSR.start();
    for (size_t i = 0; i < klangwellen::KlangWellen::DEFAULT_SAMPLING_RATE / 6; i++) {
        // std::cout << fADSR.output() * fWavetable.output();
        // std::cout << ", ";
        if (i == klangwellen::KlangWellen::DEFAULT_SAMPLING_RATE / 12) {
            fADSR.stop();
        }
    }
    std::cout << std::endl;

    return 0;
}
