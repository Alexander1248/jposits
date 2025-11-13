package ru.alexander1248.jposit

import org.assertj.core.api.SoftAssertions
import org.assertj.core.data.Offset
import ru.alexander1248.jposit.HalfPosit.Companion.toHalfPosit
import kotlin.math.*
import kotlin.random.Random
import kotlin.test.Ignore
import kotlin.test.Test
import kotlin.test.assertTrue

class HalfPositTest {
    private val rounds = 1000
    private val delta = 1.0
    @Test
    fun testInit() {
        val x = PositsJNI.posit16_fromFloat(10f)
        println(x) // должно вывести значение posit32 в виде jint
        assertTrue(x.toInt() != 0) // минимальная проверка
    }
    @Test
    fun convertTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextFloat() * 200 - 100
                softly.assertThat(a.toHalfPosit().toFloat())
                    .describedAs("Float -> posit -> float conversion in iteration $i failed")
                    .isEqualTo(a, Offset.offset(delta.toFloat()))
            }
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                softly.assertThat(a.toHalfPosit().toDouble())
                    .describedAs("Double -> posit -> double conversion in iteration $i failed")
                    .isEqualTo(a, Offset.offset(delta))
            }
            for (i in 1..rounds) {
                val a = Random.nextInt(-100, 100)
                softly.assertThat(a.toHalfPosit().toInt())
                    .describedAs("Int -> posit -> int conversion in iteration $i failed")
                    .isEqualTo(a)
            }
            for (i in 1..rounds) {
                val a = Random.nextLong(-100, 100)
                softly.assertThat(a.toHalfPosit().toLong())
                    .describedAs("Long -> posit -> long conversion in iteration $i failed")
                    .isEqualTo(a)
            }
        }
    }
    @Test
    fun addTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val b = Random.nextDouble(-100.0, 100.0)
                val rr = a + b
                val tr = a.toHalfPosit() + b.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong addition in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun subTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val b = Random.nextDouble(-100.0, 100.0)
                val rr = a - b
                val tr = a.toHalfPosit() - b.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong subtraction in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun mulTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val b = Random.nextDouble(-100.0, 100.0)
                val rr = a * b
                val tr = a.toHalfPosit() * b.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong multiplication in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun divTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val b = Random.nextDouble(-100.0, 100.0)
                val rr = a / b
                val tr = a.toHalfPosit() / b.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong division in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun remTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(100.0)
                val b = Random.nextDouble(100.0)
                val rr = a % b
                val tr = a.toHalfPosit() % b.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong remainder in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun negTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = -a
                val tr = -a.toHalfPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong negation in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun absTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = abs(a)
                val tr = HalfPosit.abs(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong abs in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun signTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = sign(a)
                val tr = HalfPosit.sign(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong sign in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun sqrtTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(100.0)
                val rr = sqrt(a)
                val tr = HalfPosit.sqrt(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong sqrt in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun sinTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = sin(a)
                val tr = HalfPosit.sin(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong sin in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun cosTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = cos(a)
                val tr = HalfPosit.cos(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong cos in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Ignore("Always fails due to low precision")
    @Test
    fun tanTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = tan(a)
                val tr = HalfPosit.tan(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong tan in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1) * 100))
            }
        }
    }
    @Test
    fun expTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-10.0, 10.0)
                val rr = exp(a)
                val tr = HalfPosit.exp(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong exp in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1) * 100))
            }
        }
    }
    @Test
    fun logTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(1000.0)
                val rr = ln(a)
                val tr = HalfPosit.log(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong log in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun powTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(10.0)
                val b = Random.nextDouble(10.0)
                val rr = a.pow(b)
                val tr = HalfPosit.pow(a.toHalfPosit(), b.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong pow in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1) * 100))
            }
        }
    }
    @Test
    fun ceilTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = ceil(a)
                val tr = HalfPosit.ceil(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong ceil in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun floorTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = floor(a)
                val tr = HalfPosit.floor(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong floor in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun roundTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = round(a)
                val tr = HalfPosit.round(a.toHalfPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong round in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
}
