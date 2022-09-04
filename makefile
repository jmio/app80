#
#      Generic-App Makefile              by Mio  1995/03/02
#                                        by jmio 2021/03/01
#


LD      = gcc                         
CC      = gcc        
#LD      = /c/msys64/mingw32/bin/gcc                  
#CC      = /c/msys64/mingw32/bin/gcc       

CFLAGS  = -DAPP80 
#CFLAGS = -DAPP86

LDFLAGS = -static
#
APP8xOBJS = appsub.o miscloop.o superstr.o apperr.o\
            ifdefs.o condexp.o ifswitch.o function.o
APP8xOBJ  = appsub miscloop superstr \
            ifdefs condexp ifswitch function apperr

APP86OBJS = eval80.o cond86.o code86.o tools86.o misc86.o helpmsg86.o
APP86OBJ  = main86 eval86 cond86 code86 tools86 misc86 appmsg86

APP80OBJS = eval80.o cond80.o code80.o tools80.o misc80.o helpmsg.o
APP80OBJ  = eval80 cond80 code80 tools80 misc80 helpmsg
STDDEPS = appsub.h appdefs.h apperr.h proto.h

app80.exe: $(APP8xOBJS) $(APP80OBJS)
	$(LD) $(APP8xOBJS) $(APP80OBJS) $(CFLAGS) $(LDFLAGS) appmain.c -o app80.exe

app86.exe: $(APP8xOBJS) $(APP86OBJS)
	$(LD) $(APP8xOBJS) $(APP86OBJS) $(CFLAGS) $(LDFLAGS) appmain.c -o app86.exe

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f app80.exe
	rm -f app86.exe

appsub.o: appsub.c appsub.h
miscloop.o: miscloop.c miscloop.h
cond80.o: cond80.c cond80.h
cond86.o: cond86.c cond86.h
code80.o: code80.c code80.h
code86.o: code86.c code86.h
misc80.o: misc80.c misc80.h
misc86.o: misc86.c misc86.h
eval80.o: eval80.c misc80.h
superstr.o: superstr.c superstr.h
apperr.o: apperr.c apperr.h
ifdefs.o: ifdefs.c ifdefs.h
condexp.o: condexp.c condexp.h
ifswitch.o: ifswitch.c ifswitch.h 
function.o: function.c function.h
tools80.o: tools80.c tools80.h
tools86.o: tools86.c tools86.h
helpmsg.o: helpmsg.c
helpmsg86.o: helpmsg86.c





