#!/bin/bash

PLANG=${3:-en}
./tools/pandoc "$1" \
    --include-in-header share/doc/header.tex \
    -V linkcolor:blue \
    -V geometry:a4paper \
    -V geometry:margin=2cm \
    -V lang=$PLANG \
    --pdf-engine=xelatex \
    --toc \
    -o "$2"
