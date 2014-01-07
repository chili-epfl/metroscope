#!/bin/sh

grep -r craftag *.cfg | sed 's|\(.*\).cfg.*id:[^0-9]*\([0-9]*\).*|\2  \1|g' | sort -n
