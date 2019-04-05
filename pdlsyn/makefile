
ast_depends = port.c connection.c config.c edge.c vertex.c transition.c condition.c packet.c ref.c value.c transaction.c parameter.c
fsm_depends = fsm.c path.c body.c
protocol_depends = protocol.c rtl_block.c transaction_sum.c
common_depends = common.c btimer.c

warning_flags = -Wno-write-strings -Wno-deprecated -Wfatal-errors

all: parse lex compile

parse:
	bison -d parser.y

lex:
	flex lexer.l

compile:
	g++ -std=c++0x -o pdlsyn -g lex.yy.c parser.tab.c $(ast_depends) $(fsm_depends) $(protocol_depends) $(common_depends) main.c $(warning_flags) -lpthread

run:
	./pdlsyn

clean:
	rm -rf *.tab.c *.tab.h *.yy.c pdlsyn html latex *.conf

docs:
	doxygen -g docs.conf;
	doxygen docs.conf;

