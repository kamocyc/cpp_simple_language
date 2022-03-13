base: y.tab.c lex.yy.c base.cpp syntax.cpp
	g++ -g -o base y.tab.c lex.yy.c base.cpp syntax.cpp

lex.yy.c: calc.l
	flex -l calc.l

# y.tab.h:
y.tab.c: calc.y
	bison -dy calc.y

.PHONY: clean
clean:
	rm -f y.tab.c y.tab.h lex.yy.c base
