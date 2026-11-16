#!/bin/bash
# This script requires GNU bash. It uses bash-specific features (arrays, etc.) and will not work with /bin/sh.

set -u

# The configuration array below is derived from CMake presets instead of
# being hardcoded. Configure presets are taken from "cmake
# --list-presets=configure", build preset names and display labels from
# "cmake --list-presets=build", and the configure-to-build preset mapping
# is scraped from the "buildPresets" of CMakePresets.json and, when it
# exists, CMakeUserPresets.json. CMake lists user presets too, so presets
# from CMakeUserPresets.json are discovered automatically. A configure
# preset without a listed build preset is skipped with a note. On a
# Windows host Linux presets are discovered and run through WSL, which
# replaces the old hardcoded "wsl" prefix. Keep running the script from
# the repository root.

SECONDS=0

# Turn "cmake --list-presets" output into "<name><TAB><display name>" rows.
parse_preset_listing() {
    tr -d '\r' | awk '
        {
            if (!match($0, /^[ \t]*"[^"]*"/)) {
                next
            }
            name = substr($0, RSTART, RLENGTH)
            sub(/^[ \t]*"/, "", name)
            sub(/"$/, "", name)
            display = substr($0, RSTART + RLENGTH)
            sub(/^[ \t]*-[ \t]+/, "", display)
            gsub(/^[ \t]+|[ \t]+$/, "", display)
            print name "\t" display
        }
    '
}

# Print presets of a kind as "<name><TAB><display name>" rows. The second
# argument "wsl" runs the query inside WSL from the current directory.
list_presets() {
    if [ "$2" = wsl ]; then
        wsl -e cmake --list-presets="$1" 2> /dev/null | parse_preset_listing
    else
        cmake --list-presets="$1" 2> /dev/null | parse_preset_listing
    fi
}

# Print "<configurePreset><TAB><buildPreset>" pairs taken from the JSON
# preset files. This is a simple line-oriented scrape of the formatting
# that CMake and IDEs generate, not a full JSON parser.
scrape_build_preset_map() {
    local file
    for file in CMakePresets.json CMakeUserPresets.json; do
        [ -f "$file" ] || continue
        awk -F'"' '
            $2 ~ /Presets$/ {
                in_build = ($2 == "buildPresets")
                pending = ""
                next
            }
            in_build && $2 == "name" {
                pending = $4
                next
            }
            in_build && $2 == "configurePreset" && pending != "" {
                print $4 "\t" pending
            }
        ' "$file"
    done
}

have_wsl=0
if [ "$(uname -s)" != Linux ] && command -v wsl > /dev/null 2>&1; then
    have_wsl=1
fi

# List the available build presets as "name<TAB>display name" rows and the
# configure-to-build mapping, both merged over the host and WSL views.
build_list=$({
    list_presets build ""
    if [ "$have_wsl" = 1 ]; then
        list_presets build wsl
    fi
} | awk -F'\t' '!seen[$1]++')
build_names_nl=$(printf '%s\n' "$build_list" | cut -f1)
map_list=$(scrape_build_preset_map)

is_listed_build() { # $1 = build preset name to look up.
    case $'\n'"$build_names_nl"$'\n' in
        *$'\n'"$1"$'\n'*) return 0 ;;
    esac
    return 1
}

# Echo the build preset of a configure preset: the same name when it is
# listed, otherwise the first mapped and listed candidate. Fail if none.
find_build_preset() { # $1 = configure preset name.
    local cfg="$1" candidate
    if is_listed_build "$cfg"; then
        printf '%s\n' "$cfg"
        return 0
    fi
    while IFS= read -r candidate; do
        if [ -n "$candidate" ] && is_listed_build "$candidate"; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done <<< "$(printf '%s\n' "$map_list" |
        awk -F'\t' -v c="$cfg" '$1 == c { print $2 }')"
    return 1
}

# Echo the display name of a build preset, falling back to its own name.
build_label() { # $1 = build preset name.
    local label
    label=$(printf '%s\n' "$build_list" |
        awk -F'\t' -v n="$1" '$1 == n { print $2; exit }')
    printf '%s\n' "${label:-$1}"
}

configs=() # Rows "configure preset build preset label" for the loop below.
prefixes=() # Empty, or "wsl" for presets discovered through WSL.

# Add a configure preset with its build preset to the run, or skip it with
# a note when no build preset is available for it.
add_preset() { # $1 = configure preset, $2 = command prefix ("" or "wsl").
    local cfg="$1" prefix="$2" build label
    if ! build=$(find_build_preset "$cfg"); then
        echo "note: $cfg has no listed build preset and was skipped." >&2
        return 0
    fi
    label=$(build_label "$build")
    configs+=("$cfg $build $label")
    prefixes+=("$prefix")
    echo "${#configs[@]}> [$label] $cfg -> $build${prefix:+ ($prefix)}"
}

echo -e '\033[1;36mDiscovered configure presets:\033[0m'

native_cfg_list=$(list_presets configure "")
while IFS=$'\t' read -r cfg _display; do
    [ -n "$cfg" ] || continue
    add_preset "$cfg" ""
done <<< "$native_cfg_list"

if [ "$have_wsl" = 1 ]; then
    native_cfg_names=$(printf '%s\n' "$native_cfg_list" | cut -f1)
    wsl_cfg_list=$(list_presets configure wsl)
    if [ -z "$wsl_cfg_list" ]; then
        echo "note: cmake through wsl listed no configure presets, so the" \
            "Linux presets were not added." >&2
    fi
    while IFS=$'\t' read -r cfg _display; do
        [ -n "$cfg" ] || continue
        case $'\n'"$native_cfg_names"$'\n' in
            *$'\n'"$cfg"$'\n'*) continue ;;
        esac
        add_preset "$cfg" wsl
    done <<< "$wsl_cfg_list"
fi

if [ "${#configs[@]}" -eq 0 ]; then
    echo "error: no buildable presets found, run the script in the" \
        "repository root." >&2
    exit 1
fi

tmp_dir="/tmp/bi_all_times_$$"
mkdir -p "$tmp_dir"

pids=()
for i in "${!configs[@]}"; do
    config="${configs[$i]}"
    read -r CFG_NAME BUILD_NAME SHORT_NAME <<< "$config"
    PREFIX="${prefixes[$i]}"
    (
        start_time=$(date +%s)
        {
            echo "Build started: $CFG_NAME"
            ${PREFIX:+$PREFIX } cmake -D CFG_NAME:string="$CFG_NAME" -D BUILD_NAME:string="$BUILD_NAME" -P ./misc/bi.cmake
            echo "Build finished: $CFG_NAME"
        } | while IFS= read -r line; do echo "$((i+1))> [$SHORT_NAME]  $line"; done
        end_time=$(date +%s)
        duration=$((end_time - start_time))
        echo "$duration" > "$tmp_dir/build_time_$i.txt"
    ) 2>&1 | ./misc/colorize.sh &
    pids+=($!)
done

for pid in "${pids[@]}"; do
    wait $pid
done

echo -e '\033[1;36m'
echo "Build times by configuration:"
for i in "${!configs[@]}"; do
    config="${configs[$i]}"
    read -r CFG_NAME BUILD_NAME SHORT_NAME <<< "$config"
    if [[ -f "$tmp_dir/build_time_$i.txt" ]]; then
        duration=$(cat "$tmp_dir/build_time_$i.txt")
        echo "$((i+1))> [$SHORT_NAME] $CFG_NAME: $duration seconds"
        rm -f "$tmp_dir/build_time_$i.txt"
    fi
done
rmdir "$tmp_dir"

duration=$SECONDS
printf "\033[1;36mTotal time: %d seconds (%d:%02d).\033[0m\n" "$duration" "$((duration/60))" "$((duration%60))"
