
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/builds/esp32_c6_ai/build/x509_crt_bundle.S" "/home/builds/esp32_c6_ai/build/esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/__/__/x509_crt_bundle.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "ESP_PLATFORM"
  "ESP_PSA_ITS_AVAILABLE"
  "IDF_VER=\"v6.0.3\""
  "MBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\""
  "MBEDTLS_MAJOR_VERSION=4"
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  "TF_PSA_CRYPTO_USER_CONFIG_FILE=\"mbedtls/esp_config.h\""
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  "_POSIX_READER_WRITER_LOCKS"
  "__STDC_WANT_LIB_EXT1__=0"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/home/builds/esp_idf/components/mbedtls/port/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/library"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/core"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/src"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/extras"
  "/home/builds/esp_idf/components/mbedtls/esp_crt_bundle/include"
  "/home/builds/esp_idf/components/mbedtls/port/psa_driver/include"
  "/home/builds/esp_idf/components/mbedtls/port/psa_crypto_storage/include"
  "/home/builds/esp_idf/components/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls"
  "/home/builds/esp_idf/components/esp_libc/platform_include"
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
  "/home/builds/esp_idf/components/esp_security/include"
  "/home/builds/esp_idf/components/esp_hal_security/esp32c6/include"
  "/home/builds/esp_idf/components/esp_hal_security/include"
  "/home/builds/esp_idf/components/esp_pm/include"
  "/home/builds/esp_idf/components/esp_driver_dma/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/include"
  "esp-idf/mbedtls/mbedtls/tf-psa-crypto/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/everest/include"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/p256-m/p256-m"
  "/home/builds/esp_idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/pqcp/include"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/builds/esp_idf/components/mbedtls/esp_crt_bundle/esp_crt_bundle.c" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/esp_crt_bundle/esp_crt_bundle.c.obj" "gcc" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/esp_crt_bundle/esp_crt_bundle.c.obj.d"
  "/home/builds/esp_idf/components/mbedtls/port/esp_mem.c" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/port/esp_mem.c.obj" "gcc" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/port/esp_mem.c.obj.d"
  "/home/builds/esp_idf/components/mbedtls/port/psa_crypto_storage/esp_psa_key_file.c" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/port/psa_crypto_storage/esp_psa_key_file.c.obj" "gcc" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/port/psa_crypto_storage/esp_psa_key_file.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
