# 8088ify Makefile

PROG =	8088ify
OBJS =	8088ify.o

all: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} ${OBJS}

clean:
	rm -f ${PROG} ${OBJS}
