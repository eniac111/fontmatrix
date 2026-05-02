IF(WANT_HARFBUZZ)
	# Two unity-build aggregators (`harfbuzz.c` and `harfbuzz-shaper-all.cpp`)
	# `#include` every other source in this directory. Listing them alongside
	# the individual files makes MSVC emit LNK4006 for every symbol; both
	# aggregators are omitted here.
	SET(harfbuzz_SOURCES
	harfbuzz-arabic.c
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

