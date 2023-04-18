SET
    client_min_messages = warning;

-- SQL definitions
CREATE TYPE lfp_decimal;

-- basic i/o functions
CREATE
OR REPLACE FUNCTION decimal_in(cstring) RETURNS lfp_decimal AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_out(lfp_decimal) RETURNS cstring AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE lfp_decimal (
    input = decimal_in,
    output = decimal_out,
    passedbyvalue
);

-- functions for operators
CREATE
OR REPLACE FUNCTION decimal_lt(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_le(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_eq(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_ne(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_ge(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_gt(lfp_decimal, lfp_decimal) RETURNS bool AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_cmp(lfp_decimal, lfp_decimal) RETURNS int4 AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
-- operators
CREATE OPERATOR < (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_lt,
    commutator = >,
    negator = >=,
    restrict = scalarltsel,
    join = scalarltjoinsel
);

CREATE OPERATOR <= (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_le,
    commutator = >=,
    negator = >,
    restrict = scalarltsel,
    join = scalarltjoinsel
);

CREATE OPERATOR = (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_eq,
    commutator = =,
    negator = <>,
    restrict = eqsel,
    join = eqjoinsel,
    merges
);

CREATE OPERATOR <> (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_ne,
    commutator = <>,
    negator = =,
    restrict = neqsel,
    join = neqjoinsel
);

CREATE OPERATOR > (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_gt,
    commutator = <,
    negator = <=,
    restrict = scalargtsel,
    join = scalargtjoinsel
);

CREATE OPERATOR >= (
    leftarg = lfp_decimal,
    rightarg = lfp_decimal,
    procedure = decimal_ge,
    commutator = <=,
    negator = <,
    restrict = scalargtsel,
    join = scalargtjoinsel
);

-- aggregates
CREATE
OR REPLACE FUNCTION decimal_smaller(lfp_decimal, lfp_decimal) RETURNS lfp_decimal AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE
OR REPLACE FUNCTION decimal_larger(lfp_decimal, lfp_decimal) RETURNS lfp_decimal AS '$libdir/lfp_decimal' LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE sum(lfp_decimal) (
    SFUNC = decimal_add,
    STYPE = lfp_decimal,
    INITCOND = '0'
);

CREATE AGGREGATE min(lfp_decimal) (
    SFUNC = decimal_smaller,
    STYPE = lfp_decimal,
    SORTOP = <
);

CREATE AGGREGATE max(lfp_decimal) (
    SFUNC = decimal_larger,
    STYPE = lfp_decimal,
    SORTOP = >
);

CREATE AGGREGATE avg (lfp_decimal) (
    SFUNC = decimal_accum,
    STYPE = lfp_decimal [],
    FINALFUNC = lfp_avg,
    INITCOND = '{0,0,0}'
);

CREATE AGGREGATE var(lfp_decimal) (
    SFUNC = decimal_accum,
    STYPE = lfp_decimal [],
    FINALFUNC = lfp_var,
    INITCOND = '{0,0,0}'
);

CREATE AGGREGATE stdev(lfp_decimal) (
    SFUNC = decimal_accum,
    STYPE = lfp_decimal [],
    FINALFUNC = lfp_var,
    INITCOND = '{0,0,0}'
);