# Level format

A level mainly consists of three parts:

1. An image that describes the geometry
2. The id table file
3. The extra data file

These three files are alone not enough to describe a level, they're used to reference tile-descriptor files. These files are not exclusive for a level and may be shared between levels and/or tiles may be defined in code.

All three files must be in the same directory. The name of the level is the same as the name of the directory. 
All files have a fixed name:


|		Part		|	Filename	|
|-------------------|---------------|
|1. geometry image	|	"layout.png"|
|2. id table 		|	"ids.txt"	|
|3. extra data		| *TODO* 		|


### 1. Level geometry
The geometry of a level file is described by a simple RGB *.png image. The color value at each pixel of the image is interpreted as a single 3-byte unsigned ID. IDs do not need to be consecutive.

### 2. ID table file
This table translates the numeric ID from the geometry image to a tile-descriptor name. The format of this file is a simple plaintext file with a ID <-> name pair in each line. Names may only contain alphanumeric characters and the following punktuation: `_/-`. Lines that start with `#` or consist entirely of whitespace are ignored.

Example Table:
```
# This is a comment
0 air

1 environment/dirt
2 environment/stone
```

### 3. Extra data file
TODO, maybe JSON or something