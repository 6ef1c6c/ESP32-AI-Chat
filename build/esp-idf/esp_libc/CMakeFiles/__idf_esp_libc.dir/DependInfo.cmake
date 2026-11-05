
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/builds/esp_idf/components/esp_libc/src/port/riscv/strcmp.S" "/home/builds/esp32_c6_ai/build/esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/strcmp.S.obj"
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
  "/home/builds/esp_idf/components/esp_libc/platform_include"
  "/home/builds/esp_idf/components/esp_libc/priv_include"
  "/home/builds/esp_idf/components/freertos/config/include"
  "/home/builds/esp_idf/components/freertos/config/include/freertos"
  "/home/builds/esp_idf/components/freertos/config/riscv/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/include"
  "/home/builds/esp_idf/components/freertos/FreeRTOS-Kernel/portable/riscv/include/freertos"
  "/home/builds/esp_idf/components/freertos/esp_additions/include"
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
  "/home/builds/esp_idf/components/spi_flash/include"
  "/home/builds/esp_idf/components/esp_hal_mspi/include"
  "/home/builds/esp_idf/components/esp_hal_mspi/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_gpspi/include"
  "/home/builds/esp_idf/components/esp_hal_gpspi/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_clock/include"
  "/home/builds/esp_idf/components/esp_hal_clock/esp32c6/include"
  "/home/builds/esp_idf/components/esp_blockdev/include"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/builds/esp_idf/components/esp_libc/src/abort.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/abort.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/abort.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/assert.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/assert.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/assert.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/getentropy.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/getentropy.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/getentropy.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/heap.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/heap.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/heap.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/init.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/init.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/init.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/locks.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/locks.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/locks.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/bufio_setvbuf.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/bufio_setvbuf.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/bufio_setvbuf.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/errno.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/errno.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/errno.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/fopencookie.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/fopencookie.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/fopencookie.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/getreent.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/getreent.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/getreent.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/open_memstream.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/open_memstream.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/open_memstream.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/picolibc_init.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/picolibc_init.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/picolibc_init.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/picolibc/rand.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/rand.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/picolibc/rand.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/poll.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/poll.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/poll.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/port/esp_time_impl.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/esp_time_impl.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/esp_time_impl.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/port/riscv/memcpy.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/memcpy.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/memcpy.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/port/riscv/memmove.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/memmove.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/memmove.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/port/riscv/strcpy.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/strcpy.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/port/riscv/strcpy.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/pthread.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/pthread.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/pthread.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/random.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/random.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/random.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/realpath.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/realpath.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/realpath.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/reent_syscalls.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/reent_syscalls.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/reent_syscalls.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/scandir.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/scandir.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/scandir.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/stdatomic.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/stdatomic.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/stdatomic.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/string/memcmp.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/memcmp.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/memcmp.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/string/strncmp.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/strncmp.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/strncmp.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/string/strncpy.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/strncpy.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/string/strncpy.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/syscalls.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/syscalls.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/syscalls.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/sysconf.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/sysconf.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/sysconf.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/termios.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/termios.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/termios.c.obj.d"
  "/home/builds/esp_idf/components/esp_libc/src/time.c" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/time.c.obj" "gcc" "esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/src/time.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
