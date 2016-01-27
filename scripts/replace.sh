#!/bin/bash
for filename in ../src/*.{cc,h}; do
sed 's/RootJS/rootJS/g' $filename > temp && mv temp $filename
done
