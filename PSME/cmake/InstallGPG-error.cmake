# <license_header>
#
# Copyright (c) 2017-2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

function(install_gpg_error)
    include(ConfigurationPackage OPTIONAL)
    assure_package(gpg-error 1.27 "https://gnupg.org/ftp/gcrypt/libgpg-error/libgpg-error-1.27.tar.bz2" "5217ef3e76a7275a2a3b569a12ddc989")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --with-cxx)

    set(ENV{CFLAGS} "-m32")
    set(ENV{LDFLAGS} "-L${CMAKE_BINARY_DIR}/lib")
    set(ENV{LD_LIBRARY_PATH} "${CMAKE_BINARY_DIR}/lib")

    set(ac_cv_va_copy_tmp $ENV{ac_cv_va_copy})
    set(ENV{ac_cv_va_copy} C99)
    execute_process(
        COMMAND ${source_dir}/configure ${CONFIGURE_FLAGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project ${source_dir}")
    endif()
    set(ENV{ac_cv_va_copy} ${ac_cv_va_copy_tmp})

    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/gpg-error.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/gpg-error.pc
        @ONLY
    )

    file(INSTALL ${binary_dir}/src/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libgpg-error*.so*"
    )
    file(INSTALL ${binary_dir}/src/gpg-error.h
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
    file(COPY ${binary_dir}/src/gpg-error-config
        DESTINATION ${CMAKE_BINARY_DIR}/bin
    )
endfunction()

install_gpg_error()
