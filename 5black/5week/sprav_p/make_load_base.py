import json
import copy
import random

settings = {
    "serialization_settings": {
        "file": "data_load.bin"
    },
    "routing_settings": {
        "bus_wait_time": 2,
        "bus_velocity": 30
    },
    "render_settings": {
        "width": 1500,
        "height": 950,
        "padding": 50,
        "outer_margin": 150,
        "stop_radius": 3,
        "line_width": 10,
        "bus_label_font_size": 18,
        "bus_label_offset": [
            7,
            15
        ],
        "stop_label_font_size": 13,
        "stop_label_offset": [
            7,
            -3
        ],
        "underlayer_color": [
            255,
            255,
            255,
            0.85
        ],
        "underlayer_width": 3,
        "color_palette": [
            "red",
            "green",
            "blue",
            "brown",
            "orange"
        ],
        "layers": [
            "bus_lines",
            "bus_labels",
            "stop_points",
            "stop_labels"
        ]
    }
}

base_stop_request = {
    "type": "Stop",
    "name": None,
    "latitude": None,
    "longitude": None,
    "road_distances": {
    }
}

base_bus_request = {}

base_stops = []
for n in range(1, 450):
    stop = copy.deepcopy(base_stop_request)
    stop["name"] = "stop" + str(n)
    stop["latitude"] = random.uniform(10,70)
    stop["longitude"] = random.uniform(10,70)
    base_stops.append(stop)

make = copy.deepcopy(settings)
make["base_requests"] = []
make["base_requests"].extend(base_stops)
f = open("load_make_base.in.json", "w")
f.write(json.dumps(make))
base = copy.deepcopy(settings)
f.close()

process = copy.deepcopy(settings)
process["stat_requests"] = []
f = open("load_process_requests.in.json", "w")
f.write(json.dumps(process))
f.close()
