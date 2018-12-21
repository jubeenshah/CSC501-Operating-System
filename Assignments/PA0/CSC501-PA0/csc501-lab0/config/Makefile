#
# Make the Xinu configuration program
#
CC	= /usr/bin/gcc	
LEX	= /usr/bin/flex
YACC	= /usr/bin/bison -y     # Flag enables yacc-compatible filenames
DEFS	=
#CFLAGS	= -march=i586 ${DEFS}
CFLAGS	= -m32 ${DEFS}
LFLAGS	= -lfl

all:		config

config:		lex.yy.c y.tab.c
		${CC} ${CFLAGS} -o config y.tab.c ${LFLAGS}

lex.yy.c:	config.l
		${LEX} config.l

y.tab.c:	config.y
		${YACC} config.y

clean:
		/bin/rm -f lex.yy.c y.tab.c core
