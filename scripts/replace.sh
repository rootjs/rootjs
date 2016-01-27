#!/bin/bash
for filename in ../src/*.{cc,h}; do
sed 's/namespace RootJS/namespace rootJS/g' $filename > temp && mv temp $filename
done
