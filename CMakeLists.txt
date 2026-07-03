zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PMW3610 src/pmw3610.c)
# Behavior is compiled unconditionally so the &tb_cpi label (defined in the
# shared dtsi) resolves on both split halves; on the half without the trackball
# (CONFIG_PMW3610 unset) it builds as a no-op.
zephyr_library_sources(src/behavior_pmw3610_cpi.c)
zephyr_library_sources(src/behavior_pmw3610_scroll_cpi.c)
zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
