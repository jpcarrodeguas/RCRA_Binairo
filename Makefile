# Makefile for RCRA P1 - Binairo Puzzle

all: sat asp

sat: binairo.c
	gcc binairo.c -o binairo

asp: binairoasp.c
	gcc binairoasp.c -o binairoasp

clean:
	rm -f binairo binairo.txt binairo.cnf binairoasp binairoasp.txt Binairo_EDB.txt