CPPFLAGS = -std=c++11 -Wall -g -DDEBUG -Iinc/ -Itestinc/
CPP = g++

bin/graphicaltest2: testsrc/graphicaltest2.cc dmx.o controls.o periodicworker.o serial.o config.o 
	${CPP} ${CPPFLAGS} -o bin/graphicaltest2 testsrc/graphicaltest2.cc dmx.o controls.o periodicworker.o serial.o config.o -lconfuse -lrt -lSDL -lSDL_ttf -lfontconfig -Llib/ -ltgui

bin/newdmx: testsrc/newdmx.cc dmx.o periodicworker.o serial.o config.o
	${CPP} ${CPPFLAGS} -o bin/newdmx testsrc/newdmx.cc dmx.o periodicworker.o serial.o config.o -lrt -lconfuse

bin/graphicaltest: testsrc/graphicaltest.cc dmxio.o periodicworker.o serial.o config.o pattern.o
	${CPP} ${CPPFLAGS} -o bin/graphicaltest testsrc/graphicaltest.cc dmxio.o periodicworker.o serial.o config.o pattern.o -lconfuse -lrt -lSDL -lSDL_ttf -lfontconfig -Llib/ -ltgui

bin/dmxiotest: testsrc/dmxiotest.cc dmxio.o periodicworker.o serial.o config.o
	${CPP} ${CPPFLAGS} -o bin/dmxiotest testsrc/dmxiotest.cc dmxio.o periodicworker.o serial.o config.o -lconfuse -lrt

bin/serialtest: testsrc/serialtest.cc serial.o config.o
	${CPP} ${CPPFLAGS} -o bin/serialtest testsrc/serialtest.cc serial.o config.o -lconfuse

bin/configtest: testsrc/configtest.cc config.o
	${CPP} ${CPPFLAGS} -o bin/configtest testsrc/configtest.cc config.o -lconfuse

dmxio.o: src/dmxio.cc inc/dmxio.hh inc/periodicworker.hh
	${CPP} ${CPPFLAGS} -c src/dmxio.cc -lrt

periodicworker.o: src/periodicworker.cc inc/periodicworker.hh
	${CPP} ${CPPFLAGS} -c src/periodicworker.cc -lrt

serial.o: src/serial.cc inc/serial.hh
	${CPP} ${CPPFLAGS} -c src/serial.cc

config.o: src/config.cc inc/config.hh
	${CPP} ${CPPFLAGS} -c src/config.cc -lconfuse

pattern.o: src/pattern.cc inc/pattern.hh
	${CPP} ${CPPFLAGS} -c src/pattern.cc

dmx.o: src/dmx.cc inc/dmx.hh
	${CPP} ${CPPFLAGS} -c src/dmx.cc

controls.o: src/controls.cc inc/controls.hh
	${CPP} ${CPPFLAGS} -c src/controls.cc

	
.PHONY: configtest_successful_run
configtest_successful_run: bin/configtest
	./bin/configtest

