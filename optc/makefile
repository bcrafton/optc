
warning_flags = -Wno-write-strings -Wno-deprecated -Wfatal-errors

all: parse lex compile

parse:
	bison -d parser.y

lex:
	flex lexer.l

compile:
	g++ -std=c++0x -o optc -g lex.yy.c parser.tab.c main.c $(warning_flags) -lpthread

run:
	./optc

clean:
	rm -rf *.tab.c *.tab.h *.yy.c pdlsyn html latex *.conf

