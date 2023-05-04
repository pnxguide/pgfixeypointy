\echo Use "CREATE EXTENSION pgfixeypointy" to load this file. \quit

CREATE TYPE fxypty;

CREATE FUNCTION fxypty_in(cstring, oid, int4)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_out(fxypty)
    RETURNS cstring AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_typmod_in(_cstring)
    RETURNS INT4 AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_typmod_out(int4)
    RETURNS cstring AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE fxypty (
    INTERNALLENGTH  = 32,
    INPUT           = fxypty_in,
    OUTPUT          = fxypty_out,
    TYPMOD_IN       = fxypty_typmod_in,
    TYPMOD_OUT      = fxypty_typmod_out
);

CREATE FUNCTION fxypty_add(fxypty, fxypty)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_subtract(fxypty, fxypty)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_multiply(fxypty, fxypty)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_divide(fxypty, fxypty)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_eq(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_neq(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_lt(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_gt(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_lte(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_gte(fxypty, fxypty)
    RETURNS bool AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR + (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_add,
    commutator = +
);

CREATE OPERATOR - (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_subtract
);

CREATE OPERATOR * (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_multiply,
    commutator = *
);

CREATE OPERATOR / (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_divide
);

CREATE OPERATOR = (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_eq,
    commutator = =,
    negator = <>
);

CREATE OPERATOR <> (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_neq,
    commutator = <>,
    negator = =
);

CREATE OPERATOR < (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_lt,
    commutator = >,
    negator = >=
);

CREATE OPERATOR > (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_gt,
    commutator = <,
    negator = <=
);

CREATE OPERATOR <= (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_lte,
    commutator = >=,
    negator = >
);

CREATE OPERATOR >= (
    leftarg = fxypty,
    rightarg = fxypty,
    function = fxypty_gte,
    commutator = <=,
    negator = <
);

CREATE FUNCTION fxypty(fxypty, int4)
    RETURNS fxypty AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE CAST (fxypty AS fxypty)
	WITH FUNCTION fxypty(fxypty, int4) AS ASSIGNMENT;

-- aggregates
CREATE FUNCTION fxypty_smaller(fxypty, fxypty) 
    RETURNS fxypty AS 'MODULE_PATHNAME' 
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION fxypty_larger(fxypty, fxypty) 
    RETURNS fxypty AS 'MODULE_PATHNAME' 
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE sum(fxypty) (
    sfunc = fxypty_add,
    stype = fxypty
);

CREATE AGGREGATE min(fxypty) (
    sfunc = fxypty_smaller,
    stype = fxypty
);

CREATE AGGREGATE max(fxypty) (
    sfunc = fxypty_larger,
    stype = fxypty
);

-- CREATE FUNCTION fxypty_avg_accum(fxypty[], fxypty)
--     RETURNS fxypty[] AS
--     'SELECT array[$1[1] + $2, $1[2] + CAST (0 AS fxypty)];'
--     LANGUAGE SQL;

-- CREATE FUNCTION fxypty_avg_final(fxypty [])
--     RETURNS fxypty AS
--     'SELECT CASE 
--         WHEN $1[2] != 0
--         THEN $1[1]/$1[2]
--         ELSE null::fxypty
--     END;'
--     LANGUAGE SQL;

-- CREATE AGGREGATE avg(fxypty) (
--     sfunc = fxypty_avg_accum,
--     stype = fxypty[],
--     FINALFUNC = fxypty_avg_final,
--     INITCOND = '{0,0}'
-- );

-- CREATE AGGREGATE var(fxypty) (
--     sfunc = fxypty_accum,
--     stype = fxypty [],
--     FINALFUNC = lfp_var,
--     INITCOND = '{0,0,0}'
-- );

-- CREATE AGGREGATE stdev(fxypty) (
--     sfunc = fxypty_accum,
--     stype = fxypty [],
--     FINALFUNC = lfp_var,
--     INITCOND = '{0,0,0}'
-- );