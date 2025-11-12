package ru.alexander1248.jposit

class Posit : Comparable<Posit> {
    private val value : Int

    private constructor(value: Int) {
        this.value = value
    }
    constructor(value: Float) {
        this.value = value.toPosit().value
    }
    constructor(value: Double) {
        this.value = value.toPosit().value
    }

    fun toFloat(): Float =
        PositsJNI.posit32_toFloat(this.value)
    fun toDouble(): Double =
        PositsJNI.posit32_toDouble(this.value)
    fun toInt(): Int =
        PositsJNI.posit32_toInt(this.value)
    fun toLong(): Long =
        PositsJNI.posit32_toLong(this.value)

    companion object {
        val ZERO = Posit(PositsJNI.posit32_fromInt(0))
        val ONE = Posit(PositsJNI.posit32_fromInt(1))
        val NEGATIVE_ONE = Posit(PositsJNI.posit32_fromInt(-1))
        val NAR = Posit(PositsJNI.posit32_nar())

        private val ALMOST_HALF = Posit(PositsJNI.posit32_fromDouble(0.4999999))
        fun Float.toPosit(): Posit =
            Posit(PositsJNI.posit32_fromFloat(this))
        fun Double.toPosit(): Posit =
            Posit(PositsJNI.posit32_fromDouble(this))
        fun Int.toPosit(): Posit =
            Posit(PositsJNI.posit32_fromInt(this))
        fun Long.toPosit(): Posit =
            Posit(PositsJNI.posit32_fromLong(this))

        fun abs(posit: Posit): Posit =
            Posit(PositsJNI.posit32_abs(posit.value))
        fun sign(posit: Posit): Posit =
            Posit(PositsJNI.posit32_sign(posit.value))

        fun sqrt(posit: Posit): Posit =
            Posit(PositsJNI.posit32_sqrt(posit.value))
        fun sin(posit: Posit): Posit =
            Posit(PositsJNI.posit32_sin(posit.value))
        fun cos(posit: Posit): Posit =
            Posit(PositsJNI.posit32_cos(posit.value))
        fun tan(posit: Posit): Posit =
            sin(posit) / cos(posit)

        fun exp(posit: Posit): Posit =
            Posit(PositsJNI.posit32_exp(posit.value))
        fun log(posit: Posit): Posit =
            Posit(PositsJNI.posit32_log(posit.value))
        fun pow(value: Posit, power: Posit): Posit =
            exp(power * log(value))
        fun ceil(posit: Posit): Posit =
            Posit(PositsJNI.posit32_round((posit + ALMOST_HALF).value))
        fun floor(posit: Posit): Posit =
            Posit(PositsJNI.posit32_round((posit - ALMOST_HALF).value))
        fun round(posit: Posit): Posit =
            Posit(PositsJNI.posit32_round(posit.value))
    }
    operator fun plus(other: Posit): Posit =
         Posit(PositsJNI.posit32_add(this.value, other.value))
    operator fun minus(other: Posit): Posit =
        Posit(PositsJNI.posit32_sub(this.value, other.value))
    operator fun times(other: Posit): Posit =
        Posit(PositsJNI.posit32_mul(this.value, other.value))
    operator fun div(other: Posit): Posit =
        Posit(PositsJNI.posit32_div(this.value, other.value))
    operator fun unaryMinus(): Posit =
        Posit(PositsJNI.posit32_neg(this.value))
    operator fun rem(other: Posit): Posit =
        this - floor(this / other) * other


    override fun toString(): String = this.toFloat().toString()
    override fun compareTo(other: Posit): Int =
        if (PositsJNI.posit32_eq(this.value, other.value)) 0
        else if (PositsJNI.posit32_lt(this.value, other.value)) -1 else 1
}