CC = cc
CFLAGS = -Wall -O2
LDFLAGS = -lm

all: gpss datran datpts datgen datcnf sum
%.o: %.c
	$(CC) -c $(CFLAGS) $<
gpss: gpss.o
	$(CC) -o $@ $^ $(LDFLAGS)
datran: datran.o
	$(CC) -o $@ $^ $(LDFLAGS)
datpts: datpts.o
	$(CC) -o $@ $^ $(LDFLAGS)
datgen: datgen.o
	$(CC) -o $@ $^ $(LDFLAGS)
datcnf: datcnf.o
	$(CC) -o $@ $^ $(LDFLAGS)
sum: sum.o
	$(CC) -o $@ $^ $(LDFLAGS)
clean:
	rm -f *.o gpss datran datpts datgen datcnf sum
