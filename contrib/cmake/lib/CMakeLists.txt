# ################################################################
# zstd - Makefile
# Copyright (C) Yann Collet 2014-2015
# All rights reserved.
# 
# BSD license
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# You can contact the author at :
#  - zstd source repository : https://github.com/Cyan4973/zstd
#  - Public forum : https://groups.google.com/forum/#!forum/lz4c
# ################################################################

# Get library version based on information from input content (use regular exp)
function(GetLibraryVersion _content _outputVar1 _outputVar2 _outputVar3)
    string(REGEX MATCHALL ".*define ZSTD_VERSION_MAJOR+.* ([0-9]+).*define ZSTD_VERSION_MINOR+.* ([0-9]+).*define ZSTD_VERSION_RELEASE+.* ([0-9]+)" VERSION_REGEX "${_content}")
    SET(${_outputVar1} ${CMAKE_MATCH_1} PARENT_SCOPE)
    SET(${_outputVar2} ${CMAKE_MATCH_2} PARENT_SCOPE)
    SET(${_outputVar3} ${CMAKE_MATCH_3} PARENT_SCOPE)
endfunction()

PROJECT(libzstd)

SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Define project root directory
SET(ROOT_DIR ../../..)

# Define library directory, where sources and header files are located
SET(LIBRARY_DIR ${ROOT_DIR}/lib)
INCLUDE_DIRECTORIES(${LIBRARY_DIR})

# Read file content
FILE(READ ${LIBRARY_DIR}/zstd.h HEADER_CONTENT)

# Parse version
GetLibraryVersion("${HEADER_CONTENT}" LIBVER_MAJOR LIBVER_MINOR LIBVER_RELEASE)
MESSAGE("ZSTD VERSION ${LIBVER_MAJOR}.${LIBVER_MINOR}.${LIBVER_RELEASE}")

ADD_DEFINITIONS(-DZSTD_LIB_MINIFY=1)
ADD_DEFINITIONS(-DZSTD_NO_INLINE=1)


SET(Sources
${LIBRARY_DIR}/common/debug.c
${LIBRARY_DIR}/common/entropy_common.c
${LIBRARY_DIR}/common/error_private.c
${LIBRARY_DIR}/common/fse_decompress.c
${LIBRARY_DIR}/common/pool.c
${LIBRARY_DIR}/common/threading.c
${LIBRARY_DIR}/common/xxhash.c
${LIBRARY_DIR}/common/zstd_common.c
${LIBRARY_DIR}/compress/fse_compress.c
${LIBRARY_DIR}/compress/hist.c
${LIBRARY_DIR}/compress/huf_compress.c
${LIBRARY_DIR}/compress/zstd_compress.c
${LIBRARY_DIR}/compress/zstd_compress_literals.c
${LIBRARY_DIR}/compress/zstd_compress_sequences.c
${LIBRARY_DIR}/compress/zstd_compress_superblock.c
${LIBRARY_DIR}/compress/zstd_double_fast.c
${LIBRARY_DIR}/compress/zstd_fast.c
${LIBRARY_DIR}/compress/zstd_lazy.c
${LIBRARY_DIR}/compress/zstd_ldm.c
${LIBRARY_DIR}/compress/zstd_opt.c
${LIBRARY_DIR}/compress/zstdmt_compress.c
${LIBRARY_DIR}/decompress/huf_decompress.c
${LIBRARY_DIR}/decompress/zstd_ddict.c
${LIBRARY_DIR}/decompress/zstd_decompress.c
${LIBRARY_DIR}/decompress/zstd_decompress_block.c
)

SET(Headers
${LIBRARY_DIR}/common/bitstream.h
${LIBRARY_DIR}/common/compiler.h
${LIBRARY_DIR}/common/cpu.h
${LIBRARY_DIR}/common/debug.h
${LIBRARY_DIR}/common/error_private.h
${LIBRARY_DIR}/common/fse.h
${LIBRARY_DIR}/common/huf.h
${LIBRARY_DIR}/common/mem.h
${LIBRARY_DIR}/common/pool.h
${LIBRARY_DIR}/common/threading.h
${LIBRARY_DIR}/common/xxhash.h
${LIBRARY_DIR}/common/zstd_deps.h
${LIBRARY_DIR}/common/zstd_internal.h
${LIBRARY_DIR}/common/zstd_trace.h
${LIBRARY_DIR}/compress/clevels.h
${LIBRARY_DIR}/compress/hist.h
${LIBRARY_DIR}/compress/zstd_compress_internal.h
${LIBRARY_DIR}/compress/zstd_compress_literals.h
${LIBRARY_DIR}/compress/zstd_compress_sequences.h
${LIBRARY_DIR}/compress/zstd_compress_superblock.h
${LIBRARY_DIR}/compress/zstd_cwksp.h
${LIBRARY_DIR}/compress/zstd_double_fast.h
${LIBRARY_DIR}/compress/zstd_fast.h
${LIBRARY_DIR}/compress/zstd_lazy.h
${LIBRARY_DIR}/compress/zstd_ldm.h
${LIBRARY_DIR}/compress/zstd_ldm_geartab.h
${LIBRARY_DIR}/compress/zstd_opt.h
${LIBRARY_DIR}/compress/zstdmt_compress.h
${LIBRARY_DIR}/decompress/zstd_ddict.h
${LIBRARY_DIR}/decompress/zstd_decompress_block.h
${LIBRARY_DIR}/decompress/zstd_decompress_internal.h
${LIBRARY_DIR}/zdict.h
${LIBRARY_DIR}/zstd_errors.h
${LIBRARY_DIR}/zstd.h)

IF (MSVC)
    SET(MSVC_RESOURCE_DIR ${ROOT_DIR}/visual/2013/zstdlib)
    SET(PlatformDependResources ${MSVC_RESOURCE_DIR}/resource.h ${MSVC_RESOURCE_DIR}/zstdlib.rc)
ENDIF (MSVC)

# Split project to static and shared libraries build
ADD_LIBRARY(libzstd_static STATIC ${Sources} ${Headers} ${PlatformDependResources})

# Add specific compile definitions for MSVC project
IF (MSVC)
    SET_TARGET_PROPERTIES(libzstd_static PROPERTIES COMPILE_DEFINITIONS "ZSTD_HEAPMODE=0;_CRT_SECURE_NO_WARNINGS")
ENDIF (MSVC)

# Define include directories
IF (NOT WORKAROUND_OUTDATED_CODE_STYLE)
    TARGET_INCLUDE_DIRECTORIES(libzstd_static PUBLIC ${LIBRARY_DIR})
ENDIF (NOT WORKAROUND_OUTDATED_CODE_STYLE)

# Define library base name
IF (UNIX)
    SET(LIBRARY_BASE_NAME libzstd)
ELSEIF (MSVC)
    SET(LIBRARY_BASE_NAME zstdlib)
ELSE ()
    MESSAGE(FATAL_ERROR "Unsupported build type")
ENDIF (UNIX)

# Define static and shared library names
SET(STATIC_LIBRARY_OUTPUT_NAME ${LIBRARY_BASE_NAME})

IF (MSVC)
    IF (CMAKE_SIZEOF_VOID_P MATCHES "8")
        SET(STATIC_LIBRARY_OUTPUT_NAME ${STATIC_LIBRARY_OUTPUT_NAME}_x64)
    ELSE ()
        SET(STATIC_LIBRARY_OUTPUT_NAME ${STATIC_LIBRARY_OUTPUT_NAME}_x86)
    ENDIF (CMAKE_SIZEOF_VOID_P MATCHES "8")
ENDIF (MSVC)

SET_TARGET_PROPERTIES(
        libzstd_static
        PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${STATIC_LIBRARY_OUTPUT_NAME})

IF (UNIX)
    SET(PREFIX /usr/local)
    SET(INSTALL_LIBRARY_DIR ${PREFIX}/lib)
    SET(INSTALL_INCLUDE_DIR ${PREFIX}/include)

    # install target
    INSTALL(FILES ${LIBRARY_DIR}/zstd.h DESTINATION ${INSTALL_INCLUDE_DIR})
    INSTALL(TARGETS libzstd_static DESTINATION ${INSTALL_LIBRARY_DIR})
ENDIF (UNIX)
