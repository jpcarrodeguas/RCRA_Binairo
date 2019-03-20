# Makefile for RCRA P1 - Binairo Puzzle

all: binairo binairoasp

binairo: binairo.c
	gcc binairo.c -o binairo

binairoasp: binairoasp.c
	gcc binairoasp.c -o binairoasp

clean:
	rm -f binairo binairo.txt binairo.cnf binairoasp binairoasp.txt Binairo_EDB.txt