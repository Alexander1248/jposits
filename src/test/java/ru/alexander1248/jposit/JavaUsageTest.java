package ru.alexander1248.jposit;

import org.junit.jupiter.api.Test;

import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

public class JavaUsageTest {
    @Test
    public void testJavaUsage() throws IOException {
        Posit a = new Posit(1.5f);
        Posit b = new Posit(2.25f);
        Posit result = a.times(b);
        assertThat(result.toFloat()).isEqualTo(3.375f);
    }
}
