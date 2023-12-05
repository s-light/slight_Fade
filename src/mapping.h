/******************************************************************************

    some mapping helpers..

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
/******************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Stefan Krüger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/



#ifndef mapping_H_
#define mapping_H_


// include Core Arduino functionality
#include <Arduino.h>

#include <algorithm>
#include <array>



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// clamp / clip / constrain / limit

// https://stackoverflow.com/a/8941363/574981
// https://www.arduino.cc/reference/en/language/functions/math/constrain/
// template<class T>
// const T& clamp(const T& n, const T& lower, const T& upper) {
//     if(n < lower) {
//         return lower;
//     } else if(upper < n) {
//         return upper;
//     } else {
//         return n;
//     }
// }

// as of C++17 clamp is part of std::

template <typename T>
const T& clamp(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

template <typename T>
const T& clamp01(const T& n) {
    const T& lower = 0.0;
    const T& upper = 1.0;
    return std::max(lower, std::min(n, upper));
}

// template <typename T1, typename T2, typename T3>
// const T1& clamp(const T1& n, const T2& lower, const T3& upper) {
//     return std::max(lower, std::min(n, upper));
// }

template<class T>
const T& limit(const T& x, const T& top) {
    if(x > top) {
        return top;
    } else {
        return x;
    }
}


template<class T>
const T& overflow01(const T& x) {
    while (x > 1.0) {
        x -= 1.0;
    }
    return x;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// map_range

template<class T>
T map_range(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1, class T2>
T2 map_range(T1 x, T1 in_min, T1 in_max, T2 out_min, T2 out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1, class T2>
T2 map_range(T1 x, T2 in_min, T2 in_max, T2 out_min, T2 out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1, class T2, class T3>
T3 map_range(T1 x, T2 in_min, T2 in_max, T3 out_min, T3 out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1, class T2, class T3, class T4>
T4 map_range(T1 x, T2 in_min, T2 in_max, T3 out_min, T3 out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
T6 map_range(T1 x, T2 in_min, T3 in_max, T4 out_min, T5 out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<class T1>
T1 map_range_clamped(
    T1 x, T1 in_min, T1 in_max, T1 out_min, T1 out_max
) {
    // x = std::clamp(x, in_min, in_max);  // C++17
    x = clamp(x, in_min, in_max);
    return map_range(x, in_min, in_max, out_min, out_max);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// map_range_01_to

template<class T1, class T2>
T2 map_range_01_to(T1 x, T2 out_min, T2 out_max) {
    return x * (out_max - out_min) / 1.0 + out_min;
}

template<class T1, class T2, class T3>
T3 map_range_01_to(T1 x, T2 out_min, T3 out_max) {
    return x * (out_max - out_min) / 1.0 + out_min;
}

// template<class T>
// T map_range_11_to(T x, T out_min, T out_max) {
//     return (x - -1.0) * (out_max - out_min) / (1.0 - -1.0) + out_min;
// }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// map_range_01_to_0x

template<class T1, class T2>
T2 map_range_01_to_0n(T1 x, T2 out_max) {
    return x * out_max;
}

// template<class T1, class T2, class T3>
// T3 map_range_01_to_0n(T1 x, T2 out_max) {
//     return x * out_max;
// }


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// normalize_to_01

template<class T1, class T2>
T2 normalize_to_01(T1 x, T1 in_min, T1 in_max) {
    return ((x - in_min) * 1.0) / (in_max - in_min);
}


template<class T1>
double normalize_to_01(T1 x, T1 in_min, T1 in_max) {
    return ((x - in_min) * 1.0) / (in_max - in_min);
}

template<class T1, class T2>
double normalize_to_01(T1 x, T2 in_min, T2 in_max) {
    return ((x - in_min) * 1.0) / (in_max - in_min);
}

template<class T1, class T2, class T3>
T3 normalize_to_01(T1 x, T2 in_min, T2 in_max) {
    return ((x - in_min) * 1.0) / ((in_max - in_min) * 1.0);
}

// template<class T1, class T2, class T3>
// T3 normalize_to_01(T1 x, T2 in_min, T2 in_max) {
//     return ((x - in_min) * 1.0) / (in_max - in_min);
// }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// map_range_0n_to_01

template<class T1>
float map_range_0n_to_01(T1 x, T1 in_max) {
    return 1.0 * x / in_max;
}

template<class T1, class T2>
float map_range_0n_to_01(T1 x, T2 in_max) {
    return 1.0 * x / in_max;
}


template<class T1>
T1 map_range_0n_to_10(T1 x, T1 in_max) {
    return x * -1.0 / in_max + 1.0;
}

template<class T1, class T2>
float map_range_0n_to_10(T1 x, T2 in_max) {
    return x * -1.0 / in_max + 1.0;
}

// template<class T1, class T2>
// T2 map_range_0n_to_10(T1 x, T1 in_max) {
//     return static_cast<T2>(1) * x / in_max;
// }




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MultiMap / multi_map

// http://arduino.cc/playground/Main/MultiMap
template<class T, uint8_t N>
T multi_map(T val, std::array<T, N> _in, std::array<T, N> _out) {
    // take care the value is within range
    // val = constrain(val, _in[0], _in[N-1]);
    if (val <= _in[0]) {
        return _out[0];
    }
    if (val >= _in[N-1]) {
        return _out[N-1];
    }

    // search right interval
    uint8_t pos = 1;  // _in[0] allready tested
    while (val > _in[pos]) {
        pos++;
    }

    // this will handle all exact "points" in the _in array
    if (val == _in[pos]) {
        return _out[pos];
    }

    // interpolate in the right segment for the rest
    return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}


template<class T, uint8_t N>
class MultiMap {
 public:
    MultiMap(
        const std::array<T, N> in,
        const std::array<T, N> out
    ):
        in(in),
        out(out)
    {}

    T mapit(T val) {
        // take care the value is within range
        // val = std::clamp(val, _in[0], _in[N-1]);
        if (val <= in[0]) {
            return out[0];
        }
        if (val >= in[N-1]) {
            return out[N-1];
        }

        // search right interval
        uint8_t pos = 1;  // _in[0] allready tested
        while (val > in[pos]) {
            pos++;
        }

        // this will handle all exact "points" in the _in array
        if (val == in[pos]) {
            return out[pos];
        }

        // interpolate in the right segment for the rest
        return map_range(val, in[pos-1], in[pos], out[pos-1], out[pos]);
    }
 private:
    // std:array<T, N> in;
    // std:array<T, N> out;
    const std::array<T, N> in;
    const std::array<T, N> out;
};

#endif  // mapping_H_
