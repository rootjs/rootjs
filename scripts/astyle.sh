#!/bin/bash
for filename in $(dirname $0)/../src/*.{cc,h}; do
	astyle --style=kr --indent=tab --indent-classes --indent-namespaces -v --suffix=none $filename
done
