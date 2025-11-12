package ru.alexander1248.jposit;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

class NativeLoader {
    static void load(String libName) throws IOException {
        String os = System.getProperty("os.name").toLowerCase();
        String mappedName;

        if (os.contains("win")) {
            mappedName = "/native/win/" + libName + ".dll";
        } else if (os.contains("mac")) {
            mappedName = "/native/macos/" + libName + ".dylib";
        } else if (os.contains("nix") || os.contains("nux") || os.contains("aix")) {
            mappedName = "/native/linux/" + libName + ".so";
        } else {
            throw new UnsupportedOperationException("Unsupported OS: " + os);
        }

        try (InputStream in = NativeLoader.class.getResourceAsStream(mappedName)) {
            if (in == null) {
                throw new IOException("Native library not found in resources: " + mappedName);
            }

            Path temp = Files.createTempFile(libName, mappedName.substring(mappedName.lastIndexOf('.')));
            temp.toFile().deleteOnExit();
            Files.copy(in, temp, StandardCopyOption.REPLACE_EXISTING);

            System.load(temp.toAbsolutePath().toString());
        }
    }
}
