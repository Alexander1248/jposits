package ru.alexander1248.jposit

class HalfPosit : Comparable<HalfPosit> {
    private val value : Short

    private constructor(value: Short) {
        this.value = value
    }

    fun toFloat(): Float =
        PositsJNI.posit16_toFloat(this.value)
    fun toDouble(): Double =
        PositsJNI.posit16_toDouble(this.value)
    fun toInt(): Int =
        PositsJNI.posit16_toInt(this.value)
    fun toLong(): Long =
        PositsJNI.posit16_toLong(this.value)

    companion object {
        val ZERO = HalfPosit(PositsJNI.posit16_fromInt(0))
        val ONE = HalfPosit(PositsJNI.posit16_fromInt(1))
        val NEGATIVE_ONE = HalfPosit(PositsJNI.posit16_fromInt(-1))
        val NAR = HalfPosit(PositsJNI.posit16_nar())

        private val ALMOST_HALF = HalfPosit(PositsJNI.posit16_fromDouble(0.4999999))
        fun Float.toHalfPosit(): HalfPosit =
            HalfPosit(PositsJNI.posit16_fromFloat(this))
        fun Double.toHalfPosit(): HalfPosit =
            HalfPosit(PositsJNI.posit16_fromDouble(this))
        fun Int.toHalfPosit(): HalfPosit =
            HalfPosit(PositsJNI.posit16_fromInt(this))
        fun Long.toHalfPosit(): HalfPosit =
            HalfPosit(PositsJNI.posit16_fromLong(this))

        fun abs(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_abs(posit.value))
        fun sign(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_sign(posit.value))

        fun sqrt(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_sqrt(posit.value))
        fun sin(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_sin(posit.value))
        fun cos(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_cos(posit.value))
        fun tan(posit: HalfPosit): HalfPosit =
            sin(posit) / cos(posit)

        fun exp(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_exp(posit.value))
        fun log(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_log(posit.value))
        fun pow(value: HalfPosit, power: HalfPosit): HalfPosit =
            exp(power * log(value))
        fun ceil(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_round((posit + ALMOST_HALF).value))
        fun floor(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_round((posit - ALMOST_HALF).value))
        fun round(posit: HalfPosit): HalfPosit =
            HalfPosit(PositsJNI.posit16_round(posit.value))
    }
    operator fun plus(other: HalfPosit): HalfPosit =
         HalfPosit(PositsJNI.posit16_add(this.value, other.value))
    operator fun minus(other: HalfPosit): HalfPosit =
        HalfPosit(PositsJNI.posit16_sub(this.value, other.value))
    operator fun times(other: HalfPosit): HalfPosit =
        HalfPosit(PositsJNI.posit16_mul(this.value, other.value))
    operator fun div(other: HalfPosit): HalfPosit =
        HalfPosit(PositsJNI.posit16_div(this.value, other.value))
    operator fun unaryMinus(): HalfPosit =
        HalfPosit(PositsJNI.posit16_neg(this.value))
    operator fun rem(other: HalfPosit): HalfPosit =
        this - floor(this / other) * other


    override fun toString(): String = this.toFloat().toString()
    override fun compareTo(other: HalfPosit): Int =
        if (PositsJNI.posit16_eq(this.value, other.value)) 0
        else if (PositsJNI.posit16_lt(this.value, other.value)) -1 else 1
}