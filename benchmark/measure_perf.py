import psycopg2
import random
import time
import subprocess

conn = psycopg2.connect(
    host="localhost",
    database="test",
    user="postgres"
)

lines = 10000000
bound = 100000
trials = 1

table_list = ["test_fxypty", "test_numeric", "test_double", "test_real"]
type_list = ["fxypty(30,10)", "NUMERIC(30,10)", "DOUBLE PRECISION", "REAL"]

cur = conn.cursor()

val_x_list = []
val_y_list = []

query_list = ["init", "*","x+y","x-y","x*y","x/y","SUM(x)","MIN(x)","MAX(x)","COUNT(x)"]
query_name = ["init", "all", "add", "sub", "mul", "div", "sum", "min", "max", "cnt"]

for idx in range(len(table_list)):
# for idx in range(1):
    table_name = table_list[idx]
    val_type = type_list[idx]
    
    print("{}, {}".format(table_name, val_type))

    for trial in range(trials):
        print("try: {}".format(trial))
       
        i = 1
        for query in query_list:
            if query == "init":
                continue;
            ex_query = f"SELECT {query} FROM {table_name};"

            print(conn.get_backend_pid())

            perf_command = f"sudo perf record -a -g"
            process = subprocess.Popen(perf_command, shell=True)
            cur.execute(ex_query)
            process.terminate()

            print("sleep 1 sec first")
            time.sleep(1)

            flame_name = f"flamegraph_{table_name}_{query_name[i]}_{trial}.svg"
            flame_command = f"sudo perf script | ~/FlameGraph/stackcollapse-perf.pl | ~/FlameGraph/flamegraph.pl > {flame_name}"
            subprocess.run(flame_command, shell=True, check=True)
            
            perf_command = f"sudo perf record -a -g"
            process = subprocess.Popen(perf_command, shell=True)
            rows = cur.fetchall()
            process.terminate()            

            print("sleep 1 sec second")
            time.sleep(1)

            flame_name = f"flamegraph_fetchall_{table_name}_{query_name[i]}_{trial}.svg"
            flame_command = f"sudo perf script | ~/FlameGraph/stackcollapse-perf.pl | ~/FlameGraph/flamegraph.pl > {flame_name}"
            subprocess.run(flame_command, shell=True, check=True)
            
            i += 1
            
conn.commit()
cur.close()
conn.close()
