import random
import time

def generate_random_number(length):
    first_digit = str(random.randint(1,9))
    random_number = ''.join(str(random.randint(0,9)) for _ in range(length))
    return first_digit + random_number

lines = 10**4
scale = 8
predicate = 17
integerpart = predicate - scale

for precision in range(38):
    with open(f"data_{precision}.in", "w") as f:
        f.write(f"{lines}\n")
        for line in range(lines):
            x_val = generate_random_number(precision)
            y_val = generate_random_number(precision)

            x_hi = x_val[18:37]
            if x_hi == "":
                x_hi = "0"

            y_hi = y_val[18:37]
            if y_hi == "":
                y_hi = "0"

            x_lo = x_val[0:18]
            y_lo = y_val[0:18]

            f.write(f"{x_hi} {x_lo} {y_hi} {y_lo}\n")
        f.close()