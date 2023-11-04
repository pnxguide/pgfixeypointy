test: extended_types.h throwError.h wide_integer.h types.h Decimal_fwd.h Decimal.h test.cpp
	rm -rf test.out;
	rm -rf *.gch;
	g++ -std=c++23 $^ -o test.out;

full: *.h *.cpp
	rm -rf full.out;
	rm -rf *.gch;
	g++ -std=c++23 $^ -o full.out 2> error.log;