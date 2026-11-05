
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/portasm.S" "/home/builds/esp32_c6_ai/build/esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/portable/riscv/portasm.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "ESP_PLATFORM"
  "IDF_VER=\"v6.0.3\""
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  "_POSIX_READER_WRITER_LOCKS"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/home/builds/esp_idf/components/freertos/config/include"
  "/home/builds/esp_idf/components/freertos/config/include/freertos"
  "/home/builds/esp_idf/components/freertos/config/riscv/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/include/freertos"
  "/home/builds/esp_idf/components/freertos/esp_additions/include"
  "/home/builds/esp_idf/components/freertos"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/include/freertos"
  "/home/builds/esp_idf/components/freertos/esp_additions"
  "/home/builds/esp_idf/components/esp_libc/platform_include"
  "/home/builds/esp_idf/components/esp_hw_support/include"
  "/home/builds/esp_idf/components/esp_hw_support/include/soc"
  "/home/builds/esp_idf/components/esp_hw_support/ldo/include"
  "/home/builds/esp_idf/components/esp_hw_support/debug_probe/include"
  "/home/builds/esp_idf/components/esp_hw_support/etm/include"
  "/home/builds/esp_idf/components/esp_hw_support/mspi_timing_tuning/include"
  "/home/builds/esp_idf/components/esp_hw_support/mspi_timing_tuning/tuning_scheme_impl/include"
  "/home/builds/esp_idf/components/esp_hw_support/power_supply/include"
  "/home/builds/esp_idf/components/esp_hw_support/modem/include"
  "/home/builds/esp_idf/components/esp_hw_support/port/esp32c6/."
  "/home/builds/esp_idf/components/esp_hw_support/port/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hw_support/port/esp32c6/private_include"
  "/home/builds/esp_idf/components/heap/include"
  "/home/builds/esp_idf/components/heap/tlsf"
  "/home/builds/esp_idf/components/log/include"
  "/home/builds/esp_idf/components/soc/include"
  "/home/builds/esp_idf/components/soc/esp32c6"
  "/home/builds/esp_idf/components/soc/esp32c6/include"
  "/home/builds/esp_idf/components/soc/esp32c6/register"
  "/home/builds/esp_idf/components/hal/platform_port/include"
  "/home/builds/esp_idf/components/hal/esp32c6/include"
  "/home/builds/esp_idf/components/hal/include"
  "/home/builds/esp_idf/components/esp_rom/include"
  "/home/builds/esp_idf/components/esp_rom/esp32c6/include"
  "/home/builds/esp_idf/components/esp_rom/esp32c6/include/esp32c6"
  "/home/builds/esp_idf/components/esp_rom/esp32c6"
  "/home/builds/esp_idf/components/esp_common/include"
  "/home/builds/esp_idf/components/esp_system/include"
  "/home/builds/esp_idf/components/esp_system/port/soc"
  "/home/builds/esp_idf/components/esp_system/port/include/riscv"
  "/home/builds/esp_idf/components/esp_system/port/include/private"
  "/home/builds/esp_idf/components/esp_stdio/include"
  "/home/builds/esp_idf/components/riscv/include"
  "/home/builds/esp_idf/components/esp_hal_gpio/include"
  "/home/builds/esp_idf/components/esp_hal_gpio/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_usb/include"
  "/home/builds/esp_idf/components/esp_hal_usb/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_pmu/include"
  "/home/builds/esp_idf/components/esp_hal_pmu/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_ana_conv/include"
  "/home/builds/esp_idf/components/esp_hal_ana_conv/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_dma/include"
  "/home/builds/esp_idf/components/esp_hal_dma/esp32c6/include"
  "/home/builds/esp_idf/components/esp_timer/include"
  "/home/builds/esp_idf/components/lwip/include"
  "/home/builds/esp_idf/components/lwip/include/apps"
  "/home/builds/esp_idf/components/lwip/lwip/src/include"
  "/home/builds/esp_idf/components/lwip/port/include"
  "/home/builds/esp_idf/components/lwip/port/freertos/include"
  "/home/builds/esp_idf/components/lwip/port/esp32xx/include"
  "/home/builds/esp_idf/components/lwip/port/esp32xx/include/arch"
  "/home/builds/esp_idf/components/lwip/port/esp32xx/include/sys"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/event_groups.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/event_groups.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/event_groups.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/list.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/list.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/list.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/port.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/portable/riscv/port.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/portable/riscv/port.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/queue.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/queue.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/queue.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/stream_buffer.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/stream_buffer.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/stream_buffer.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/tasks.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/tasks.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/tasks.c.obj.d"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/timers.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/timers.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/timers.c.obj.d"
  "/home/builds/esp_idf/components/freertos/app_startup.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/app_startup.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/app_startup.c.obj.d"
  "/home/builds/esp_idf/components/freertos/esp_additions/idf_additions.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/esp_additions/idf_additions.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/esp_additions/idf_additions.c.obj.d"
  "/home/builds/esp_idf/components/freertos/esp_additions/idf_additions_event_groups.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/esp_additions/idf_additions_event_groups.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/esp_additions/idf_additions_event_groups.c.obj.d"
  "/home/builds/esp_idf/components/freertos/heap_idf.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/heap_idf.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/heap_idf.c.obj.d"
  "/home/builds/esp_idf/components/freertos/port_common.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port_common.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port_common.c.obj.d"
  "/home/builds/esp_idf/components/freertos/port_systick.c" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port_systick.c.obj" "gcc" "esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port_systick.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
