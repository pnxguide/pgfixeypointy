DROP EXTENSION IF EXISTS pgfixeypointy CASCADE;

CREATE EXTENSION pgfixeypointy;

DROP TABLE IF EXISTS test_fxypty;

CREATE TABLE test_fxypty (
    x fxypty,
    y fxypty
);

INSERT INTO test_fxypty (x, y) VALUES ('10.123', '2.45');

SELECT * FROM test_fxypty;