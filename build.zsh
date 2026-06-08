#!/bin/zsh

# Compiler
CC="gcc"

# Flags as an array
FLAGS=(-Wall -std=c11)
LIBS=(-lglfw -lGL -lm -lassimp)

# Source files
SRCS=(src/main.c vendor/glad/glad.c src/Shader/shader.c src/Model/model.c src/Common/objects.c)

# Output binary
TARGET="a.exe"

# Build mode (default: release)
for arg in "$@"; do
    if [[ $arg == "debug" ]]; then
        FLAGS+=(-g)
    fi

    if [[ $arg == "benchmark" ]]; then
        SRCS[1]='src/benchmark.c'
        TARGET="b.exe"
    fi
done

# Run the build
echo "🔧 Building [$MODE]..."
echo "$CC ${FLAGS[@]} -o $TARGET ${SRCS[@]} ${LIBS[@]}"
$CC "${FLAGS[@]}" -o "$TARGET" "${SRCS[@]}" "${LIBS[@]}"

# Check success
if [[ $? -eq 0 ]]; then
  echo "✅ Build succeeded: $TARGET"
else
  echo "❌ Build failed"
  exit 1
fi
