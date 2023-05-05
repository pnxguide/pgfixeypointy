import psycopg2
import random
import time

conn = psycopg2.connect(
    host="localhost",
    database="test",
    user="postgres"
)

lines = 4000000
bound = 100000
trials = 6

table_list = ["test_fxypty", "test_numeric", "test_double", "test_real"]
type_list = ["fxypty(30,10)", "NUMERIC(30,10)", "DOUBLE PRECISION", "REAL"]

cur = conn.cursor()

cur.execute(f"DROP EXTENSION IF EXISTS pgfixeypointy CASCADE;")
cur.execute(f"CREATE EXTENSION pgfixeypointy;")

f = open("./output.out", "w")

val_x_list = []
val_y_list = []
# query_list = ["init", "*","x + y","x - y","x * y","x / y","SUM(x)","MIN(x)","MAX(x)","AVG(x)","COUNT(x)","VARIANCE(x)","STDDEV(x)"]

query_list = ["init", "*","x+y","x-y","x*y","x/y","SUM(x)","MIN(x)","MAX(x)","COUNT(x)","SUM(x),SUM(y)", "MIN(x),MIN(y)", "MAX(x),MAX(y)", "COUNT(x),COUNT(y)"]

for line in range(lines):
    val_x_list.append(random.uniform(1, bound))
    val_y_list.append(random.uniform(1, bound))

# for idx in range(len(table_list)):
for idx in range(1):
    table_name = table_list[idx]
    val_type = type_list[idx]
    
    print("{}, {}".format(table_name, val_type))

    clean_query = f"DROP TABLE IF EXISTS {table_name};"
    create_query = f"CREATE TABLE {table_name} (x {val_type} NOT NULL, y {val_type} NOT NULL);"
    
    f.write("{}, {}\n".format(table_name, val_type))

    result = [0] * len(query_list)
    for i in range(len(query_list)):
        result[i] = [0.0] * trials

    add_query_list = []
    for line in range(lines):
        add_query = f"INSERT INTO {table_name} (x, y) VALUES ('{val_x_list[line]}', '{val_y_list[line]}');"
        add_query_list.append(add_query)

    cur.execute(clean_query)
    cur.execute(create_query)
    
    start_time = time.perf_counter_ns()
    for line in range(lines):
        cur.execute(add_query_list[line])           
    end_time = time.perf_counter_ns()

    for trial in range(trials):
        print("try: {}".format(trial))        
        result[0][trial] += (end_time - start_time)
        
        i = 1
        for query in query_list:
            if query == "init":
                continue;
            ex_query = f"SELECT {query} FROM {table_name};"
            print(ex_query)
            start_time = time.perf_counter_ns()
            cur.execute(ex_query)
            rows = cur.fetchall()
            end_time = time.perf_counter_ns()
            result[i][trial] += (end_time - start_time)
            
            i += 1
            
    for trial in range(trials):
        if trial == 0:
            for query in query_list:
                f.write("{} ".format(query))
            f.write("\n")
            continue
        for i in range(len(query_list)):
            f.write("{} ".format(result[i][trial] / 1000))
        f.write("\n")

conn.commit()
cur.close()
conn.close()

f.close()