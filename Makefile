example:
	gcc -Wall -O3 -o example example_main.c saleae_v2_digital.c mmap.c
	
qspi:
	gcc -Wall -O3 -o qspi qspi_main.c saleae_v2_digital.c mmap.c

clean:
	rm -f example qspi

all: clean qspi