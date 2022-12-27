def cartesian_to_array(x, y):
    m, n = 257, 257
    i = (n - 1) // 2 - y
    j = (n - 1) // 2 + x
    if i < 0 or i >= m or j < 0 or j >= n:
        raise ValueError("Coordinates not within given dimensions.")
    return i, j


filepath = "./solution/solution3.eax"
output1 = "./solution/input-1.eax"
output2 = "./solution/input-2.eax"

with open(filepath, "r") as f:
    lines = f.readlines()
res_idx = list(map(int, lines[1].strip().split(" ")))

with open("../input/image.csv", "r") as f:
    lines = f.readlines()
pos = []
for l in lines[1:]:
    vals = l.split(",")
    pos.append((int(vals[0]), int(vals[1])))

path = [pos[i-1] for i in res_idx]

for i in range(len(path)):
    if path[i] == (0, 0):
        start = i
        print("center:", i)
path = path[start:] + path[:start]

path = [cartesian_to_array(*p) for p in path]
path = [(p[0]-128, p[1]-128) for p in path]
path += [(0, 0)]

corners = []
for i in range(len(path)):
    if abs(path[i][0]) == 128 and abs(path[i][1]) == 128:
        corners.append(i)
        print(i, path[i])

path1 = path[:corners[0]]
path2 = path[corners[0]:][::-1]

# initial config
cf = [(64, 0), (-32, 0), (-16, 0), (-8, 0), (-4, 0), (-2, 0), (-1, 0), (-1, 0)]

with open(output1, "w") as f:
    f.write(f"{len(path1)}\n")
    
    for c in cf:
        f.write(f"{c[0]} {c[1]} ")
    f.write("\n")
    
    for p in path1:
        f.write(f"{p[0]} {p[1]}\n")

with open(output2, "w") as f:
    f.write(f"{len(path2)}\n")
    
    for c in cf:
        f.write(f"{c[0]} {c[1]} ")
    f.write("\n")
    
    for p in path2:
        f.write(f"{p[0]} {p[1]}\n")
    