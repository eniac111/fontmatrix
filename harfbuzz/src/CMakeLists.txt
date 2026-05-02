IF(WANT_HARFBUZZ)
	# harfbuzz-shaper-all.cpp is a "unity build" aggregator: it does
	# nothing but `#include` every other shaper source (shaper.cpp,
	# indic.cpp, tibetan.c, khmer.c, hebrew.c, arabic.c, hangul.c,
	# myanmar.c, thai.c). If we list it AND those same files, every
	# symbol they define ends up duplicated across two .obj files in
	# harfbuzz.lib. GNU ld silently picks one archive member and
	# skips the other; MSVC's link.exe pulls both in and errors with
	# LNK2005. Listing the individual files (as we do below) is the
	# correct approach — shaper-all.cpp is redundant and is omitted.
	SET(harfbuzz_SOURCES
	harfbuzz-arabic.c
	harfbuzz.c
	harfbuzz-gsub.c
	harfbuzz-hebrew.c
	harfbuzz-myanmar.c
	harfbuzz-stream.c
	harfbuzz-buffer.c
	harfbuzz-dump.c
	harfbuzz-gdef.c
	harfbuzz-gpos.c
	harfbuzz-hangul.c
	harfbuzz-khmer.c
	harfbuzz-open.c
	harfbuzz-tibetan.c
	harfbuzz-indic.cpp
	harfbuzz-shaper.cpp
	harfbuzz-thai.c
	harfbuzz-impl.c
	)
ELSE(WANT_HARFBUZZ)
	SET(harfbuzz_SOURCES
	harfbuzz.c
	harfbuzz-gsub.c
	harfbuzz-stream.c
	harfbuzz-buffer.c
	harfbuzz-gdef.c
	harfbuzz-gpos.c
	harfbuzz-open.c
	harfbuzz-impl.c
	)
ENDIF(WANT_HARFBUZZ)


ADD_LIBRARY(harfbuzz
STATIC ${harfbuzz_SOURCES})

target_compile_options(harfbuzz PRIVATE -w)

# harfbuzz-indic.cpp does `#include <QDebug>` (its INDIC_DEBUG branch is
# unconditionally enabled) and calls qDebug() at runtime. On Linux this
# happens to compile because Qt's include dir is on the system include
# path, but under MSVC/Craft Qt6 headers are only reachable via the
# Qt6::Core target's include directories. Linking PRIVATEly here pulls
# in both the include path and the qDebug symbol on every platform.
target_link_libraries(harfbuzz PRIVATE Qt6::Core)

# MESSAGE(STATUS "Adding ${harfbuzz}" )

