# Generate local CMake presets from installed PLCnext SDKs.
#
# Usage (plcncli must be on PATH, run from anywhere):
#   cmake -P misc/configure_plcnext_presets.cmake
#
# Creates one configure/build preset pair per installed SDK target in
# CMakeUserPresets.json. Generated presets carry a vendor key, are
# replaced on re-run, and hand-written presets are preserved.
#
# The document is assembled as JSON text because string(JSON SERIALIZE)
# was removed in CMake 4; string(JSON GET) returns nested values as
# CMake-style formatted JSON, which keeps the output readable.

set(owner "ptusa_main/plcnext-presets/1.0")
get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(output "${root}/CMakeUserPresets.json")

if(NOT WIN32 AND NOT UNIX)
    message(FATAL_ERROR "Only Windows and Linux hosts are supported")
endif()

if(WIN32)
    set(inherit "windows-AXCF-default")
else()
    set(inherit "linux-AXCF-default")
endif()

# Render a list of JSON values as an indented JSON array.
function(json_array out items)
    if(NOT items)
        set(${out} "[]" PARENT_SCOPE)
        return()
    endif()
    set(body "")
    set(sep "")
    foreach(item ${items})
        string(REPLACE "\n" "\n  " indented "${item}")
        string(APPEND body "${sep}\n  ${indented}")
        set(sep ",")
    endforeach()
    set(${out} "[${body}\n]" PARENT_SCOPE)
endfunction()

# Read the SDK list from plcncli; tolerate a leading BOM or newlines.
execute_process(COMMAND plcncli get sdks
    OUTPUT_VARIABLE sdks_json ERROR_VARIABLE sdks_err RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "plcncli get sdks failed: ${sdks_err}")
endif()
string(FIND "${sdks_json}" "{" json_start)
if(json_start LESS 0)
    message(FATAL_ERROR "plcncli get sdks returned no JSON")
endif()
string(SUBSTRING "${sdks_json}" ${json_start} -1 sdks_json)
string(JSON sdk_count ERROR_VARIABLE json_err LENGTH "${sdks_json}" "sdks")
if(json_err)
    message(FATAL_ERROR "Cannot parse plcncli output: ${json_err}")
endif()

# Keep the existing document if it is valid JSON.
if(EXISTS "${output}")
    file(READ "${output}" doc)
    string(JSON _unused ERROR_VARIABLE doc_err TYPE "${doc}")
    if(doc_err)
        message(FATAL_ERROR "Existing ${output} is not valid JSON: ${doc_err}")
    endif()
else()
    set(doc "{}")
endif()
string(JSON _unused ERROR_VARIABLE version_err TYPE "${doc}" "version")
if(version_err)
    set(version_value "4")
else()
    string(JSON version_value GET "${doc}" "version")
endif()

set(conf_generated "")
set(build_generated "")
set(labels_generated "")
set(names_generated "")
set(seen "")
math(EXPR sdk_last "${sdk_count} - 1")
foreach(i RANGE 0 ${sdk_last})
    string(JSON sdk_path GET "${sdks_json}" "sdks" ${i} "path")
    file(TO_CMAKE_PATH "${sdk_path}" sdk_path)
    get_filename_component(sdk_path "${sdk_path}" REALPATH)
    if(NOT EXISTS "${sdk_path}/toolchain.cmake")
        message(FATAL_ERROR "SDK toolchain not found: ${sdk_path}/toolchain.cmake")
    endif()
    set(make_exe "")
    if(WIN32)
        file(GLOB make_candidates "${sdk_path}/sysroots/*/usr/bin/make.exe")
        list(LENGTH make_candidates make_count)
        if(NOT make_count EQUAL 1)
            message(FATAL_ERROR
                "Expected one SDK make.exe in ${sdk_path}, found ${make_count}")
        endif()
        list(GET make_candidates 0 make_exe)
    endif()
    string(JSON target_count LENGTH "${sdks_json}" "sdks" ${i} "targets")
    math(EXPR target_last "${target_count} - 1")
    foreach(j RANGE 0 ${target_last})
        string(JSON device GET "${sdks_json}" "sdks" ${i} "targets" ${j} "name")
        string(JSON version GET "${sdks_json}" "sdks" ${i} "targets" ${j}
            "longVersion")
        string(JSON short GET "${sdks_json}" "sdks" ${i} "targets" ${j}
            "shortVersion")
        string(JSON tver GET "${sdks_json}" "sdks" ${i} "targets" ${j} "version")
        # Include the path so parallel SDK installations never share a cache.
        set(identity "${sdk_path}|${device}|${version}")
        string(FIND ";${seen};" ";${identity};" known)
        if(NOT known LESS 0)
            continue()
        endif()
        list(APPEND seen "${identity}")
        string(SHA256 identity_sha "${identity}")
        string(SUBSTRING "${identity_sha}" 0 10 name_suffix)
        string(REGEX REPLACE "[^A-Za-z0-9_-]+" "-" slug "${device}-${tver}")
        set(name "local-plcnext-${slug}-${name_suffix}")
        set(label "Local ${device} ${version}")

        set(environment "{}")
        string(JSON environment SET "${environment}"
            "PLCNEXT_SDK_ROOT" "\"${sdk_path}\"")
        string(JSON environment SET "${environment}"
            "ARP_DEVICE" "\"${device}\"")
        string(JSON environment SET "${environment}"
            "ARP_DEVICE_VERSION" "\"${version}\"")
        string(JSON environment SET "${environment}"
            "ARP_DEVICE_SHORT_VERSION" "\"${short}\"")
        set(cache_variables "{\"CMAKE_BUILD_TYPE\":\"Release\"}")
        if(make_exe)
            file(TO_CMAKE_PATH "${make_exe}" make_exe)
            string(JSON cache_variables SET "${cache_variables}"
                "CMAKE_MAKE_PROGRAM" "\"${make_exe}\"")
            file(RELATIVE_PATH sysroot_rel "${sdk_path}/sysroots" "${make_exe}")
            string(REGEX REPLACE "^([^/]+)/.*$" "\\1" sysroot_name "${sysroot_rel}")
            string(JSON environment SET "${environment}"
                "ARP_SDK_PACKAGE_NAME" "\"${sysroot_name}\"")
        endif()
        set(vendor "{\"${owner}\":{}}")

        set(preset "{}")
        string(JSON preset SET "${preset}" "name" "\"${name}\"")
        string(JSON preset SET "${preset}" "displayName" "\"${label}\"")
        string(JSON preset SET "${preset}"
            "description" "\"Installed SDK: ${sdk_path}\"")
        string(JSON preset SET "${preset}" "inherits" "\"${inherit}\"")
        string(JSON preset SET "${preset}" "binaryDir"
            "\"\${sourceDir}/bin/build/\${presetName}\"")
        string(JSON preset SET "${preset}" "environment" "${environment}")
        string(JSON preset SET "${preset}" "cacheVariables" "${cache_variables}")
        string(JSON preset SET "${preset}" "vendor" "${vendor}")
        list(APPEND conf_generated "${preset}")

        set(build_preset "{}")
        string(JSON build_preset SET "${build_preset}" "name" "\"${name}\"")
        string(JSON build_preset SET "${build_preset}"
            "displayName" "\"${label}\"")
        string(JSON build_preset SET "${build_preset}"
            "configurePreset" "\"${name}\"")
        string(JSON build_preset SET "${build_preset}" "vendor" "${vendor}")
        list(APPEND build_generated "${build_preset}")
        list(APPEND labels_generated "${label}")
        list(APPEND names_generated "${name}")
    endforeach()
endforeach()

# Replace only our own presets; refuse to overwrite hand-written names.
# A function (not a macro) is required: macro arguments are substituted
# textually, which would re-expand the ${sourceDir} preset placeholders.
function(merge_presets key generated merged_list)
    string(JSON _unused ERROR_VARIABLE type_err TYPE "${doc}" "${key}")
    set(kept_items "")
    if(NOT type_err)
        string(JSON item_count LENGTH "${doc}" "${key}")
        math(EXPR item_last "${item_count} - 1")
        foreach(k RANGE 0 ${item_last})
            string(JSON item GET "${doc}" "${key}" ${k})
            string(JSON _unused ERROR_VARIABLE vendor_err
                TYPE "${item}" "vendor" "${owner}")
            if(vendor_err)
                list(APPEND kept_items "${item}")
            endif()
        endforeach()
    endif()
    set(collisions "")
    foreach(item ${generated})
        string(JSON item_name GET "${item}" "name")
        foreach(kept_item ${kept_items})
            string(JSON kept_name GET "${kept_item}" "name")
            if(item_name STREQUAL kept_name)
                list(APPEND collisions "${item_name}")
            endif()
        endforeach()
    endforeach()
    if(collisions)
        list(REMOVE_DUPLICATES collisions)
        list(SORT collisions)
        message(FATAL_ERROR "Existing user preset names conflict: ${collisions}")
    endif()
    set(${merged_list} ${kept_items} ${generated} PARENT_SCOPE)
endfunction()
merge_presets(configurePresets "${conf_generated}" conf_merged)
merge_presets(buildPresets "${build_generated}" build_merged)

# Assemble the document: version, both preset arrays, then any other
# top-level members of the previous file, in their original order.
json_array(conf_text "${conf_merged}")
json_array(build_text "${build_merged}")
set(other_text "")
set(member_index 0)
while(TRUE)
    string(JSON member_name ERROR_VARIABLE member_err
        MEMBER "${doc}" ${member_index})
    if(member_err)
        break()
    endif()
    math(EXPR member_index "${member_index} + 1")
    if(member_name STREQUAL "version"
            OR member_name STREQUAL "configurePresets"
            OR member_name STREQUAL "buildPresets")
        continue()
    endif()
    string(JSON member_value GET "${doc}" "${member_name}")
    string(REPLACE "\n" "\n  " member_value "${member_value}")
    string(JSON member_quoted STRING_ENCODE "${member_name}")
    string(APPEND other_text ",\n  ${member_quoted} : ${member_value}")
endwhile()

set(content "{\n")
string(APPEND content "  \"version\" : ${version_value},\n")
string(APPEND content "  \"configurePresets\" : ${conf_text},\n")
string(APPEND content "  \"buildPresets\" : ${build_text}")
string(APPEND content "${other_text}\n}")
file(WRITE "${output}.tmp" "${content}\n")
file(RENAME "${output}.tmp" "${output}")

list(LENGTH seen target_total)
message("Updated ${output}: ${target_total} installed SDK target(s).")
math(EXPR print_last "${target_total} - 1")
foreach(n RANGE 0 ${print_last})
    list(GET labels_generated ${n} label)
    list(GET names_generated ${n} name)
    message("  ${label}")
    message("    cmake --preset ${name}")
    message("    cmake --build --preset ${name}")
endforeach()
