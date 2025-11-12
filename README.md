# jposits

**jposits** is a lightweight Java/Kotlin library for working with the [Posit](https://posithub.org/docs/posit_standard.pdf) number system — a modern alternative to IEEE 754 floating-point arithmetic.  
It provides full 16-bit and 32-bit Posit arithmetic implemented in native C via integrated [SoftPosit](https://gitlab.com/cerlane/SoftPosit) bindings and exposed to Java through JNI.

---

## ✨ Features

- 🔢 Support for **16-bit (HalfPosit)** and **32-bit (Posit)** number formats  
- ⚙️ Full set of arithmetic operations:  
  `add`, `sub`, `mul`, `div`, `abs`, `sign`, `sqrt`, `sin`, `cos`, `tan`, `exp`, `log`, `pow`, `ceil`, `floor`, `round`, `remainder`  
- 🧮 Conversion between Posit and standard IEEE 754 types (`float`, `double`)  
- 🔁 Built-in [SoftPosit](https://gitlab.com/cerlane/SoftPosit) native implementation — **no external installation required**  
- 🧱 Cross-platform build (Windows / Linux / macOS) through **Gradle + CMake + Ninja**
- 📦 Lightweight and dependency-free

---

## 🚀 Installation & Build

### ✅ Requirements

- **Java 21+** or **Kotlin 2.2+**
- **Gradle 8+**
- **CMake**, **Ninja**, and **GCC** or **Clang**

> 🪟 For Windows users:  
> By default, the project is preconfigured to use **JetBrains CLion toolchain**  
> (MinGW, CMake, Ninja paths are already defined in `build.gradle.kts`).

---

### 🧰 Build steps

Clone the repository and build everything — including native JNI library:

```bash
git clone https://github.com/Alexander1248/jposits.git
cd jposits
gradle build
````

Gradle will automatically:

1. Generate JNI headers (`generateJNIHeader`)
2. Configure and build the native SoftPosit bindings via CMake (`cmakeConfigure`, `cmakeBuild`)
3. Copy the resulting `libjposits` library into `build/resources/main/native/<os>`
4. Package everything into the final JAR file

You’ll find the ready-to-use library under:

```
build/libs/jposits-1.0.jar
```

---

### 🧩 Example usage

```java
import ru.alexander1248.jposit.Posit;
import ru.alexander1248.jposit.HalfPosit;

public class Example {
    public static void main(String[] args) {
        Posit a = new Posit(1.5f);
        Posit b = new Posit(2.25f);
        Posit result = a.multiply(b);

        System.out.println("Result (float): " + result.toFloat());
    }
}
```

---

## ⚙️ Native library details

The native code (C + JNI) is located in `src/main/nativelib` and is automatically compiled using **CMake**.
The resulting shared library (`libjposits.dll`, `.so`, or `.dylib`) is packaged into the JAR.

You **don’t need to install SoftPosit separately** — it’s already embedded in the source tree.

If you wish to rebuild or debug the native code manually:

```bash
gradle cmakeBuild
```

---

## 🧠 About Posits

Posits are a modern number format designed by **John L. Gustafson** as a replacement for IEEE 754 floats.
They provide higher precision and dynamic range using fewer bits and simpler arithmetic rules.

📚 Learn more:

* [The Posit Standard (PDF)](https://posithub.org/docs/posit_standard.pdf)
* [SoftPosit Reference Implementation](https://gitlab.com/cerlane/SoftPosit)

---

## 🧑‍💻 Development

Run tests:

```bash
gradle test
```

Rebuild JNI bindings:

```bash
gradle cmakeBuild
```

---

## 📄 License

This project is licensed under the **MIT License**.
See [LICENSE](LICENSE) for details.

---

## 🤝 Acknowledgments

* [SoftPosit](https://gitlab.com/cerlane/SoftPosit) — C reference implementation of Posit arithmetic
* John L. Gustafson — creator of the Posit number system
