package ru.alexander1248.jposit;

import java.io.IOException;

class PositsJNI {
    static {
        try { NativeLoader.load("libjposits"); } catch (IOException e) { throw new RuntimeException(e); }
        init();
    }

    public static native void init();
//    ====================================================================
//                                  POSIT_32
//    ====================================================================
//    Conversion functions
    public static native int posit32_nar();

    public static native int posit32_fromFloat(float f);
    public static native int posit32_fromDouble(double d);
    public static native int posit32_fromInt(int i);
    public static native int posit32_fromLong(long l);

    public static native float posit32_toFloat(int p);
    public static native double posit32_toDouble(int p);
    public static native int posit32_toInt(int p);
    public static native long posit32_toLong(int p);
//    Math functions
    public static native int posit32_add(int a, int b);
    public static native int posit32_sub(int a, int b);
    public static native int posit32_mul(int a, int b);
    public static native int posit32_div(int a, int b);


    public static native boolean posit32_eq(int a, int b);
    public static native boolean posit32_le(int a, int b);
    public static native boolean posit32_lt(int a, int b);

    public static native int posit32_neg(int a);
    public static native int posit32_abs(int a);
    public static native int posit32_sign(int a);

    public static native int posit32_round(int a);

    public static native int posit32_sqrt(int a);
    public static native int posit32_exp(int a);
    public static native int posit32_log(int a);
    public static native int posit32_sin(int a);
    public static native int posit32_cos(int a);

//    ====================================================================
//                                  POSIT_16
//    ====================================================================
//    Conversion functions
    public static native short posit16_nar();

    public static native short posit16_fromFloat(float f);
    public static native short posit16_fromDouble(double d);
    public static native short posit16_fromInt(int i);
    public static native short posit16_fromLong(long l);

    public static native float posit16_toFloat(short p);
    public static native double posit16_toDouble(short p);
    public static native int posit16_toInt(short p);
    public static native long posit16_toLong(short p);
    //    Math functions
    public static native short posit16_add(short a, short b);
    public static native short posit16_sub(short a, short b);
    public static native short posit16_mul(short a, short b);
    public static native short posit16_div(short a, short b);


    public static native boolean posit16_eq(short a, short b);
    public static native boolean posit16_le(short a, short b);
    public static native boolean posit16_lt(short a, short b);

    public static native short posit16_neg(short a);
    public static native short posit16_abs(short a);
    public static native short posit16_sign(short a);

    public static native short posit16_round(short a);

    public static native short posit16_sqrt(short a);
    public static native short posit16_exp(short a);
    public static native short posit16_log(short a);
    public static native short posit16_sin(short a);
    public static native short posit16_cos(short a);
}
