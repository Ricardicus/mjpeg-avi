#!/bin/bash
# Should be called with the path to the jpgs to be converted as first argument
# resolution of the jpgs as the second argument, optional fps as third

path_jpgs=$1
resolution_jpgs=$2
fps_jpgs=10

if [ $# -eq 3 ]; then
	fps_jpgs=$3
fi

_dir="jpgs"
_unique_name_count=1

while [ -d $_dir ]; do
	_dir=$_dir$_unique_name_count
	_unique_name_count=$((_unique_name_count+1))
done
mkdir $_dir

count=1
for file in $(ls $path_jpgs); do
	e=${file%*.jpeg}
	f=${file%*.jpg}
	if [ ! $e = $file ] || [ ! $f = $file ]; then
		cp $path_jpgs/$file $_dir/$count.jpeg
		count=$((count+1))
	fi
done

# calling the program!
./avimake -r $resolution_jpgs -l $_dir -n $((count-1)) -s $fps_jpgs

# cleanup 
if cd $_dir; then
	rm *
	cd ..
	rmdir $_dir
fi

