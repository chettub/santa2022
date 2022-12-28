import streamlit as st
import copy
from io import StringIO
from pathlib import Path
import numpy as np
import pandas as pd
from bokeh.plotting import figure, show
from bokeh.models import LinearColorMapper, ColumnDataSource, MultiLine, ColorBar
import colorcet

# st.set_page_config(layout="wide")

def cartesian_to_array(x, y, shape):
    m, n = shape[:2]
    i = (n - 1) // 2 - y
    j = (n - 1) // 2 + x
    if i < 0 or i >= m or j < 0 or j >= n:
        raise ValueError("Coordinates not within given dimensions.")
    return i, j


def array_to_cartesian(i, j, shape):
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
    
# load image
df_image = pd.read_csv(Path(__file__).resolve().parent.parent / "input/image.csv")
image = df_to_image(df_image)
image_orig = copy.deepcopy(image)
image = (255*image).astype(np.uint8)

def calc_costs(path, image=image_orig):
    conf = 0
    color = 0
    ppos = [cartesian_to_array(*p, image.shape) for p in path]
    for p1, p2 in zip(ppos[:-1], ppos[1:]):
        conf += np.sqrt(abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]))
        color += 3.0 * np.abs(image[p1] - image[p2]).sum()
    return {"total": conf+color, "reconf": conf, "color": color, "len": len(path)}

img_plt = np.empty((257, 257), dtype=np.uint32)
view = img_plt.view(dtype=np.uint8).reshape((257, 257, 4))
view[:, :, 0:3] = np.flipud(image[:, :, 0:3])#上下反転あり
view[:, :, 3] = 255

alpha = st.sidebar.slider("image alpha", 0., 1., 0.5, step=0.1)
figsize = st.sidebar.slider("image size", 100, 1600, 600, step=100)

TOOLS = "pan,hover,reset,wheel_zoom,box_zoom,crosshair"

TOOLTIPS = [
    ("index", "@index"),
    ("(x, y)", "(@xpos, @ypos)")
]

p = figure(x_range=(-130, 130), y_range=(-130, 130), width=figsize, height=figsize, tools=TOOLS, tooltips=TOOLTIPS)
p.image_rgba(image=[img_plt], x=-128.5, y=-128.5, dw=257, dh=257, alpha=alpha)

# get paths from bestSolution.txt (output file of GA-EAX)
uploaded_file = st.sidebar.file_uploader("Choose route file (bestSolution.txt)")
is_color = st.sidebar.checkbox("Plot colored line")

if uploaded_file is not None:
    bytes_data = uploaded_file.getvalue()
    stringio = StringIO(uploaded_file.getvalue().decode("utf-8"))
    string_data = stringio.read()
    lines = string_data.split("\n")
    lengths = []
    scores = []
    paths = []
    for i in range(0, len(lines)-1, 2):
        length, score = lines[i].split(" ")
        path = lines[i+1].split(" ")
        lengths.append(int(length))
        scores.append(int(score) / 1000000)
        paths.append([int(p) for p in path[:int(length)]])


    options = pd.DataFrame({"score": [f"{i}: {s}" for i, s in enumerate(scores)]})

    chosen = st.sidebar.selectbox("Select path to show", options, len(options)-1)
    idx, score = chosen.split(":")
    idx = int(idx)
    path = paths[idx]

    pos = []
    for r in df_image.itertuples():
        pos.append((r.x, r.y))
    path = [pos[r-1] for r in path]
    for i in range(len(path)):
        if path[i] == (0, 0):
            start = i
    path = path[start:] + path[:start]
    path = [cartesian_to_array(*p, image.shape) for p in path]
    path = [(p[0]-128, p[1]-128, i) for i, p in enumerate(path)]    

    st.text("best theoretical score with optimal arm configs:")
    st.text(calc_costs([p[:2] for p in path] + [(0, 0)]))
    
    path_range = st.sidebar.slider("Index range to plot", 0, len(path), (0, len(path)))

    pathnp = np.array(path)
    pathnp = pathnp[path_range[0]:path_range[1]]

    if not is_color:
        s = ColumnDataSource({"xpos": pathnp[:, 0], "ypos": pathnp[:, 1], "index": pathnp[:, 2]})
        p.line("xpos", "ypos", source=s)
        p.scatter("xpos", "ypos", source=s, size=1.5)
    else:
        s = ColumnDataSource({"xposs": [pathnp[i:i+2, 0] for i in range(pathnp.shape[0]-1)], "yposs": [pathnp[i:i+2, 1] for i in range(pathnp.shape[0]-1)], "color": pathnp[:, 2] / pathnp.shape[0], "index": pathnp[:, 2],
                                "xpos": pathnp[:, 0], "ypos": pathnp[:,1]})
        color_mapper = LinearColorMapper(palette=colorcet.rainbow4, low=np.min(pathnp[:, 2]), high=np.max(pathnp[:, 2]))
        glyph = MultiLine(xs="xposs", ys="yposs", line_color={"field": "index", "transform": color_mapper}, line_width=1)
        cb = ColorBar(color_mapper=color_mapper)
        p.add_glyph(s, glyph)
        p.add_layout(cb, "right")
    
    st.sidebar.download_button("Download this solution", f"""{lengths[idx]} {int(scores[idx]*1000000)}\n{' '.join([str(p) for p in paths[idx]])}""", file_name=f"{idx}_{score}.txt")


st.bokeh_chart(p, use_container_width=False)
