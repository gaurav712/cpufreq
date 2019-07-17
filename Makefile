CC="gcc"
CFLAGS="-Wall"

all:
	$(CC) -o cpufreq $(CFLAGS) cpufreq.c

clean:
	rm -f cpufreq

install: all
	cp cpufreq /usr/local/bin/cpufreq

uninstall:
	rm -f /usr/local/bin/cpufreq
