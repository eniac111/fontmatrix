# Samples
IF(UNIX AND NOT APPLE)
    SET(SAMPLE_DIR "${KDE_INSTALL_DATADIR}/fontmatrix-ng/resources/Samples")
    # Mirror samples into the build tree so the binary can be run in-place
    # without a make install step (binary at build/bin/ → build/share/...).
    file(COPY ${CMAKE_SOURCE_DIR}/samples/
         DESTINATION "${CMAKE_BINARY_DIR}/share/fontmatrix-ng/resources/Samples"
         FILES_MATCHING PATTERN "sample")
ELSE()
    SET(SAMPLE_DIR "${RESOURCESDIR}/Samples")
ENDIF()

# Bengali
ADD_SUBDIRECTORY(bn)

# Deutsch
ADD_SUBDIRECTORY(de)

# Français
ADD_SUBDIRECTORY(fr)

# Gujarati
ADD_SUBDIRECTORY(gu)

# Hebrew
ADD_SUBDIRECTORY(he)

# Hindi
ADD_SUBDIRECTORY(hi)

# Kannada
ADD_SUBDIRECTORY(kn)

# Malayalam
ADD_SUBDIRECTORY(ml)

# Marathi
ADD_SUBDIRECTORY(mr)

# Oriya
ADD_SUBDIRECTORY(or)

# Punjabi
ADD_SUBDIRECTORY(pa)

# Russian
ADD_SUBDIRECTORY(ru)

# Tamil
ADD_SUBDIRECTORY(ta)

# Telugu
ADD_SUBDIRECTORY(te)

# Udmurt
ADD_SUBDIRECTORY(udm)
