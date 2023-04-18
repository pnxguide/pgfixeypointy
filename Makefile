EXTENSION = pgfixeypointy
MODULE_big = pgfixeypointy
DATA = pgfixeypointy--0.0.1.sql
OBJS = src/decimal.o src/decimal_impl.o
SHLIB_LINK = -lstdc++

PG_CONFIG ?= /opt/local/pgsql/bin/pg_config
PG_CONFIG ?= pg_config

override PG_CXXFLAGS += -std=c++17
override PG_CXXFLAGS += -Wno-register

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# all:
# g++-12 -O2 -std=c++17 -I. -I./ -I/opt/local/pgsql/include/server \
# 	-I/opt/local/pgsql/include/internal \
# 	-c src/decimal_impl.cpp \
# 	-o src/decimal_impl.o
