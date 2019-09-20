BIN=hackasm
SRC=main.c parser.c symtab.c util.c
HDR=data.h parser.h symtab.h util.h
OBJS=${SRC:%.c=%.o}

all: ${BIN}

${BIN}: ${OBJS}
	${CC} -o $@ ${OBJS}

%.o: %.c ${HDR}
	${CC} -c $<

clean:
	rm -f ${OBJS} ${BIN}
