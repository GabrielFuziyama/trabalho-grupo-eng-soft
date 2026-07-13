.PHONY: all clean test

all:
	mkdir -p build
	cd build && cmake .. && make

test: all
	./bin/marmomanager_tests

clean:
	rm -rf build
	rm -f bin/marmomanager_app
	rm -f bin/marmomanager_tests
