CC= g++ -O2 -g
BOOST_HOME=/home/zhangzj/local/boost
GLIB_HOME=/home/zhangzj/local/glib

INCLUDES= -I./ -I${BOOST_HOME}/include/ -I${GLIB_HOME}/include/glib-2.0/ -I${GLIB_HOME}/include/glib-2.0/glib/ -I${GLIB_HOME}/lib/glib-2.0/include/
LIBS= -lglib-2.0 -lboost_system
CPPFLAGS= -fPIC -c -Wno-deprecated
O_OBJECT= token.o process.o main.o file.o utils.o hasharray.o
DEST= yam4 libyam4.a libyam4.so

all: ${DEST}

yam4: ${O_OBJECT}
	${CC} -Wall -fPIC -rdynamic -o $@ ${O_OBJECT} ${INCLUDES} ${LIBS}

libyam4.a: ${O_OBJECT}
	ar cqs libyam4.a  ${O_OBJECT}

libyam4.so: ${O_OBJECT}
	${CC} --shared -fPIC -o libyam4.so ${O_OBJECT} ${LIBS}

token.o: token.cc token.h
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

process.o: process.cc process.h
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

main.o: main.cc
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

file.o: file.cc
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

utils.o: utils.cc
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

hasharray.o: hasharray.cc
	${CC} -Wall ${CPPFLAGS} $< -o $@ ${INCLUDES}

clean:
	rm -f *.o ${DEST}

install:
	cp ${DEST} ~/local/yam4/bin/
	cp *.h ~/local/yam4/include/
	cp libyam4.* ~/local/yam4/lib/

