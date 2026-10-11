set -e

SCRIPT_DIR="$(dirname "$(readlink -f "${0}")")"
DIRS_TO_CHECK=("${SCRIPT_DIR}/Graphs"
)

if [[ -z ${CLANG_FORMAT_EXECUTABLE} ]]; then
    CLANG_FORMAT_EXECUTABLE=clang-format-6.0
fi

print_usage () {
    echo "Usage:"
    echo "      $0"
    echo "          [-f ] - format code in place"
    echo "          [-v ] - verbose output"
    echo
}

CHECK_ONLY=true
VERBOSE_OUTPUT=false
while getopts "fhv" opt; do
  case $opt in
    f)
      CHECK_ONLY=false
      ;;
    h)
      print_usage
      exit 0
      ;;
    v)
      VERBOSE_OUTPUT=true
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
    *)
      echo "Invalid option: -$OPTARG" >&2
      print_usage
      exit 1
      ;;
  esac
done

function format() {
    find "${DIRS_TO_CHECK[@]}" \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec ${CLANG_FORMAT_EXECUTABLE} -i -style=file {} +
}

function check() {
    FOUND_ERRORS=false
    if $VERBOSE_OUTPUT; then
        FILES_TO_CHECK=$(find "${DIRS_TO_CHECK[@]}" \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))
        for f in ${FILES_TO_CHECK}
        do
            CHECK_RESULT=$(${CLANG_FORMAT_EXECUTABLE} -style=file -output-replacements-xml "$f")
            if [[ $CHECK_RESULT == *"</replacement>"*  ]]; then
                echo "Formatting errors found in $f"
                REFORMATTED_FILE=$(${CLANG_FORMAT_EXECUTABLE} -style=file "$f")
                diff -u "$f" - <<< "${REFORMATTED_FILE}"
                FOUND_ERRORS=true
            fi
        done
    else
        CF_RESULT=$(find "${DIRS_TO_CHECK[@]}" \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec ${CLANG_FORMAT_EXECUTABLE} -output-replacements-xml -style=file {} +)
        if grep -q '</replacement>' <<< "$CF_RESULT"; then
            FOUND_ERRORS=true
        fi
    fi
    if ${FOUND_ERRORS}; then
        echo "NOK"
        exit 1
    else
        echo "OK"
    fi
}

if ${CHECK_ONLY}; then
    check
else
    format
fi
