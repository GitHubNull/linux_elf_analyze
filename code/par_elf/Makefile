SUBDIRS := src/user src/kernel
ROOT_DIR := $(shell pwd)
default:
	for dir in $(SUBDIRS); do \
		echo "Making default in :" $$dir; \
		(cd $$dir && make $@ ) || exit 1; \
	done;
clean:
	for dir in $(SUBDIRS); do \
		echo "Making clean in :" $$dir; \
		(cd $$dir && make clean $@ ) || exit 1; \
	done;