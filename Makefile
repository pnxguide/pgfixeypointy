EXTENSION = pgfixeypointy
MODULE_big = pgfixeypointy
DATA = pgfixeypointy--0.0.1.sql
OBJS = src/fxypty.o src/fxypty_impl.o
SHLIB_LINK = -lstdc++ -llibfixeypointy

PG_CONFIG ?= /usr/local/ykerdcha/pgsql/bin/pg_config

override PG_CXXFLAGS += -std=c++17
override PG_CXXFLAGS += -Wno-register

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
