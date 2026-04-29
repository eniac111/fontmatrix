kdoctools_create_handbook(
    index.docbook
    INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en
    SUBDIR fontmatrix-ng
)

# Dev-tree mirror: copy the handbook artifacts into ${CMAKE_BINARY_DIR}/data/
# under the layout khelpcenter expects (doc/HTML/<lang>/<app>/). Combined with
# the launcher in src/CMakeLists.txt this lets F1 find the handbook when the
# binary is run from the build tree, without `make install`.
set(_handbook_dev_dir "${CMAKE_BINARY_DIR}/data/doc/HTML/en/fontmatrix-ng")
add_custom_command(
    OUTPUT  "${_handbook_dev_dir}/index.cache.bz2"
            "${_handbook_dev_dir}/index.docbook"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${_handbook_dev_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/index.cache.bz2"
            "${_handbook_dev_dir}/index.cache.bz2"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
            "${_handbook_dev_dir}/index.docbook"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/index.cache.bz2"
            "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
    COMMENT "Mirroring handbook into ${_handbook_dev_dir}"
    VERBATIM
)
add_custom_target(fontmatrix-ng-handbook-devtree ALL
    DEPENDS "${_handbook_dev_dir}/index.cache.bz2"
            "${_handbook_dev_dir}/index.docbook"
)
