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

\timing on

DROP TABLE IF EXISTS test_fxypty;

CREATE TABLE test_fxypty (
    x fxypty(10,2) NOT NULL,
    y fxypty(10,2) NOT NULL
);

INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_fxypty (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_fxypty (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_fxypty (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_fxypty (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_fxypty (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_fxypty (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_fxypty (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_fxypty (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_fxypty (x, y) VALUES ('6785.78', '457.45');

-- INSERT INTO test_fxypty (x, y) SELECT CAST(random() * 1000000 AS fxypty(10,2)), CAST(random() * 1000000 AS fxypty(10,2)) FROM generate_series(1, 100000);

SELECT * FROM test_fxypty;

SELECT x + y FROM test_fxypty;
SELECT x - y FROM test_fxypty;
SELECT x * y FROM test_fxypty;
SELECT x / y FROM test_fxypty;

SELECT SUM(x) FROM test_fxypty;
SELECT MIN(x) FROM test_fxypty;
SELECT MAX(x) FROM test_fxypty;
SELECT COUNT(x) FROM test_fxypty;

SELECT AVG(x) FROM test_fxypty;
SELECT VARIANCE(x) FROM test_fxypty;
SELECT STDDEV(x) FROM test_fxypty;

DROP TABLE IF EXISTS test_numeric;

CREATE TABLE test_numeric (
    x NUMERIC(10,2) NOT NULL,
    y NUMERIC(10,2) NOT NULL
);

INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_numeric (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_numeric (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_numeric (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_numeric (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_numeric (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_numeric (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_numeric (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_numeric (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_numeric (x, y) VALUES ('6785.78', '457.45');

SELECT * FROM test_numeric;

SELECT x + y FROM test_numeric;
SELECT x - y FROM test_numeric;
SELECT x * y FROM test_numeric;
SELECT x / y FROM test_numeric;

SELECT SUM(x) FROM test_numeric;
SELECT MIN(x) FROM test_numeric;
SELECT MAX(x) FROM test_numeric;
SELECT AVG(x) FROM test_numeric;
SELECT COUNT(x) FROM test_numeric;
SELECT VARIANCE(x) FROM test_numeric;
SELECT STDDEV(x) FROM test_numeric;

DROP TABLE IF EXISTS test_double;

CREATE TABLE test_double (
    x DOUBLE PRECISION NOT NULL,
    y DOUBLE PRECISION NOT NULL
);

INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');
INSERT INTO test_double (x, y) VALUES ('10.12', '24.68');
INSERT INTO test_double (x, y) VALUES ('10.12', '10.123');
INSERT INTO test_double (x, y) VALUES ('245.689', '32.45');
INSERT INTO test_double (x, y) VALUES ('11.23', '12.45');
INSERT INTO test_double (x, y) VALUES ('18.56', '22.45');
INSERT INTO test_double (x, y) VALUES ('91.57', '298.45');
INSERT INTO test_double (x, y) VALUES ('7894.48', '27.45');
INSERT INTO test_double (x, y) VALUES ('7564.78', '45.45');
INSERT INTO test_double (x, y) VALUES ('4824.45', '73.45');
INSERT INTO test_double (x, y) VALUES ('6785.78', '457.45');

SELECT * FROM test_double;

SELECT x + y FROM test_double;
SELECT x - y FROM test_double;
SELECT x * y FROM test_double;
SELECT x / y FROM test_double;

SELECT SUM(x) FROM test_double;
SELECT MIN(x) FROM test_double;
SELECT MAX(x) FROM test_double;
SELECT AVG(x) FROM test_double;
SELECT COUNT(x) FROM test_double;
SELECT VARIANCE(x) FROM test_double;
SELECT STDDEV(x) FROM test_double;