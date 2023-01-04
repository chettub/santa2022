from decimal import Decimal, getcontext
import argparse
import sys
import io
# from time import time, sleep
from pathlib import Path
# import multiprocessing
# from threading import Thread
# from subprocess import Popen, PIPE
import subprocess
from functools import *
from itertools import *

import numpy as np
import pandas as pd
from copy import copy

getcontext().prec = 40  # 
# getcontext().prec = 100

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

class Cost:
    def __init__(self):
        self.cnt1 = Decimal(0)
        self.cnt2 = Decimal(0)
        self.cnt3 = Decimal(0)
        self.cnt5 = Decimal(0)
        self.cnt6 = Decimal(0)
        self.cnt7 = Decimal(0)

    def add_reconf(self, man: int):
        if man == 1:
            self.cnt1 += 1
        elif man == 2:
            self.cnt2 += 1
        elif man == 3:
            self.cnt3 += 1
        elif man == 4:
            self.cnt1 += 2
        elif man == 5:
            self.cnt5 += 1
        elif man == 6:
            self.cnt6 += 1
        elif man == 7:
            self.cnt7 += 1
        elif man == 8:
            self.cnt2 += 2
        else: # man == 0
            pass

    def add_color(self, c: Decimal):
        self.cnt1 += c
    
    def __str__(self):
        res = str(self.cnt1)
        res += " + "
        res += str(self.cnt2) + "*sqrt(2)"
        res += " + "
        res += str(self.cnt3) + "*sqrt(3)"
        res += " + "
        res += str(self.cnt5) + "*sqrt(5)"
        res += " + "
        res += str(self.cnt6) + "*sqrt(6)"
        res += " + "
        res += str(self.cnt7) + "*sqrt(7)"
        return res
    
    def get_total(self):
        res = self.cnt1
        res += Decimal("2").sqrt() * self.cnt2
        res += Decimal("3").sqrt() * self.cnt3
        res += Decimal("5").sqrt() * self.cnt5
        res += Decimal("6").sqrt() * self.cnt6
        res += Decimal("7").sqrt() * self.cnt7
        return res

def calc_costs(path, image):
    cost = Cost()

    ppos = [cartesian_to_array(*p, image.shape) for p in path]
    for p1, p2 in zip(ppos[:-1], ppos[1:]):
        cost.add_reconf(abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]))
        color_diff = image[p1] - image[p2]
        color_diff = color_diff[0].copy_abs() + color_diff[1].copy_abs() + color_diff[2].copy_abs()
        cost.add_color(Decimal(3) * color_diff)
    return cost


def get_args():
    parser = argparse.ArgumentParser(description="Run exact score calculation for paths genetated by GA-EAX")
    parser.add_argument("-f", "--input_file")
    args = parser.parse_args()
    return args

if __name__ == "__main__":
    image_file = Path(__file__).resolve().parent.parent / "input/image.csv"
    with open(image_file, "r") as f:
        lines = f.readlines()
    pos = []
    for l in lines[1:]:
        vals = l.split(",")
        pos.append((int(vals[0]), int(vals[1])))

    image = []
    for l in lines[1:]:
        vals = l.split(",")
        image.append([Decimal(vals[2]), Decimal(vals[3]), Decimal(vals[4])])
    image = np.array(image).reshape(257, 257, -1)
    print(image[:5, :5])
    _args = get_args()

    input_file = _args.input_file

    with open(input_file, "r") as f:
        lines = f.readlines()
    n_paths = len(lines) // 2

    print(f"Paths contained in the file: {n_paths}")

    for i_path in range(n_paths):
        length, score = lines[2*i_path].strip().split(" ")
        length = int(length)
        score = int(score)

        print(f"idx: {i_path}, length: {length}, score(in GA-EAX): {score}")

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
        

        cost = calc_costs(path, image)

        # getcontext().prec = 20  # 

        print("score:", cost.get_total(), "=", cost)
        if i_path == 10:
            break
