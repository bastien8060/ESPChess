#include "PrintStream.h"

void powermode(int mode) {
    if (mode == LOW) {
        setCpuFrequencyMhz(80);
    }
    if (mode == HIGH) {
        setCpuFrequencyMhz(240);
    }
}
