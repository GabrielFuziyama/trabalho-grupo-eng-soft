.PHONY: all clean test

all:
	mkdir -p build
	cd build && cmake .. && make

test: all
	cd build && ctest --output-on-failure

clean:
	rm -rf build
	rm -f bin/marmomanager_app
	rm -f bin/marmomanager_tests
	rm -f bin/marmomanager_gui_tests
	rm -f bin/marmomanager_login_tests
	rm -f bin/marmomanager_acceptance_tests
	rm -f bin/libmarmomanager_data.so
	rm -f bin/libmarmomanager_business.so
	rm -f bin/libmarmomanager_presentation.so
