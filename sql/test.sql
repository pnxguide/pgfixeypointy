DROP EXTENSION IF EXISTS pgfixeypointy CASCADE;
CREATE EXTENSION pgfixeypointy;

DROP TABLE IF EXISTS test_fxypty;
CREATE TABLE test_fxypty (
    x fxypty,
    y fxypty
);

SELECT * FROM test_fxypty;