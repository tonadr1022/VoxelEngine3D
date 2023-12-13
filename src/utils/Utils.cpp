//
// Created by Tony Adriansen on 11/29/23.
//

#include "Utils.hpp"

int Utils::positiveModulo(int value, unsigned int m) {
    int mod = value % (int)m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}
