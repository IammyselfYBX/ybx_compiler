TARGET = ./bin/ybx 
CC=gcc
OBJS = \
  lex.yy.o\
  y.tab.o\
  main.o\
  interface.o\
  create.o\
  execute.o\
  eval.o\
  string.o\
  string_pool.o\
  util.o\
  native.o\
  error.o\
  error_message_not_gcc.o\
  ./memory/mem.o\
  ./debug/dbg.o
CFLAGS = -c -g -Wall -Wswitch-enum -ansi -pedantic -DDEBUG  --std=c99
INCLUDES = \

$(TARGET):$(OBJS)
	mkdir bin
	cd ./memory; $(MAKE);
	cd ./debug; $(MAKE);
	$(CC) $(OBJS) -o $@ -lm
  
clean:
	rm -rf ./bin *.o lex.yy.c y.tab.c y.output y.tab.h *~ debug/*.o memory/*.o

install:
	make
	sudo cp ./bin/ybx  /bin/
	echo success!

uninstall:
	sudo rm /bin/ybx


y.tab.h : ybx.y
	bison --yacc -dv ybx.y
y.tab.c : ybx.y
	bison --yacc -dv ybx.y
lex.yy.c : ybx.l ybx.y y.tab.h
	flex ybx.l
y.tab.o: y.tab.c ybxcompiler.h MEM.h
	$(CC) -c -w -g $*.c $(INCLUDES)
lex.yy.o: lex.yy.c ybxcompiler.h MEM.h
	$(CC) -c -w -g $*.c $(INCLUDES)
.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)
./memory/mem.o:
	cd ./memory; $(MAKE);
./debug/dbg.o:
	cd ./debug; $(MAKE);

create.o: create.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
error.o: error.c MEM.h ybxcompiler.h YBX.h YBX_dev.h
error_message.o: error_message.c ybxcompiler.h MEM.h YBX.h YBX_dev.h
error_message_not_gcc.o:error_message_not_gcc.c ybxcompiler.h MEM.h YBX.h YBX_dev.h
eval.o: eval.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
execute.o: execute.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
interface.o: interface.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
main.o: main.c YBX.h MEM.h
native.o: native.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
string.o: string.c MEM.h ybxcompiler.h YBX.h YBX_dev.h
string_pool.o: string_pool.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
util.o: util.c MEM.h DBG.h ybxcompiler.h YBX.h YBX_dev.h
