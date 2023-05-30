import psycopg2
import random
import time

conn = psycopg2.connect(
    host="localhost",
    database="test",
    user="postgres"
)

def generate_random_number(length):
    first_digit = str(random.randint(1,9))
    random_number = ''.join(str(random.randint(0,9)) for _ in range (length - 1))
    return random_number

lines = 10**7
scale = 12
predicate = 38
integerpart = predicate - scale
trials = 12

table_list = ["test_numeric", "test_fxypty", "test_double"] #, "test_real"]
type_list = [f"NUMERIC({predicate},{scale})", f"fxypty({predicate},{scale})", "DOUBLE PRECISION"] # , "REAL"]

cur = conn.cursor()

cur.execute(f"DROP EXTENSION IF EXISTS pgfixeypointy CASCADE;")
cur.execute(f"CREATE EXTENSION pgfixeypointy;")

f = open(f"./output_{lines}_({predicate},{scale}).out", "w")

val_x_list = []
val_y_list = []
# query_list = ["init", "*","x + y","x - y","x * y","x / y","SUM(x)","MIN(x)","MAX(x)","AVG(x)","COUNT(x)","VARIANCE(x)","STDDEV(x)"]

# query_list = ["init", "*","x+y","x-y","x*y","x/y","SUM(x)","MIN(x)","MAX(x)","COUNT(x)","SUM(x),SUM(y)", "MIN(x),MIN(y)", "MAX(x),MAX(y)", "COUNT(x),COUNT(y)"]
query_list = ["init", "*","x+y","x-y","x/y"]

for line in range(lines):
    val_x_list.append(generate_random_number(integerpart) + '.' + generate_random_number(scale))
    val_y_list.append(generate_random_number(integerpart) + '.' + generate_random_number(scale))
    
    if line < 2:
        print(val_x_list[line])

for idx in range(len(table_list)):
# for idx in range(1):
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
            if trial < 6:
                start_time = time.perf_counter_ns()
                cur.execute(ex_query)
                # rows = cur.fetchall()
                end_time = time.perf_counter_ns()
            else:
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