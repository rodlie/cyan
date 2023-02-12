#!/bin/bash
#
# Build Cyan README.pdf
#
# Ubuntu: sudo apt install texlive-xetex
# macports: sudo port install textlive-xetex textlive-latex-extra texlive-fonts-recommended texlive-fonts-extra
#
# Download pandoc and copy 'pandoc' binary to this folder.
# https://github.com/jgm/pandoc/releases/tag/2.7.2/
#

cat share/doc/header.md > tmp.md
cat README.md >> tmp.md

#echo "## GNU General Public License v3.0" >> tmp.md
#echo "" >> tmp.md
#cat COPYING >> tmp.md

./tools/md2pdf.sh tmp.md docs/README.pdf

rm tmp.md
