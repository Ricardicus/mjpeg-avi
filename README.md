#Time lapse this
This is a program that can convert a bunch of jpgs to AVI format. 
You can specify at what rate the jpgs can show up.
#To make it work
You need JPG images of the same width and height. 
Place them in a directory.
Change executable permissions of the script 'create-avi.sh' and compile the C source files.
Run the script 'create-avi.sh' as follows:
./create-avi.sh <path where the jpgs reside> <widthxheight of the jpgs> <the number of jpgs> 
#Example
type the following commands: \n
cd <this-directory> \n
make \n
chmod ugo+x create-avi.sh \n
./create-avi.sh jpgs 1080x720 1
