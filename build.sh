#!/usr/bin/env bash
set -e
target="$(make -s print-OUTPUT_DIR)/$(make -s print-TARGET)"

# 简单帮助
usage() {
    cat << EOF
Usage: $(basename "$0") <command> [args...]
    Commands:
    run                 Build (if needed) and run the emulator with given ROM.
                            -v : show version
    help                Show this help.
EOF
}

# 尝试构建（调用 make），但不失败于已存在的可执行文件
ensure_built() {
    if command -v make >/dev/null 2>&1; then
        echo "Building project with make..."
        make -s
        echo "Build finished. "
    fi
    if [ -x "$target" ] || [ -x "$target.exe" ]; then
        return 0
    fi
}

cmd="$1"; shift || true
case "$cmd" in
    run)
        args="$1"
        echo "--------------------------------"
        ensure_built
        echo "--------------------------------"
        if [ -x "$target" ]; then
            $target $args
        elif [ -x "$target" ]; then
            $target $args
        else
            echo "Error: executable $target not found after build."
            exit 1
        fi
        ;;
    all)
        make clean
        make all
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
