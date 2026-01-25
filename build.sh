#!/usr/bin/env bash
set -e
target="$(make -s print-OUTPUT_DIR)/$(make -s print-TARGET)"
test_target="$(make -s print-OUTPUT_DIR)/$(make -s print-TEST_TARGET)"
output_dir="$(make -s print-OUTPUT_DIR)"
suffix="$(make -s print-BIN_SUFFIX)"
# 简单帮助
usage() {
    cat << EOF
Usage: $(basename "$0") <command> [args...]
    Commands:
    run                 Build (if needed) and run the emulator with given ROM.
                        eg: 
                            ./build.sh run [args...]
                                [args...] : arguments to pass to the emulator
    test                Build and run test.
                        eg: 
                            ./build.sh test <test_name> [args...]
                                <test_name> : name of the %.c (without suffix) in the test dir 
                                [args...] : arguments to pass to the test
    all                 Clean and build the project.
    help                Show this help.
EOF
}

check_and_run()
{
    exe="$1"; shift
    args="$*"
    if [ -x "$exe" ]; then
        $exe $args
    else
        echo "Error: executable $exe not found."
        exit 1
    fi
}

cmd="$1"; shift || true
case "$cmd" in
    run)
        args="$*"
        echo "--------------------------------"
        make all
        echo "--------------------------------"
        check_and_run "$target" $args
        ;;
    all)
        make clean
        make all
        ;;
    debug)
        # TODO: implement debug build
        echo "Debug build not implemented yet."
        ;;
    test)
        flag="$output_dir/$1$suffix"; shift
        make $flag
        args="$*"
        check_and_run "$flag" $args
        ;;
    clean)
        make clean
        ;;
    help|--help|-h|"")
        usage
        ;;
    *)
        echo "Unknown command: $cmd"
        usage
        exit 2
        ;;
esac
