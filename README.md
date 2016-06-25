#What is this?
This is a program that can convert a bunch of jpgs to AVI format. 
You can specify at what rate the jpgs can show up.
#To make it work
You need JPG images of the same width and height. 
Place them in a directory.
Change executable permissions of the script 'create-avi.sh' and compile the C source files.
Run the script 'create-avi.sh' as follows:
./create-avi.sh <path where the jpgs reside> <widthxheight of the jpgs> <the number of jpgs> 
#Example
Take a couple of photos and place them in a directory called for example 'jpgs'. 
Check their dimensions, let's say 1080 pixels in width and 720 pixels in height. 
Say we want to display 1 frame per second in the video. 
Then type the following commands in the terminal:

cd 'this-directory'

make 

chmod ugo+x create-avi.sh 

./create-avi.sh jpgs 1080x720 1
