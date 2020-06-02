# What is this?
This is a program that can convert a bunch of jpgs to AVI format. 
You can specify at what rate the jpgs can show up.

# To make it work
You need JPG images of the same width and height. 
Place them in a directory.

Name them 1.jpeg, 2.jpeg, 3.jpeg, ..., n.jpeg, in the order you want them
to show up in the video.

Change executable permissions of the script 'create-avi.sh' and compile the C source files.
Run the script 'create-avi.sh' as follows:

```
./create-avi.sh path/to/jpgs widthxheight [fps - optional]
```

# Example
Take the photos I have added in the repo for reference.
I have places jpg files under the folder 'jpgs' of where I
make the dishes in my student apartment.
The dimensions are 800 pixels in width and 600 pixels in height. 
Say we want to display 1 frame per second in the video. 
Then type the following commands in the terminal:

```
cd 'this-directory'

make

chmod ugo+x create-avi.sh 

./create-avi.sh jpgs 800x600 1
```

