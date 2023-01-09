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
# is_color = st.sidebar.checkbox("Plot colored line")
color_type = st.sidebar.selectbox("Plot color type", ["simple", "progress", "distance"])

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

    is_comparison = st.sidebar.checkbox("Compare two paths")
    if not is_comparison:

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

        if color_type == "simple":
            s = ColumnDataSource({"xpos": pathnp[:, 0], "ypos": pathnp[:, 1], "index": pathnp[:, 2]})
            p.line("xpos", "ypos", source=s)
            p.scatter("xpos", "ypos", source=s, size=1.5)
        elif color_type == "progress":
            s = ColumnDataSource({"xposs": [pathnp[i:i+2, 0] for i in range(pathnp.shape[0]-1)], "yposs": [pathnp[i:i+2, 1] for i in range(pathnp.shape[0]-1)], "color": pathnp[:, 2] / pathnp.shape[0], "index": pathnp[:, 2],
                                    "xpos": pathnp[:, 0], "ypos": pathnp[:,1]})
            color_mapper = LinearColorMapper(palette=colorcet.rainbow4, low=np.min(pathnp[:, 2]), high=np.max(pathnp[:, 2]))
            glyph = MultiLine(xs="xposs", ys="yposs", line_color={"field": "index", "transform": color_mapper}, line_width=1)
            cb = ColorBar(color_mapper=color_mapper)
            p.add_glyph(s, glyph)
            p.add_layout(cb, "right")
        elif color_type == "distance":
            min_dist = st.sidebar.slider("min_dist", min_value=1, max_value=16, value=2)
            dists = np.array([np.abs(pathnp[i+1,0]-pathnp[i,0])+np.abs(pathnp[i+1,1]-pathnp[i,1]) for i in range(pathnp.shape[0]-1)])
            s = ColumnDataSource({"xposs": [pathnp[i:i+2, 0] for i in range(pathnp.shape[0]-1) if dists[i] >= min_dist], "yposs": [pathnp[i:i+2, 1] for i in range(pathnp.shape[0]-1) if dists[i] >= min_dist], "color": [dists[i] for i in range(len(dists)) if dists[i] >= min_dist], "index": pathnp[:-1, 2][dists >= min_dist],
                                    "xpos": pathnp[:-1, 0][dists >= min_dist], "ypos": pathnp[:-1,1][dists >= min_dist]})
            color_mapper = LinearColorMapper(palette=colorcet.rainbow4, low=1, high=10)
            glyph = MultiLine(xs="xposs", ys="yposs", line_color={"field": "color", "transform": color_mapper}, line_width=1)
            cb = ColorBar(color_mapper=color_mapper)
            p.add_glyph(s, glyph)
            p.add_layout(cb, "right")

        
        st.sidebar.download_button("Download this solution", f"""{lengths[idx]} {int(scores[idx]*1000000)}\n{' '.join([str(p) for p in paths[idx]])}""", file_name=f"{idx}_{float(score)}.txt")
    else: # comparison mode
        ignore_direction = st.sidebar.checkbox("ignore direction in path comparison")
        options = pd.DataFrame({"score": [f"{i}: {s}" for i, s in enumerate(scores)]})

        chosen1 = st.sidebar.selectbox("Select path to compare 1", options, 0)
        chosen2 = st.sidebar.selectbox("Select path to compare 2", options, 1)
        idx1, score1 = chosen1.split(":")
        idx1 = int(idx1)
        path1 = paths[idx1]
        idx2, score2 = chosen2.split(":")
        idx2 = int(idx2)
        path2 = paths[idx2]

        pos = []
        for r in df_image.itertuples():
            pos.append((r.x, r.y))
        path1 = [pos[r-1] for r in path1]
        path2 = [pos[r-1] for r in path2]

        for i in range(len(path1)):
            if path1[i] == (0, 0):
                start1 = i
        path1 = path1[start1:] + path1[:start1]
        path1 = [cartesian_to_array(*p, image.shape) for p in path1]
        path1 = [(p[0]-128, p[1]-128, i) for i, p in enumerate(path1)]    
        path1 = np.array(path1)

        for i in range(len(path2)):
            if path2[i] == (0, 0):
                start2 = i
        path2 = path2[start2:] + path2[:start2]
        path2 = [cartesian_to_array(*p, image.shape) for p in path2]
        path2 = [(p[0]-128, p[1]-128, i) for i, p in enumerate(path2)]    
        path2 = np.array(path2)

        st.text("best theoretical score with optimal arm configs:")
        st.text(calc_costs([p[:2] for p in path1] + [(0, 0)]))
        st.text(calc_costs([p[:2] for p in path2] + [(0, 0)]))

        path1edge = []
        path2edge = []
        set_path1edge = set()
        set_path2edge = set()

        def to_tuple(a):
            try:
                return tuple(to_tuple(i) for i in a)
            except:
                return a

        for i in range(len(path1) -1):
            path1edge.append(path1[i:i+2])
            path2edge.append(path2[i:i+2])
            set_path1edge.add(to_tuple(path1[i:i+2, 0:2]))
            set_path2edge.add(to_tuple(path2[i:i+2, 0:2]))
            if ignore_direction:
                set_path1edge.add(to_tuple(path1[i+1:i-1:-1, 0:2]))
                set_path2edge.add(to_tuple(path2[i+1:i-1:-1, 0:2]))

        path1only = []
        path2only = []

        for i in range(len(path1) - 1):
            if to_tuple(path1[i:i+2, 0:2]) not in set_path2edge:
                path1only.append(path1[i:i+2])
            if to_tuple(path2[i:i+2, 0:2]) not in set_path1edge:
                path2only.append(path2[i:i+2])

        vals = st.sidebar.multiselect("Path to plot", ["1", "2", "only1", "only2"], default=["only1", "only2"])

        if "1" in vals:
            s1 = ColumnDataSource({"xpos": [p[:, 0] for p in path1edge], "ypos": [p[:, 1] for p in path1edge], "index": [p[:, 2] for p in path1edge]})
            glyph1 = MultiLine(xs="xpos", ys="ypos", line_color="blue", line_width=1, line_alpha=0.3, )
            p.add_glyph(s1, glyph1)
        if "2" in vals:
            s2 = ColumnDataSource({"xpos": [p[:, 0] for p in path2edge], "ypos": [p[:, 1] for p in path2edge], "index": [p[:, 2] for p in path2edge]})
            glyph2 = MultiLine(xs="xpos", ys="ypos", line_color="red", line_width=1, line_alpha=0.3)
            p.add_glyph(s2, glyph2)
        if "only1" in vals:
            s3 = ColumnDataSource({"xpos": [p[:, 0] for p in path1only], "ypos": [p[:, 1] for p in path1only], "index": [p[:, 2] for p in path1only]})
            glyph3 = MultiLine(xs="xpos", ys="ypos", line_color="blue", line_width=2, line_alpha=0.8)
            p.add_glyph(s3, glyph3)
        if "only2" in vals:
            s4 = ColumnDataSource({"xpos": [p[:, 0] for p in path2only], "ypos": [p[:, 1] for p in path2only], "index": [p[:, 2] for p in path2only]})
            glyph4 = MultiLine(xs="xpos", ys="ypos", line_color="red", line_width=2, line_alpha=0.8)
            p.add_glyph(s4, glyph4)

        # st.text(path1only)
        # st.text(path2only)
st.bokeh_chart(p, use_container_width=False)
