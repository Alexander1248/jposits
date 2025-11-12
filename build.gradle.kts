plugins {
    kotlin("jvm") version "2.2.0"
    java
}

java {
    sourceCompatibility = JavaVersion.VERSION_21
    targetCompatibility = JavaVersion.VERSION_21
}

group = "ru.alexander"
version = "1.0"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test"))
    testImplementation("org.assertj:assertj-core:3.27.6")
}

tasks.test {
    useJUnitPlatform()
}

// ------------------ JNI header generation ------------------
tasks.register<Exec>("generateJNIHeader") {
    val tempFile = layout.buildDirectory.file("PositsJNI.java").get().asFile
    doFirst {
        // создаём временный файл без NativeLoader.load()
        val origFile = file("src/main/java/ru/alexander1248/jposit/PositsJNI.java")
        tempFile.writeText(origFile.readText().replace("try", "// try"))
    }

    workingDir = projectDir
    commandLine = listOf(
        "javac",
        "-h", "src/main/nativelib",
        tempFile.absolutePath,
    )
}

// ------------------ Paths for CMake/MinGW/Ninja ------------------
val mingwPath = "C:/Program Files/JetBrains/CLion 2023.3.4/bin/mingw/bin"
val cmakePath = "C:/Program Files/JetBrains/CLion 2023.3.4/bin/cmake/win/x64/bin"
val ninjaPath = "C:/Program Files/JetBrains/CLion 2023.3.4/bin/ninja/win/x64"
val nativeBuildDir = layout.buildDirectory.dir("native")
val nativeSourceDirAbsolute: String = projectDir.resolve("src/main/nativelib").absolutePath

// ------------------ OS detection ------------------
fun detectOsId(): String {
    val os = System.getProperty("os.name").lowercase()
    return when {
        os.contains("win") -> "win"
        os.contains("mac") -> "macos"
        os.contains("nux") || os.contains("nix") || os.contains("aix") -> "linux"
        else -> throw GradleException("Unsupported OS: $os")
    }
}

fun libFileNameForOs(osId: String): String = when (osId) {
    "win" -> "libjposits.dll"
    "linux" -> "libjposits.so"
    "macos" -> "libjposits.dylib"
    else -> throw GradleException("Unsupported OS id: $osId")
}

// ------------------ CMake configure ------------------
tasks.register<Exec>("cmakeConfigure") {
    dependsOn("generateJNIHeader")

    doFirst {
        nativeBuildDir.get().asFile.mkdirs()
    }

    commandLine(
        "$cmakePath/cmake.exe",
        "-S", nativeSourceDirAbsolute,
        "-B", nativeBuildDir.get().asFile.absolutePath,
        "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_MAKE_PROGRAM=$ninjaPath/ninja.exe",
        "-DCMAKE_C_COMPILER=$mingwPath/gcc.exe",
        "-DCMAKE_CXX_COMPILER=$mingwPath/g++.exe"
    )
}

// ------------------ CMake build ------------------
tasks.register<Exec>("cmakeBuild") {
    dependsOn("cmakeConfigure")

    commandLine(
        "$cmakePath/cmake.exe",
        "--build", nativeBuildDir.get().asFile.absolutePath
    )
}

// ------------------ Copy native libs ------------------
tasks.register<Copy>("copyNativeLib") {
    dependsOn("cmakeBuild")

    val osId = detectOsId()
    val libName = libFileNameForOs(osId)

    from(fileTree(nativeBuildDir.get().asFile) {
        include("**/$libName")
    })

    into(layout.buildDirectory.dir("resources/main/native/$osId"))
    duplicatesStrategy = DuplicatesStrategy.INCLUDE
    outputs.upToDateWhen { false } // всегда выполнять
}

// ------------------ Build dependency ------------------
tasks.named("processResources") {
    dependsOn("copyNativeLib") // чтобы ресурсы подхватывали нативную DLL
}

tasks.named("build") {
    dependsOn("processResources")
}
