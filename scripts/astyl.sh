#!/bin/sh
for filename in ../src/*; do
astyle --indent=tab --indent-namespaces -v --suffix=none $filename
done
