CC=cc
C-FLAGS=-Wall

mainfile=main.c
others=functions.c instructions.c
object=lc3-vm

all: lc3 

lc3: ${others} ${mainfile}
	${CC} ${C-FLAGS} ${others} ${mainfile} -o ${object}

.PHONY:
clean:
	rm -f lc3
