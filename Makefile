CC = g++ -g
CFLAGS = -Wall



all: jack_delay jack_echo jack_fuzz jack_flanger


jack_delay: jack_delay.o
	$(CC) -o $@ $(CFLAGS) jack_delay.o -ljack

jack_echo: jack_echo.o
	$(CC) -o $@ $(CFLAGS) jack_echo.o -ljack

jack_fuzz: jack_fuzz.o
	$(CC) -o $@ $(CFLAGS) jack_fuzz.o -ljack

jack_flanger: jack_flanger.o
	$(CC) -o $@ $(CFLAGS) jack_flanger.o -ljack


.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm +111 -type f`

