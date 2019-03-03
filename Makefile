# Makefile for RCRA P1 - Binairo Puzzle

binairo: binairo.c
	gcc binairo.c -o binairo

clean:
	rm -f binairo binairo.txt binairo.cnf
