#!/usr/bin/env bash

if [[ "$1" == "cpp" ]]; then
  find examples -type f -name "*.ino" | while read -r FNAME; do
      mv -v "$FNAME" "${FNAME%.ino}.cpp"
  done
elif [[ "$1" == "ino" ]]; then
  find examples -type f -name "*.cpp" | while read -r FNAME; do
    mv -v "$FNAME" "${FNAME%.cpp}.ino"
  done
elif [[ "$1" == "e_run" ]]; then
  if [[ -n "$2" ]]; then
    pio ci -l src/ -c platformio.ini "$2" -e nano32 --keep-build-dir

  else
    for example in examples/*; do
      echo "compiling $example/$example.cpp"
      pio ci -l src/ -c platformio.ini $example/ -e nano32 --keep-build-dir
    done
  fi
elif [[ "$1" == "e_upload" ]]; then
  if [[ -n "$2" ]]; then
    pio ci -l src/ -c platformio.ini "$2" -e upload --keep-build-dir
  else
    echo "Error: Please specify an example to upload."
    exit 1
  fi
else
  echo "Usage: $0 command [argument]"
  echo "  cpp: Rename .ino examples to .cpp"
  echo "  ino: Rename .cpp examples to .ino"
  echo "  e_run [example]: Compile all examples or specify in the second argument"
  echo "  e_upload <example>: Compile & upload example (specify in the second argument)"
  exit 1
fi
