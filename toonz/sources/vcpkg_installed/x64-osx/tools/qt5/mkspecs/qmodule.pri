EXTRA_INCLUDEPATH += $$[QT_INSTALL_HEADERS]/../ $$[QT_INSTALL_HEADERS]
EXTRA_LIBDIR += $$[QT_INSTALL_LIBS] $$[QT_INSTALL_LIBS]/manual-link
!host_build|!cross_compile {
    QMAKE_LIBS_PRIVATE+=$$[QT_INSTALL_LIBS]/libbz2.a
    QMAKE_LIBS_PRIVATE+=$$[QT_INSTALL_LIBS]/libpng16.a $$[QT_INSTALL_LIBS]/libz.a
    QMAKE_LIBS_PRIVATE+=$$[QT_INSTALL_LIBS]/libzstd.a
    QMAKE_CC=cc
    QMAKE_CXX=c++
    QMAKE_AR=ar
    QMAKE_RANLIB=ranlib
    QMAKE_STRIP=strip
    QMAKE_NM=nm
    QMAKE_RC=
    QMAKE_MT=
    QMAKE_AR+=qc
    QMAKE_LINK=c++
    QMAKE_LINK_SHLIB=c++
    QMAKE_LINK_C=cc
    QMAKE_LINK_C_SHLIB=cc
    QMAKE_CFLAGS_RELEASE+=-fPIC -O3 -DNDEBUG  -isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk" -arch x86_64 -mmacosx-version-min=12
    QMAKE_CXXFLAGS_RELEASE+=-fPIC -O3 -DNDEBUG  -isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk" -arch x86_64 -mmacosx-version-min=12
    QMAKE_LFLAGS+=-isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk" -arch x86_64 -mmacosx-version-min=12
    QMAKE_LFLAGS_SHLIB+=-isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk" -arch x86_64 -mmacosx-version-min=12
    QMAKE_LFLAGS_PLUGIN+=-isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk" -arch x86_64 -mmacosx-version-min=12
}
QT_CPU_FEATURES.x86_64 = cx16 mmx sse sse2 sse3 ssse3 sse4.1
QT.global_private.enabled_features = sse2 alloca_h alloca avx2 dbus dlopen gc_binaries gui network reduce_exports sql system-zlib testlib widgets xml zstd
QT.global_private.disabled_features = alloca_malloc_h android-style-assets private_tests dbus-linked intelcet libudev posix_fallocate reduce_relocations release_tools relocatable stack-protector-strong
QMAKE_LIBS_LIBDL = 
QT_COORD_TYPE = double
QMAKE_LIBS_ZLIB = $$[QT_INSTALL_LIBS]/libz.a
QMAKE_LIBS_ZSTD = -lzstd
CONFIG += sse2 aesni sse3 ssse3 sse4_1 sse4_2 avx avx2 avx512f avx512bw avx512cd avx512dq avx512er avx512ifma avx512pf avx512vbmi avx512vl compile_examples f16c largefile precompile_header rdrnd rdseed shani x86SimdAlways
QT_BUILD_PARTS += libs tools
QT_HOST_CFLAGS_DBUS += 
