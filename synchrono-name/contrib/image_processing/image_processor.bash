#!/bin/bash

x=1
for i in `ls -v1 *.png`; do
	counter=$(printf %04d $x)
	mv $i frame_"$counter".png
	x=$(($x+1))
done

ffmpeg -framerate 30 -start_number 0001 -i 'frame_%04d.png' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p output.mp4
