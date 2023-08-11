benchmark_to_string: to_string.cpp
	gcc -o pg_numeric.out -c ../../third_party/pg_numeric/pg_numeric.c;
	g++ to_string.cpp \
		pg_numeric.out \
		-o benchmark_to_string.out \
		-L/home/ykerdcha/postgres/pgfixeypointy/third_party/libfixeypointy/build/ \
		-llibfixeypointy \
		-L/home/ykerdcha/arrow/cpp/build/release/ \
		-larrow;
	./benchmark_to_string.out > benchmark_to_string.log;

test_arrow_to_string: test_arrow_to_string.cpp
	g++ test_arrow_to_string.cpp \
		-o test_arrow_to_string.out \
		-L/home/ykerdcha/arrow/cpp/build/release/ \
		-larrow;
	./test_arrow_to_string.out > test_arrow_to_string.log;

perf_to_string:
	g++ to_string_longrun.cpp \
		-o perf_to_string.out \
		-L/home/ykerdcha/postgres/pgfixeypointy/third_party/libfixeypointy/build/ \
		-llibfixeypointy;
	perf record -a -F 99 -g -- ./perf_to_string.out;
	perf script | stackcollapse-perf.pl | flamegraph.pl > perf_to_string.svg;

perf_to_string_worst:
	g++ to_string_worst.cpp \
		pg_numeric.out \
		-o perf_to_string_worst.out \
		-L/home/ykerdcha/postgres/pgfixeypointy/third_party/libfixeypointy/build/ \
		-llibfixeypointy;
	perf record -a -F 99 -g -- ./perf_to_string_worst.out;
	perf script | stackcollapse-perf.pl | flamegraph.pl > perf_to_string_worst.svg;

benchmark_divide: divide.cpp
	g++ divide.cpp \
		-o benchmark_divide.out \
		-L/home/ykerdcha/postgres/pgfixeypointy/third_party/libfixeypointy/build/ \
		-llibfixeypointy;
	./benchmark_divide.out > benchmark_divide.log;

perf_divide:
	g++ divide_longrun.cpp \
		-o perf_divide.out \
		-L/home/ykerdcha/postgres/pgfixeypointy/third_party/libfixeypointy/build/ \
		-llibfixeypointy;
	perf record -a -F 99 -g -- ./perf_divide.out;
	perf script | stackcollapse-perf.pl | flamegraph.pl > perf_divide.svg;

clean:
	rm -rf *.out;
	rm -rf *.data;
	rm -rf *.data.old;