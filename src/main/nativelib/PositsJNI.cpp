#define _USE_MATH_DEFINES
#include <bitset>
#include <cmath>

#include "ru_alexander1248_jposit_PositsJNI.h"
#include <include/softposit_cpp.h>
#include <iostream>

#include "include/internals.h"


//    ====================================================================
//                                  MATH
//    ====================================================================
static const posit32 LN_2_32 = log(2);
static const posit16 LN_2_16 = log(2);
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


typedef struct {
    bool sign;
    long scale;          // итоговая степень (с учётом режима и exp)
    posit32_t fraction;  // нормализованная мантисса без скрытой 1
} posit32_parts;

posit32_parts decomposeP32(const posit32_t pA) {
    ui32_p32 uA;
    uint_fast32_t tmp=0;
    uint_fast64_t expA=0;
    int_fast32_t kA=0;

    uA.p = pA;
    uint_fast32_t uiA = uA.ui;

    if (uA.ui == 0)
        return {false, 0, pA};
    if(uA.ui == 0x80000000)
        return {true, 0, pA};

    const bool signA = signP32UI( uiA );
    if(signA) uiA = (-uiA & 0xFFFFFFFF);
    const bool regSA = signregP32UI(uiA);
    tmp = (uiA<<2)&0xFFFFFFFF;
    if (regSA){
        while (tmp>>31){
            kA++;
            tmp= (tmp<<1) & 0xFFFFFFFF;
        }
    }
    else{
        kA=-1;
        while (!(tmp>>31)){
            kA--;
            tmp= (tmp<<1) & 0xFFFFFFFF;
        }
        tmp&=0x7FFFFFFF;
    }
    expA = tmp >> 29; // 2 bits
    tmp = (tmp<<3) & 0xFFFFFFFF; // move past exponent bits

    const uint32_t frac_ui = (tmp >> 5) | 0x40000000;
    const long scale = (kA << 2) + expA;
    return {signA, scale, frac_ui};
}
posit32_t composeP32(const posit32_parts& parts) {
    constexpr int ES = 2;
    const int32_t k = static_cast<int32_t>(parts.scale) >> ES;
    const uint32_t exp = static_cast<uint32_t>(parts.scale) & ((1u << ES) - 1u);

    uint32_t ui = 0;
    int writePos = 30;
    bool saturated = false;

    // 1) regime
    if (k >= 0) {
        for (int i = 0; i < (k + 1) && writePos >= 0; ++i) {
            ui |= (1u << writePos);
            --writePos;
        }
        if (writePos >= 0) {
            --writePos; // terminating 0
        } else {
            saturated = true;
        }
    } else {
        int zeros = -k;
        for (int i = 0; i < zeros && writePos >= 0; ++i) {
            --writePos;
        }
        if (writePos >= 0) {
            ui |= (1u << writePos);
            --writePos;
        } else {
            saturated = true;
        }
    }

    // если не осталось места — насыщение
    if (!saturated) {
        // 2) exponent
        for (int b = ES - 1; b >= 0 && writePos >= 0; --b) {
            uint32_t bit = (exp >> b) & 1u;
            if (bit) ui |= (1u << writePos);
            --writePos;
        }

        // 3) fraction bits
        constexpr int SOURCE_FRAC_BITS = 29; // без hidden bit
        const uint32_t fraction_only = ((parts.fraction.v & 0x3FFFFFFF) << 2) & ((1u << SOURCE_FRAC_BITS) - 1u);

        for (int src = SOURCE_FRAC_BITS - 1; src >= 0 && writePos >= 0; --src) {
            uint32_t bit = (fraction_only >> src) & 1u;
            if (bit) ui |= (1u << writePos);
            --writePos;
        }
    }

    // finalize
    if (parts.sign) {
        ui = (~ui) + 1u;
    }

    ui32_p32 out;
    out.ui = ui;
    return out.p;
}

typedef struct {
    bool sign;
    int scale;        // чистая экспонента (kA * 2^es + exp)
    posit16_t fraction;   // нормализованная мантисса в posit-виде
} posit16_parts;
posit16_parts decomposeP16(const posit16_t pA) {
    ui16_p16 uA;
    uint_fast16_t tmp = 0;
    int_fast32_t kA = 0;
    uint_fast16_t expA = 0;

    uA.p = pA;
    uint_fast16_t uiA = uA.ui;

    // --- особые случаи ---
    if (uiA == 0)
        return {false, 0, pA};
    if (uiA == 0x8000)
        return {true, 0, pA};

    // --- знак ---
    const bool signA = signP16UI(uiA);
    if (signA)
        uiA = (-uiA & 0xFFFF);

    // --- режим ---
    const bool regSA = signregP16UI(uiA);
    tmp = (uiA << 2) & 0xFFFF;

    if (regSA) {
        while (tmp >> 15) {
            kA++;
            tmp = (tmp << 1) & 0xFFFF;
        }
    } else {
        kA = -1;
        while (!(tmp >> 15)) {
            kA--;
            tmp = (tmp << 1) & 0xFFFF;
        }
        tmp &= 0x7FFF;
    }

    // --- экспонента ---
    expA = tmp >> 14;
    tmp = (tmp<<2) & 0xFFFF; // move past exponent bits

    const uint16_t frac_ui = (tmp >> 4) | 0x4000;
    const int scale = (kA << 1) + expA;
    return {signA, scale, frac_ui};
}
posit16_t composeP16(const posit16_parts& parts) {
    constexpr int ES = 1; // для posit16 (es = 1)
    const int32_t k = parts.scale >> ES;
    const uint32_t exp = static_cast<uint32_t>(parts.scale) & ((1u << ES) - 1u);

    uint16_t ui = 0;
    int writePos = 14;
    bool saturated = false;

    // 1️⃣ — режим (regime)
    if (k >= 0) {
        for (int i = 0; i < (k + 1) && writePos >= 0; ++i) {
            ui |= (1u << writePos);
            --writePos;
        }
        if (writePos >= 0)
            --writePos; // terminating 0
        else
            saturated = true;
    } else {
        int zeros = -k;
        for (int i = 0; i < zeros && writePos >= 0; ++i)
            --writePos; // пишем 0
        if (writePos >= 0) {
            ui |= (1u << writePos); // terminating 1
            --writePos;
        } else
            saturated = true;
    }

    // 2️⃣ — экспонента и фракция
    if (!saturated) {
        // экспонента (1 бит)
        for (int b = ES - 1; b >= 0 && writePos >= 0; --b) {
            uint32_t bit = (exp >> b) & 1u;
            if (bit) ui |= (1u << writePos);
            --writePos;
        }

        // фракция (мантисса без скрытого бита)
        constexpr int SOURCE_FRAC_BITS = 13; // 1 скрытый + 13 бит => 14 всего
        const uint16_t fraction_only = ((parts.fraction.v & 0x3FFFu) << 1) & ((1u << SOURCE_FRAC_BITS) - 1u);

        for (int src = SOURCE_FRAC_BITS - 1; src >= 0 && writePos >= 0; --src) {
            const uint16_t bit = (fraction_only >> src) & 1u;
            if (bit) ui |= (1u << writePos);
            --writePos;
        }
    }

    // 3️⃣ — знак
    if (parts.sign)
        ui = static_cast<uint16_t>((~ui + 1u) & 0xFFFF);

    ui16_p16 out;
    out.ui = ui;
    return out.p;
}

posit32 sign(const posit32 posit) {
    const auto bits = posit.value;
    if (bits == 0) return 0;
    return signP32UI(bits) ? -1 : 1;
}

posit16 sign(const posit16 posit) {
    const auto bits = posit.value;
    if (bits == 0) return 0;
    return signP16UI(bits) ? -1 : 1;
}

template <typename T>
T abs(const T posit) {
    return sign(posit) < 0 ? -posit : posit;
}

template<typename T>
T tailor_log_recursive( // NOLINT(*-no-recursion)
    const T x,
    const int order,
    const int stopOrder) {
    if (order > stopOrder) return 1;
    const auto order2 = order * 2;
    const T o = static_cast<T>(order2 - 1) / (order2 + 1);
    return 1 + x * o * tailor_log_recursive(x, order + 1, stopOrder);
}

template<typename T>
T tailor_log_approx(const T x) {
    const auto z = (x - 1) / (x + 1);
    return 2 * z * tailor_log_recursive(z * z, 1, 8);
}

posit32 fast_log(const posit32 x) {
    const auto parts = decomposeP32(posit32_t{x.value});
    if (parts.sign) return posit32().toNaR();
    auto frac = posit32();
    frac.value = parts.fraction.v;
    return posit32(parts.scale) * LN_2_32 + tailor_log_approx(frac);
}

posit16 fast_log(const posit16 x) {
    const auto parts = decomposeP16(posit16_t{x.value});
    if (parts.sign) return posit16().toNaR();
    auto frac = posit16();
    frac.value = parts.fraction.v;
    return posit16(parts.scale) * LN_2_16 + tailor_log_approx(frac);
}

template<typename T>
T tailor_exp_recursive( // NOLINT(*-no-recursion)
    const T x,
    const int order,
    const int stopOrder) {
    if (order > stopOrder) return 1;
    return 1 + x * tailor_exp_recursive(x, order + 1, stopOrder) / order;
}

template<typename T>
T tailor_exp_approx(const T x) {
    return tailor_exp_recursive(x, 1, 8);
}

posit32 fast_exp(const posit32 x) {
    const auto sign = signP32UI(x.value);
    const auto bx = (sign ? -x : x) / LN_2_32;
    const auto cbx = rint(bx);
    const auto frac = LN_2_32 * (bx - cbx);
    const auto base = tailor_exp_approx(frac);
    auto parts = decomposeP32(posit32_t{base.value});
    printf("DEBUG base=%f recomposed=%f parts.scale=%d\n", base.toDouble(), convertP32ToDouble(composeP32(parts)), parts.scale);
    parts.scale += static_cast<long>(cbx.toDouble());

    posit32 result;
    result.value = composeP32(parts).v;
    return sign ? 1 / result : result;
}

posit16 fast_exp(const posit16 x) {
    const auto sign = signP16UI(x.value);
    const auto bx = (sign ? -x : x) / LN_2_16;
    const auto cbx = rint(bx);
    const auto frac = LN_2_16 * (bx - cbx);
    const auto base = tailor_exp_approx(frac);
    auto parts = decomposeP16(posit16_t{base.value});
    parts.scale += static_cast<int32_t>(cbx.toDouble());

    posit16 result;
    result.value = composeP16(parts).v;
    return sign ? 1 / result : result;
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
//                                 DEBUG
//    ====================================================================
bool debug = false;

JNIEXPORT void JNICALL Java_ru_alexander1248_jposit_PositsJNI_set_1debug
  (JNIEnv *, jclass, jboolean state) {
    debug = state;
}

JNIEXPORT jboolean JNICALL Java_ru_alexander1248_jposit_PositsJNI_get_1debug
  (JNIEnv *, jclass) {
    return debug;
}

//    ====================================================================
//                                  INIT
//    ====================================================================
JNIEXPORT void JNICALL Java_ru_alexander1248_jposit_PositsJNI_init
(JNIEnv *, jclass) {
    if (debug) std::cout << "Initializing SoftPosits..." << std::endl;
    for (auto i = 0; i < table_size; i++) {
        const auto cos_val = cos(2 * M_PI * i / table_size);
        cos_table_32[i] = cos_val;
        cos_table_16[i] = cos_val;

        const auto sin_val = sin(2 * M_PI * i / table_size);
        sin_table_32[i] = sin_val;
        sin_table_16[i] = sin_val;
    }
    if (debug) std::cout << "Created " << table_size << " entries for sin and cos tables! "<< std::endl;
    if (debug) std::cout << "SoftPosits initialized!" << std::endl;
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
    const auto result = fast_exp(p);
    return static_cast<jint>(result.value);
}

JNIEXPORT jint JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit32_1log
(JNIEnv *, jclass, const jint val) {
    const auto p = fromJInt(val);
    const posit32 result = fast_log(p);
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
    const auto result = fast_exp(p);
    return static_cast<jshort>(result.value);
}

JNIEXPORT jshort JNICALL Java_ru_alexander1248_jposit_PositsJNI_posit16_1log
(JNIEnv *, jclass, const jshort val) {
    const auto p = fromJShort(val);
    const posit16 result = fast_log(p);
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
