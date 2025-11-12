#define _USE_MATH_DEFINES
#include <cmath>

#include "ru_alexander1248_jposit_PositsJNI.h"
#include <include/softposit_cpp.h>
#include <iostream>


//    ====================================================================
//                                  MATH
//    ====================================================================
static const posit32 LN_2 = log(2);
static const posit32 ONE = 1;
static const posit32 pi_2_32 = M_PI_2;
static const posit16 pi_2_16 = M_PI_2;
static constexpr auto table_size = 128;
posit32 sin_table_32[table_size];
posit32 cos_table_32[table_size];
posit16 sin_table_16[table_size];
posit16 cos_table_16[table_size];

inline posit32 fromJInt(const jint val) {
    posit32 p;
    p.value = val;
    return p;
}

inline posit16 fromJShort(const jshort val) {
    posit16 p;
    p.value = val;
    return p;
}

posit32 sign(const posit32 posit) {
    const auto bits = posit.value;
    if (bits == 0) return 0;
    return bits & 0x80000000 ? -1 : 1;
}

posit16 sign(const posit16 posit) {
    const auto bits = posit.value;
    if (bits == 0) return 0;
    return bits & 0x8000 ? -1 : 1;
}

template <typename T>
T abs(const T posit) {
    return sign(posit) < 0 ? -posit : posit;
}

posit32 tailor_log_recursive( // NOLINT(*-no-recursion)
    const posit32 x,
    const int order,
    const int stopOrder) {
    if (order > stopOrder) return 1;
    const posit32 order2 = order * 2;
    const auto o = (order2 - ONE) / (order2 + ONE);
    return 1 + x * o * tailor_log_recursive(x, order + 1, stopOrder);
}

posit32 tailor_log_approx(const posit32 x) {
    const auto z = (x - ONE) / (x + ONE);
    return 2 * z * tailor_log_recursive(z * z, 1, 4);
}

posit32 fast_log(const posit32 x) {
    const auto bits = x.value;
    if (bits & 0x80000000) return static_cast<jint>(posit32().toNaR().value);
    const uint32_t regime_sign = (bits >> 30) & 1; // первый бит после знака
    uint32_t regime_count = 1;

    uint32_t tmp = bits << 2; // сдвигаем чтобы проверить остальные биты режима
    while ((tmp >> 31 & 1) == regime_sign && regime_count < 31) {
        regime_count++;
        tmp <<= 1;
    }

    const uint32_t k = regime_sign ? (regime_count - 1) : -(regime_count - 1);
    uint32_t exponent;
    // 3. Exponent
    uint32_t exp_shift = 1 + regime_count; // биты до экспоненты (1 знак + regime_count)
    if (exp_shift >= 32) {
        exponent = 0;
    } else {
        const uint32_t remaining_bits = 32 - exp_shift;
        const uint32_t e_bits = remaining_bits < 2 ? remaining_bits : 2;
        exponent = (bits << exp_shift) >> (32 - e_bits);
        exp_shift += e_bits;
    }
    posit32 fraction;
    // 4. Fraction
    const uint32_t fraction_bits = 32 - exp_shift;
    if (fraction_bits <= 0) {
        fraction = 0.0;
    } else {
        const uint32_t frac_mask = (1u << fraction_bits) - 1;
        const uint32_t frac_bits = bits & frac_mask;
        fraction = static_cast<double>(frac_bits) / (1 << fraction_bits);
    }
    return tailor_log_approx(fraction + 1) + LN_2 * (k * 4 + exponent);
}

template<typename T>
T tailor_sin_recursive( // NOLINT(*-no-recursion)
    const T x,
    const int order,
    const int stopOrder,
    const int shift) {
    if (order > stopOrder) return 1;
    const auto order2 = order * 2;
    const auto div = order2 * (order2 + shift);
    return 1 - x * tailor_sin_recursive(x, order + 1, stopOrder, shift) / div;
}

template<typename T>
T tailor_sin_approx(const T x) {
    return x * tailor_sin_recursive(x * x, 1, 4, 1);
}

template<typename T>
T tailor_cos_approx(const T x) {
    return tailor_sin_recursive(x * x, 1, 4, -1);
}

template<typename T>
T intel_sin(const T x, T sin_table[], T cos_table[]) {
    const T pi = M_PI;
    const T pi2 = M_PI * 2;
    const auto px = abs(x);
    const auto cx = rint(px / pi2 - 0.499999);
    const auto fx = px - cx * pi2;
    constexpr auto half_size = table_size >> 1;
    const auto N = rint(fx * half_size / pi - 0.499999);
    const auto r = fx - N * pi / half_size;
    const auto i = static_cast<int>(N.toDouble());
    auto result = sin_table[i] * tailor_cos_approx(r) + cos_table[i] * tailor_sin_approx(r);
    result *= sign(x);
    return result;
}

//    ====================================================================
//                                  INIT
//    ====================================================================
JNIEXPORT void JNICALL Java_ru_alexander1248_jposit_PositsJNI_init
(JNIEnv *, jclass) {
    std::cout << "Initializing SoftPosits..." << std::endl;
    for (auto i = 0; i < table_size; i++) {
        const auto cos_val = cos(2 * M_PI * i / table_size);
        cos_table_32[i] = cos_val;
        cos_table_16[i] = cos_val;

        const auto sin_val = sin(2 * M_PI * i / table_size);
        sin_table_32[i] = sin_val;
        sin_table_16[i] = sin_val;
    }
    std::cout << "SoftPosits initialized!" << std::endl;
}

//    ====================================================================
//                                  POSIT_32
//    ====================================================================
JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1nar
(JNIEnv *, jclass) {
    return static_cast<jint>(posit32().toNaR().value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1fromFloat
(JNIEnv *, jclass, const jfloat val) {
    const posit32 x = val;
    return static_cast<jint>(x.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1fromDouble
(JNIEnv *, jclass, const jdouble val) {
    const posit32 x = val;
    return static_cast<jint>(x.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1fromInt
(JNIEnv *, jclass, const jint val) {
    const posit32 x = val;
    return static_cast<jint>(x.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1fromLong
(JNIEnv *, jclass, const jlong val) {
    const posit32 x = static_cast<double>(val);
    return static_cast<jint>(x.value);
}

JNIEXPORT jfloat JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1toFloat
(JNIEnv *, jclass, const jint posit) {
    return static_cast<jfloat>(fromJInt(posit).toDouble());
}

JNIEXPORT jdouble JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1toDouble
(JNIEnv *, jclass, const jint posit) {
    return fromJInt(posit).toDouble();
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1toInt
(JNIEnv *, jclass, const jint posit) {
    return static_cast<jint>(fromJInt(posit).toDouble());
}

JNIEXPORT jlong JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1toLong
(JNIEnv *, jclass, const jint posit) {
    return static_cast<jint>(fromJInt(posit).toDouble());
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1add
(JNIEnv *, jclass, const jint a, const jint b) {
    const auto res = fromJInt(a) + fromJInt(b);
    return static_cast<jint>(res.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1sub
(JNIEnv *, jclass, const jint a, const jint b) {\
    const auto res = fromJInt(a) - fromJInt(b);
    return static_cast<jint>(res.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1mul
(JNIEnv *, jclass, const jint a, const jint b) {
    const auto res = fromJInt(a) * fromJInt(b);
    return static_cast<jint>(res.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1div
(JNIEnv *, jclass, const jint a, const jint b) {
    const auto res = fromJInt(a) / fromJInt(b);
    return static_cast<jint>(res.value);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1eq
(JNIEnv *, jclass, const jint a, const jint b) {
    return fromJInt(a) == fromJInt(b);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1le
(JNIEnv *, jclass, const jint a, const jint b) {
    return fromJInt(a) <= fromJInt(b);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1lt
(JNIEnv *, jclass, const jint a, const jint b) {
    return fromJInt(a) < fromJInt(b);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1neg
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>((-fromJInt(val)).value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1abs
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(abs(fromJInt(val)).value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1sign
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(sign(fromJInt(val)).value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1round
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(rint(fromJInt(val)).value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1sqrt
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(sqrt(fromJInt(val)).value);
}


JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1exp
(JNIEnv *, jclass, const jint val) {
    const auto p = fromJInt(val);
    const posit32 result = exp(p.toDouble());
    return static_cast<jint>(result.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1log
(JNIEnv *, jclass, const jint val) {
    const auto p = fromJInt(val);
    const posit32 result = log(p.toDouble());
    return static_cast<jint>(result.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1sin
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(intel_sin(fromJInt(val), sin_table_32, cos_table_32).value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1cos
(JNIEnv *, jclass, const jint val) {
    return static_cast<jint>(intel_sin(fromJInt(val) + pi_2_32, sin_table_32, cos_table_32).value);
}

//    ====================================================================
//                                  POSIT_16
//    ====================================================================
JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1nar
(JNIEnv *, jclass) {
    return static_cast<jshort>(posit16().toNaR().value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1fromFloat
(JNIEnv *, jclass, const jfloat val) {
    const posit16 x = val;
    return static_cast<jshort>(x.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1fromDouble
(JNIEnv *, jclass, const jdouble val) {
    const posit16 x = val;
    return static_cast<jshort>(x.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1fromInt
(JNIEnv *, jclass, const jint val) {
    const posit16 x = val;
    return static_cast<jshort>(x.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1fromLong
(JNIEnv *, jclass, const jlong val) {
    const posit16 x = static_cast<double>(val);
    return static_cast<jshort>(x.value);
}

JNIEXPORT jfloat JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1toFloat
(JNIEnv *, jclass, const jshort posit) {
    return static_cast<jfloat>(fromJShort(posit).toDouble());
}

JNIEXPORT jdouble JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1toDouble
(JNIEnv *, jclass, const jshort posit) {
    return fromJShort(posit).toDouble();
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1toInt
(JNIEnv *, jclass, const jshort posit) {
    return static_cast<jint>(fromJShort(posit).toDouble());
}

JNIEXPORT jlong JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1toLong
(JNIEnv *, jclass, const jshort posit) {
    return static_cast<jlong>(fromJShort(posit).toDouble());
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1add
(JNIEnv *, jclass, const jshort a, const jshort b) {
    const auto res = fromJShort(a) + fromJShort(b);
    return static_cast<jshort>(res.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1sub
(JNIEnv *, jclass, const jshort a, const jshort b) {
    const auto res = fromJShort(a) - fromJShort(b);
    return static_cast<jshort>(res.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1mul
(JNIEnv *, jclass, const jshort a, const jshort b) {
    const auto res = fromJShort(a) * fromJShort(b);
    return static_cast<jshort>(res.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1div
(JNIEnv *, jclass, const jshort a, const jshort b) {
    const auto res = fromJShort(a) / fromJShort(b);
    return static_cast<jshort>(res.value);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1eq
(JNIEnv *, jclass, const jshort a, const jshort b) {
    return fromJShort(a) == fromJShort(b);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1le
(JNIEnv *, jclass, const jshort a, const jshort b) {
    return fromJShort(a) <= fromJShort(b);
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1lt
(JNIEnv *, jclass, const jshort a, const jshort b) {
    return fromJShort(a) < fromJShort(b);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1neg
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>((-fromJShort(val)).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1abs
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(abs(fromJShort(val)).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1sign
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(sign(fromJShort(val)).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1round
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(rint(fromJShort(val)).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1sqrt
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(sqrt(fromJShort(val)).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1exp
(JNIEnv *, jclass, const jshort val) {
    const auto p = fromJShort(val);
    const posit16 result = exp(p.toDouble());
    return static_cast<jshort>(result.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1log
(JNIEnv *, jclass, const jshort val) {
    const auto p = fromJShort(val);
    const posit16 result = log(p.toDouble());
    return static_cast<jshort>(result.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1sin
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(intel_sin(fromJShort(val), sin_table_16, cos_table_16).value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1cos
(JNIEnv *, jclass, const jshort val) {
    return static_cast<jshort>(intel_sin(fromJShort(val) + pi_2_16, sin_table_16, cos_table_16).value);
}
