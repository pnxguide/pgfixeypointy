EXTENSION = pgfixeypointy
MODULE_big = pgfixeypointy
DATA = pgfixeypointy--0.0.1.sql
OBJS = src/decimal.o src/decimal_impl.o
SHLIB_LINK = -lstdc++

PG_CONFIG ?= /usr/local/pgsql/bin/pg_config
PG_CONFIG ?= pg_config

override PG_CXXFLAGS += -std=c++17
override PG_CXXFLAGS += -Wno-register

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
