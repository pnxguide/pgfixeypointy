# for i in range(100):
#     print('"', end="")
#     if i < 10:
#         print("0", end="")
#     print(f"{i}", end="")
#     print('"', end=",")

for i in range(1000):
    print('"', end="")
    if i < 10:
        print("0", end="")
    if i < 100:
        print("0", end="")
    print(f"{i}", end="")
    print('"', end=",")