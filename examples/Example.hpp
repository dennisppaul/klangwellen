#ifndef _EXAMPLE_HPP_
#define _EXAMPLE_HPP_

#include "KlangWellen.h"
#include "KlangstromApplication.hpp"

using namespace klangwellen;
using namespace klangstrom;

class ExampleADSR : public KlangstromApplication {
public:

    void setup() override {
    }

    void audioblock(SIGNAL_TYPE* pOutputLeft, SIGNAL_TYPE* pOutputRight, SIGNAL_TYPE* pInputLeft, SIGNAL_TYPE* pInputRight) override {
    }
};

#endif /* _EXAMPLE_HPP_ */
