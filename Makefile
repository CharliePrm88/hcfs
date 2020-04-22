COMPILER = gcc
FILESYSTEM_FILES = hcfs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o hcfs `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./hcfs -f [mount point]'

clean:
	rm hcfs
