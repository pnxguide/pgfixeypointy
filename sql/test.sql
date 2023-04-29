DROP EXTENSION IF EXISTS pgfixeypointy CASCADE;

CREATE EXTENSION pgfixeypointy;

SELECT '10.12'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) + '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) - '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) * '245.689'::fxypty(10,2);

SELECT '245.689'::fxypty(10,2) / '10.12'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) = '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) = '10.123'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) <> '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) <> '10.123'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) < '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) < '10.123'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) > '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) > '10.123'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) <= '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) <= '10.123'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) >= '245.689'::fxypty(10,2);

SELECT '10.12'::fxypty(10,2) >= '10.123'::fxypty(10,2);


DROP TABLE IF EXISTS test_fxypty;

CREATE TABLE test_fxypty (
    x fxypty(10,2),
    y fxypty(10,2)
);

INSERT INTO test_fxypty (x, y) VALUES ('10.12', '2.45');

SELECT * FROM test_fxypty;

SELECT x + y FROM test_fxypty;