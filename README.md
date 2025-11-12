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
## ⚗️ Implementation Status

| Category                      | Operation                              | Status                   | Notes                                                     |
| ----------------------------- | -------------------------------------- | ------------------------ | --------------------------------------------------------- |
| **Arithmetic**                | `add`, `sub`                           | ✅ Implemented natively  | Fully handled via SoftPosit                               |
|                               | `mul`, `div`                           | ✅ Implemented natively  | Full precision Posit arithmetic                           |
|                               | `remainder`                            | ✅ Implemented natively  | Uses SoftPosit internal remainder logic                   |
| **Unary Ops**                 | `abs`, `sign`, `negation`              | ✅ Implemented natively  | —                                                         |
| **Rounding**                  | `ceil`, `floor`, `round`               | ✅ Implemented natively  | Integer rounding consistent with Posit spec               |
| **Trigonometry**              | `sin`, `cos`, `tan`                    | ✅ Implemented           | Uses software implementation                              |
| **Exponential / Logarithmic** | `exp`, `log`                           | ⚠️ Partially implemented | Performed via IEEE 754 (`double`) conversion              |
| **Root / Power**              | `sqrt`                                 | ✅ Implemented natively  | —                                                         |
|                               | `pow`                                  | ⚠️ Partially implemented | Uses  `exp` and `log` with partial implementation         |
| **Data Structures**           | `Posit` (32-bit), `HalfPosit` (16-bit) | ✅ Implemented           | Includes full arithmetic, comparison, and conversion APIs |
| **Conversion**                | To/from `float`, `double`              | ✅ Implemented           | IEEE 754 interop functions                                |
| **Future Plans**              | `atan`, `asin`, `acos`, `hypot`, `fma` | ⏳ Planned               | Will be implemented in pure Posit arithmetic              |

---

## 🚀 Installation & Build

### ✅ Requirements

* **Java 21+** or **Kotlin 2.2+**
* **Gradle 8+**
* **CMake** (recommended 3.20+)
* **Ninja build system**
* **GCC / MinGW / Clang**

---

### ⚙️ Configuring Paths in `build.gradle.kts`

To correctly build the native library, you need to specify the paths to compilers and tools like CMake and Ninja. Currently, these paths are defined as:

```kotlin
val gccPath = "D:/JetBrains/CLion/bin/mingw/bin"
val gppPath = "D:/JetBrains/CLion/bin/mingw/bin"
val cmakePath = "D:/JetBrains/CLion/bin/cmake/win/x64/bin"
val ninjaPath = "D:/JetBrains/CLion/bin/ninja/win/x64"
```

---

### 💡 How to Edit Paths for Your OS and Setup

#### Windows

* If you use **JetBrains CLion + MinGW**, the above paths should work after replacing `D:/JetBrains/CLion` to your CLion path.
* If you use a different MinGW, CMake, or Ninja installation, update paths accordingly, for example:

```kotlin
val gccPath = "C:/MinGW/bin"
val gppPath = "C:/MinGW/bin"
val cmakePath = "C:/Program Files/CMake/bin"
val ninjaPath = "C:/Program Files/Ninja"
```

* Make sure the paths contain `gcc.exe`, `g++.exe`, `cmake.exe`, and `ninja.exe`.

---

#### Linux

* Usually, gcc, g++, cmake, and ninja are installed globally and available in your `PATH`. You can simply set:

```kotlin
val gccPath = "/usr/bin"
val gppPath = "/usr/bin"
val cmakePath = "/usr/bin"
val ninjaPath = "/usr/bin"
```

* Adjust these if your tools are in non-standard locations.

---

#### macOS

* Similar to Linux, often installed via Homebrew:

```kotlin
val gccPath = "/usr/local/bin"
val gppPath = "/usr/local/bin"
val cmakePath = "/usr/local/bin"
val ninjaPath = "/usr/local/bin"
```

---

### 📦 Building the Project

1. **Clone the repository:**

```bash
git clone https://github.com/Alexander1248/jposits.git
cd jposits
```

2. **Edit `build.gradle.kts` to set paths to your compiler and build tools**
   (if you are on Windows, Linux, or macOS and your tools are not in standard locations).

3. **Run Gradle build:**

```bash
gradle build
```

This will automatically:

* Generate JNI headers
* Configure and build native SoftPosit bindings via CMake and Ninja
* Copy native libraries to `build/resources/main/native/<os>`
* Package everything into the final JAR

---

### 🧩 Example usage

- Java:
```java
import ru.alexander1248.jposit.Posit;
import ru.alexander1248.jposit.HalfPosit;

public class Example {
    public static void main(String[] args) {
        Posit a = new Posit(1.5f);
        Posit b = new Posit(2.25f);
        Posit result = a.times(b);

        System.out.println("Result (float): " + result.toFloat());
    }
}
```

- Kotlin:
```kotlin
import ru.alexander1248.jposit.Posit.Companion.toPosit

fun example() {
    val a = 1.5f.toPosit()
    val b = 2.25f.toPosit()
    val result = a * b
    println("Result (float): " + result.toFloat())
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
To regenerate JNI headers:

```bash
gradle generateJNIHeader
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
