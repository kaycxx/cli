# Copyright (c) 2026 Klaus Reimer
# SPDX-License-Identifier: MIT
# Version: 1.3.2
# Source: https://github.com/kaycxx/cmake-git-deps

if(COMMAND git_require)
    return()
endif()

include(FetchContent)

function(_git_parse_required output is_version value)
    if("${value}" MATCHES "^v?([0-9]+(\\.[0-9]+)*)$")
        set("${output}" "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set("${is_version}" TRUE PARENT_SCOPE)
    else()
        set("${output}" "${value}" PARENT_SCOPE)
        set("${is_version}" FALSE PARENT_SCOPE)
    endif()
endfunction()

function(git_require target)
    set(options)
    set(one_value_args PACKAGE REVISION URL VERSION USE_SYSTEM)
    set(multi_value_args)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        list(LENGTH ARG_UNPARSED_ARGUMENTS unparsed_count)
        if(unparsed_count EQUAL 1 AND NOT DEFINED ARG_REVISION)
            list(GET ARG_UNPARSED_ARGUMENTS 0 ARG_REVISION)
        else()
            message(FATAL_ERROR "Invalid git_require() arguments: ${ARG_UNPARSED_ARGUMENTS}")
        endif()
    endif()

    if(NOT DEFINED ARG_REVISION)
        message(FATAL_ERROR "git_require(${target}) needs a REVISION or positional revision")
    endif()

    if(DEFINED ARG_VERSION)
        _git_parse_required(required required_is_version "${ARG_VERSION}")
    else()
        _git_parse_required(required required_is_version "${ARG_REVISION}")
    endif()

    if("${target}" MATCHES "^([^:]+)::([^:]+)$")
        set(owner "${CMAKE_MATCH_1}")
        set(repo "${CMAKE_MATCH_2}")
    elseif(NOT DEFINED ARG_PACKAGE OR NOT DEFINED ARG_URL)
        message(FATAL_ERROR
            "Target '${target}' does not match '<owner>::<repo>'. "
            "Pass PACKAGE and URL explicitly."
        )
    endif()

    if(DEFINED ARG_PACKAGE)
        set(package "${ARG_PACKAGE}")
    else()
        set(package "${owner}-${repo}")
    endif()

    if(DEFINED ARG_URL)
        set(git_url "${ARG_URL}")
    else()
        set(git_url "https://github.com/${owner}/${repo}.git")
    endif()

    string(TOUPPER "${package}" package_key)
    string(REGEX REPLACE "[^A-Z0-9]+" "_" package_key "${package_key}")
    string(REGEX REPLACE "^_+|_+$" "" package_key "${package_key}")

    if(required_is_version)
        string(REGEX MATCH "^[0-9]+" required_major "${required}")
    endif()
    string(TOUPPER "${target}" key)
    string(REGEX REPLACE "[^A-Z0-9]+" "_" key "${key}")
    string(REGEX REPLACE "^_+|_+$" "" key "${key}")

    set(use_system_var "GIT_${key}_USE_SYSTEM")
    option("${use_system_var}" "Use installed ${package} if available" ON)

    if(DEFINED ARG_USE_SYSTEM)
        set("${use_system_var}" "${ARG_USE_SYSTEM}")
    endif()

    get_property(selected GLOBAL PROPERTY "GIT_${package_key}_VERSION")
    if(selected)
        if(required_is_version AND selected MATCHES "^[0-9]+(\\.[0-9]+)*$")
            string(REGEX MATCH "^[0-9]+" selected_major "${selected}")
            if(NOT selected_major STREQUAL required_major)
                message(FATAL_ERROR
                    "${package} ${selected} is already selected, "
                    "but ${required} requires major version ${required_major}"
                )
            endif()

            if(selected VERSION_LESS required)
                message(FATAL_ERROR
                    "${package} ${selected} is already selected, "
                    "but ${required} or newer is required"
                )
            endif()
        elseif(NOT selected STREQUAL required)
            message(FATAL_ERROR
                "${package} ${selected} is already selected, "
                "but ${required} was requested"
            )
        endif()

        if(TARGET "${target}")
            return()
        endif()
    endif()

    if(TARGET "${target}")
        set_property(GLOBAL PROPERTY "GIT_${package_key}_VERSION" "${required}")
        return()
    endif()

    if("${${use_system_var}}" AND required_is_version)
        find_package("${package}" "${required}" CONFIG QUIET)
        if(TARGET "${target}")
            set(actual "${required}")
            if(NOT "${${package}_VERSION}" STREQUAL "")
                set(actual "${${package}_VERSION}")
            endif()

            if(required_is_version AND actual MATCHES "^[0-9]+(\\.[0-9]+)*$")
                string(REGEX MATCH "^[0-9]+" actual_major "${actual}")
                if(NOT actual_major STREQUAL required_major)
                    message(FATAL_ERROR
                        "Installed ${package} ${actual} was found, "
                        "but ${required} requires major version ${required_major}"
                    )
                endif()
            elseif(NOT actual STREQUAL required)
                message(FATAL_ERROR
                    "Installed ${package} ${actual} was found, "
                    "but ${required} was requested"
                )
            endif()

            set_property(GLOBAL PROPERTY "GIT_${package_key}_VERSION" "${actual}")
            return()
        endif()

        if("${${package}_FOUND}")
            message(FATAL_ERROR "${package} was found, but it did not define ${target}")
        endif()

        if(NOT "${${package}_CONSIDERED_CONFIGS}" STREQUAL "")
            message(FATAL_ERROR
                "Installed ${package} was found, but no compatible version for "
                "${required}. Considered versions: ${${package}_CONSIDERED_VERSIONS}"
            )
        endif()
    endif()

    set_property(GLOBAL PROPERTY "GIT_${package_key}_VERSION" "${required}")

    FetchContent_Declare(
        "${package}"
        GIT_REPOSITORY "${git_url}"
        GIT_TAG "${ARG_REVISION}"
        EXCLUDE_FROM_ALL
    )

    get_property(build_shared_libs_cache_exists CACHE BUILD_SHARED_LIBS PROPERTY VALUE SET)
    if(build_shared_libs_cache_exists)
        get_property(build_shared_libs_cache_value CACHE BUILD_SHARED_LIBS PROPERTY VALUE)
    endif()

    if(DEFINED BUILD_SHARED_LIBS)
        set(build_shared_libs_value "${BUILD_SHARED_LIBS}")
        set(build_shared_libs_exists TRUE)
    else()
        set(build_shared_libs_exists FALSE)
    endif()

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    FetchContent_MakeAvailable("${package}")

    if(build_shared_libs_cache_exists)
        set(BUILD_SHARED_LIBS "${build_shared_libs_cache_value}" CACHE BOOL "Build shared libraries" FORCE)
    else()
        unset(BUILD_SHARED_LIBS CACHE)
    endif()

    if(build_shared_libs_exists)
        set(BUILD_SHARED_LIBS "${build_shared_libs_value}")
    else()
        unset(BUILD_SHARED_LIBS)
    endif()

    if(NOT TARGET "${target}")
        message(FATAL_ERROR "${package} did not define ${target}")
    endif()
endfunction()
