CREATE TYPE fxypty;

CREATE FUNCTION fxypty_in(cstring, oid, int4)
RETURNS fxypty
AS 'decimal'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_out(fixeddecimal)
RETURNS cstring
AS 'decimal'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE fxypty (
    INTERNALLENGTH = 128,
    INPUT          = fxypty_in,
    OUTPUT         = fxypty_out
);
