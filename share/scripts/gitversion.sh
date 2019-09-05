#!/bin/sh
git describe --tags | sed 's#-#.#g;s#\.# #g' | awk '{print $1"."$2"."$3"."$4}'

