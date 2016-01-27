#!/bin/bash
for filename in ../src/*.{cc,h}; do
astyle --indent=tab --indent-namespaces -v --suffix=none $filename
done
