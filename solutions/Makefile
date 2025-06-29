SRC := Grimaud_Mandelbrot
EXE := mandelbrot
CFLAGS := -Wall -g -O3

all: $(EXE)

.PHONY: clean

$(EXE): $(SRC).c rainbow.h
	gcc $(CFLAGS) -o $@ $<
	time ./$@
	magick -size 3840x2160 -depth 8 $@.rgba $@.jpg
	rm $@.rgba
	rm -r $@.dSYM

clean:
	rm $(SRC).o $(EXE).rgba $(EXE).jpg $(EXE)
