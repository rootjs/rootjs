#!/bin/bash
for filename in $(dirname $0)/../src/*.{cc,h}; do
	astyle --indent=tab --indent-classes --indent-namespaces -v --suffix=none $filename
done
