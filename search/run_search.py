import argparse
import pathlib
import sys
import io
from time import time, sleep
from pathlib import Path
import multiprocessing
from threading import Thread
from subprocess import Popen, PIPE
import subprocess
from functools import *
from itertools import *

import numpy as np
import pandas as pd
from copy import copy


# Functions to map between cartesian coordinates and array indexes
def cartesian_to_array(x, y, shape=(257,257)):
    m, n = shape[:2]
    i = (n - 1) // 2 - y
    j = (n - 1) // 2 + x
    if i < 0 or i >= m or j < 0 or j >= n:
        raise ValueError("Coordinates not within given dimensions.")
    return i, j


def array_to_cartesian(i, j, shape=(257,257)):
    m, n = shape[:2]
    if i < 0 or i >= m or j < 0 or j >= n:
        raise ValueError("Coordinates not within given dimensions.")
    y = (n - 1) // 2 - i
    x = j - (n - 1) // 2
    return x, y

def get_position(config):
    return reduce(lambda p, q: (p[0] + q[0], p[1] + q[1]), config, (0, 0))

def df_to_image(df):
    side = int(len(df) ** 0.5)  # assumes a square image
    return df_image.set_index(['x', 'y']).to_numpy().reshape(side, side, -1)

def calc_costs(path, image):
    conf = 0
    color = 0
    ppos = [cartesian_to_array(*p, image.shape) for p in path]
    for p1, p2 in zip(ppos[:-1], ppos[1:]):
        conf += np.sqrt(abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]))
        color += 3.0 * np.abs(image[p1] - image[p2]).sum()
    return {"total": conf+color, "reconf": conf, "color": color, "len": len(path)}

def calc_costs_config(config, image):
    conf = 0
    color = 0
    path = [get_position(c) for c in config]
    ppos = [cartesian_to_array(*p, image.shape) for p in path]
    for c1, c2, p1, p2 in zip(config[:-1], config[1:], ppos[:-1], ppos[1:]):
        reconf = 0
        for i in range(8):
            reconf += abs(c1[i][0] - c2[i][0]) + abs(c1[i][1] - c2[i][1])
        conf += np.sqrt(reconf)
        color += 3.0 * np.abs(image[p1] - image[p2]).sum()
    return {"total": conf+color, "reconf": conf, "color": color, "len": len(path)}

def get_args():
    parser = argparse.ArgumentParser(description="Run beam search to all paths in bestSolution.txt genetated by GA-EAX")
    parser.add_argument("-f", "--input_file")
    parser.add_argument("-n", "--n_thread", default=1)
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    # n_cpu = multiprocessing.cpu_count()
    # print(f"cpu count: {n_cpu}")
    _args = get_args()
    image_file = Path(__file__).resolve().parent.parent / "input/image.csv"
    # input_file = Path(args["input_file"])
    input_file = _args.input_file
    output_folder = Path("./found_configs/")
    output_folder.mkdir(exist_ok=True)
    temp_input1 = output_folder / "temp_input1.txt"
    temp_input2 = output_folder / "temp_input2.txt"
    temp_config1 = output_folder / "temp_config1.txt"
    temp_config2 = output_folder / "temp_config2.txt"

    n_thread = _args.n_thread

    print("input file:", input_file)

    df_image = pd.read_csv(image_file)
    image = df_to_image(df_image)

    with open(image_file, "r") as f:
        lines = f.readlines()
    pos = []
    for l in lines[1:]:
        vals = l.split(",")
        pos.append((int(vals[0]), int(vals[1])))

    with open(input_file, "r") as f:
        lines = f.readlines()
    n_paths = len(lines) // 2

    print(f"Paths contained in the file: {n_paths}")

    for i_path in range(n_paths):
        length, score = lines[2*i_path].strip().split(" ")
        length = int(length)
        score = int(score) / 1000000

        print(f"idx: {i_path}, length: {length}, score: {score}")

        res_idx = list(map(int, lines[2*i_path+1].strip().split(" ")))
        path = [pos[i-1] for i in res_idx]

        # find start point: (0, 0)
        for i in range(len(path)):
            if path[i] == (0, 0):
                start = i
        path = path[start:] + path[:start]

        path = [cartesian_to_array(*p) for p in path]
        path = [(p[0]-128, p[1]-128) for p in path]
        path += [(0, 0)]

        corners = []
        for i in range(len(path)):
            if abs(path[i][0]) == 128 and abs(path[i][1]) == 128:
                corners.append(i)

        path1 = path[:corners[0]]
        path2 = path[corners[0]:][::-1]

        # initial config
        cf = [(64, 0), (-32, 0), (-16, 0), (-8, 0), (-4, 0), (-2, 0), (-1, 0), (-1, 0)]

        with open(temp_input1, "w") as f1:
            f1.write(f"{len(path1)}\n")
            
            for c in cf:
                f1.write(f"{c[0]} {c[1]} ")
            f1.write("\n")
            
            for p in path1:
                f1.write(f"{p[0]} {p[1]}\n")

        with open(temp_input2, "w") as f2:
            f2.write(f"{len(path2)}\n")
            
            for c in cf:
                f2.write(f"{c[0]} {c[1]} ")
            f2.write("\n")
            
            for p in path2:
                f2.write(f"{p[0]} {p[1]}\n")
        

        print("quick DP check...")
        procs = []
        threads = []
        proc = Popen([f"./check-dp.out  < {temp_input1}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        procs.append(proc)
        proc = Popen([f"./check-dp.out  < {temp_input2}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        procs.append(proc)

        def read_stream(idx_procs, in_file, out_file):
            for line in in_file:
                print(f"[{idx_procs}] {line.strip()}", file=out_file)
        for idx_procs, proc in enumerate(procs):
            threads.append(Thread(target=read_stream, args=(idx_procs, proc.stdout, sys.stdout)))
            threads.append(Thread(target=read_stream, args=(idx_procs, proc.stderr, sys.stderr)))
        for thread in threads:
            thread.start()

        ok = True
        for proc in procs:
            res = proc.wait()
            if res != 0:
                ok = False
        if not ok:
            print("Cannot solve. continue")
            continue

        print("DP check passed. Run beam search")

        procs = []
        threads = []
        proc = Popen([f"./beam-parallel.out {n_thread} < {temp_input1} > {temp_config1}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        procs.append(proc)
        proc = Popen([f"./beam-parallel.out {n_thread} < {temp_input2} > {temp_config2}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        procs.append(proc)

        def read_stream(idx_procs, in_file, out_file):
            for line in in_file:
                print(f"[{idx_procs}] {line.strip()}", file=out_file)
        for idx_procs, proc in enumerate(procs):
            threads.append(Thread(target=read_stream, args=(idx_procs, proc.stdout, sys.stdout)))
            threads.append(Thread(target=read_stream, args=(idx_procs, proc.stderr, sys.stderr)))
        for thread in threads:
            thread.start()
        for proc in procs:
            proc.wait()
        # res = subprocess.run([f"./beam-parallel.out {n_thread} < {temp_input1} > {temp_config1}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        # if res.returncode != 0:
        #     print("Cannot solve. continue")
        #     continue
        # res = subprocess.run([f"./beam-parallel.out {n_thread} < {temp_input2} > {temp_config2}"], shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        # if res.returncode != 0:
        #     print("Cannot solve. continue")
        #     continue

        print("Answers found! write submission file...")
        with open(temp_config1) as f:
            lines = f.readlines()
        conf1 = []
        for line in lines:
            vals = line.split(" ")
            config = [(int(vals[2*i]), int(vals[2*i+1])) for i in range(8)]
            conf1.append(config)
        conf1 = conf1[::-1]
        path1 = [get_position(c) for c in conf1]

        with open(temp_config2, "r") as f:
            lines = f.readlines()
        conf2 = []
        for line in lines:
            vals = line.split(" ")
            config = [(int(vals[2*i]), int(vals[2*i+1])) for i in range(8)]
            conf2.append(config)
        conf2 = conf2[::-1]
        path2 = [get_position(c) for c in conf2]

        conf = conf1 + conf2[::-1]
        path = [get_position(c) for c in conf]
        costs = calc_costs_config(conf, image)
        print(costs)

        cnt = np.zeros(shape=(257, 257), dtype=np.int32)
        for p in path:
        #     idx = cartesian_to_array(*p, image.shape)
            idx = np.array(p) + np.array([128, 128])
            cnt[idx[0], idx[1]] += 1
        for i in range(257):
            for j in range(257):
                if cnt[i, j] != 1:
                    print(i-128, j-128, cnt[i, j])

        def check_conf_dist(conf1, conf2):
            for c1, c2 in zip(conf1, conf2):
                d = abs(c1[0] - c2[0]) + abs(c1[1] - c2[1])
                if d > 1:
                    return False
            return True

        for i, (c1, c2) in enumerate(zip(conf[:-1], conf[1:])):
            if not check_conf_dist(c1, c2):
                print(i, "error! invalid config change:", c1, c2)

        mx = [64, 32, 16, 8, 4, 2, 1, 1]

        for c in conf:
            for j in range(8):
                am = max(abs(c[j][0]), abs(c[j][1]))
                if am != mx[j]:
                    print("error! invalid link place")
        
        def config_to_string(config):
            return ';'.join([' '.join(map(str, vector)) for vector in config])

        submission = pd.Series(
            [config_to_string(config) for config in conf],
            name="configuration",
        )

        submission.to_csv(output_folder / f'submission_{i_path}_{costs["total"]:.5f}.csv', index=False)

    print("all search completed")