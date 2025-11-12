package ru.alexander1248.jposit

import org.assertj.core.api.SoftAssertions
import org.assertj.core.data.Offset
import ru.alexander1248.jposit.Posit.Companion.toPosit
import kotlin.math.*
import kotlin.random.Random
import kotlin.test.Test
import kotlin.test.assertTrue

class PositTest {
    private val rounds = 1000
    private val delta = 5e-5
    @Test
    fun testInit() {
        val x = PositsJNI.posit32_fromFloat(10f)
        println(x) // должно вывести значение posit32 в виде jint
        assertTrue(x != 0) // минимальная проверка
    }
    @Test
    fun convertTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextFloat() * 2000 - 1000
                softly.assertThat(a.toPosit().toFloat())
                    .describedAs("Float -> posit -> float conversion in iteration $i failed")
                    .isEqualTo(a, Offset.offset(delta.toFloat()))
            }
            for (i in 1..rounds) {
                val a = Random.nextDouble(-1000.0, 1000.0)
                softly.assertThat(a.toPosit().toDouble())
                    .describedAs("Double -> posit -> double conversion in iteration $i failed")
                    .isEqualTo(a, Offset.offset(delta))
            }
            for (i in 1..rounds) {
                val a = Random.nextInt(-1000, 1000)
                softly.assertThat(a.toPosit().toInt())
                    .describedAs("Int -> posit -> int conversion in iteration $i failed")
                    .isEqualTo(a)
            }
            for (i in 1..rounds) {
                val a = Random.nextLong(-1000, 1000)
                softly.assertThat(a.toPosit().toLong())
                    .describedAs("Long -> posit -> long conversion in iteration $i failed")
                    .isEqualTo(a)
            }
        }
    }
    @Test
    fun addTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-1000.0, 1000.0)
                val b = Random.nextDouble(-1000.0, 1000.0)
                val rr = a + b
                val tr = a.toPosit() + b.toPosit()
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val b = Random.nextDouble(-1000.0, 1000.0)
                val rr = a - b
                val tr = a.toPosit() - b.toPosit()
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val b = Random.nextDouble(-1000.0, 1000.0)
                val rr = a * b
                val tr = a.toPosit() * b.toPosit()
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val b = Random.nextDouble(-1000.0, 1000.0)
                val rr = a / b
                val tr = a.toPosit() / b.toPosit()
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
                val a = Random.nextDouble(1000.0)
                val b = Random.nextDouble(1000.0)
                val rr = a % b
                val tr = a.toPosit() % b.toPosit()
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong division in iteration $i of $a and $b")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun negTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = -a
                val tr = -a.toPosit()
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = abs(a)
                val tr = Posit.abs(a.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = sign(a)
                val tr = Posit.sign(a.toPosit())
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
                val a = Random.nextDouble(1000.0)
                val rr = sqrt(a)
                val tr = Posit.sqrt(a.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = sin(a)
                val tr = Posit.sin(a.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = cos(a)
                val tr = Posit.cos(a.toPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong cos in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun tanTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(-100.0, 100.0)
                val rr = tan(a)
                val tr = Posit.tan(a.toPosit())
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
                val tr = Posit.exp(a.toPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong tan in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1) * 100))
            }
        }
    }
    @Test
    fun logTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(10000.0)
                val rr = ln(a)
                val tr = Posit.log(a.toPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong tan in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
    @Test
    fun powTest() {
        SoftAssertions.assertSoftly { softly ->
            for (i in 1..rounds) {
                val a = Random.nextDouble(100.0)
                val b = Random.nextDouble(10.0)
                val rr = a.pow(b)
                val tr = Posit.pow(a.toPosit(), b.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = ceil(a)
                val tr = Posit.ceil(a.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = floor(a)
                val tr = Posit.floor(a.toPosit())
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
                val a = Random.nextDouble(-1000.0, 1000.0)
                val rr = round(a)
                val tr = Posit.round(a.toPosit())
                softly.assertThat(tr.toDouble())
                    .describedAs("Wrong round in iteration $i of $a")
                    .isEqualTo(rr, Offset.offset(delta * (abs(rr) + 1)))
            }
        }
    }
}
