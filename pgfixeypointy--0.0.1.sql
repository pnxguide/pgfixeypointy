\echo Use "CREATE EXTENSION pgfixeypointy" to load this file. \quit

CREATE TYPE fxypty;

CREATE FUNCTION fxypty_in(cstring)
RETURNS fxypty AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_out(fxypty)
RETURNS cstring AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE fxypty (
    INTERNALLENGTH = 256,
    INPUT          = fxypty_in,
    OUTPUT         = fxypty_out
);
