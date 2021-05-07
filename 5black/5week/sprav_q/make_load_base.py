import json
import copy
import random
import math

max_stops = 450
max_buses = 400
max_adjacent = 200
max_requests = 50

bus_stops_mean = 10
bus_stops_sigma = 5
bus_stops_hard_max = max_stops

settings_make = {
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

settings_process = {
    "serialization_settings": {
        "file": "data_load.bin"
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

stat_stop_request = {
    "id": 1360715858,
    "type": "Stop",
    "name": None
}

base_bus_request = {
    "type": "Bus",
    "name": None,
    "stops": [],
    "is_roundtrip": False
}

stat_bus_request = {
    "id": 517161241,
    "type": "Bus",
    "name": None
}

stat_route_request = {
    "id": 1231848878,
    "type": "Route",
    "from": None,
    "to": None
}



r = random.Random()

stops = {}
for n in range(0, max_stops):
    stops["stop" + str(n)] = {
        "lat": r.uniform(30, 31),
        "lon": r.uniform(30, 31),
        "dists": {}
    }


def get_adj_rank(s):
    return len(stops[s]["dists"])


def is_adj(s_from, s_to):
    return s_to in stops[s_from]["dists"]


def set_adj(s_from, s_to):
    if is_adj(s_from, s_to):
        return

    dist_0 = math.sqrt(
        (stops[s_from]["lat"] - stops[s_to]["lat"])**2 +
        (stops[s_from]["lon"] - stops[s_to]["lon"])**2) * 1110000
    dist_low = dist_0 * 1.3
    dist_high = dist_0 * 1.5
    stops[s_from]["dists"][s_to] = int(r.uniform(dist_low, dist_high))


def check_adj_rank(s_from, s_to, two_way):
    if s_from is None or s_to is None:
        return False

    result = False
    if not is_adj(s_from, s_to):
        result = result or get_adj_rank(s_from) >= max_adjacent
    if two_way and not is_adj(s_to, s_from):
        result = result or get_adj_rank(s_to) >= max_adjacent
    return result


def get_next_stop_for_route(s_from, two_way):
    s_to = None
    count = 0
    while True:
        s_to = "stop" + str(r.randrange(0, max_stops))
        count += 1
        if not check_adj_rank(s_from, s_to, two_way):
            break
        if count > 10000:
            raise "Failed to get stop for route"

    return s_to


base_buses = []
for n in range(0, max_buses):
    bus = copy.deepcopy(base_bus_request)
    bus["name"] = str(n)

    s_cnt = r.gauss(bus_stops_mean, bus_stops_sigma)
    s_cnt = int(min(max(0, s_cnt), bus_stops_hard_max))
    prev = None
    for k in range(s_cnt):
        s = get_next_stop_for_route(prev, True)
        bus["stops"].append(s)
        if prev is not None:
            set_adj(prev, s)
            set_adj(s, prev)
        prev = s

    base_buses.append(bus)


base_stops = []
for n, d in stops.items():
    stop = copy.deepcopy(base_stop_request)
    stop["name"] = n
    stop["latitude"] = d["lat"]
    stop["longitude"] = d["lon"]
    stop["road_distances"] = copy.deepcopy(d["dists"])
    base_stops.append(stop)


make = copy.deepcopy(settings_make)
make["stat_requests"] =  [
    {
        "id": 1639812454,
        "type": "Map"
    }
]
make["base_requests"] = []
make["base_requests"].extend(base_stops)
make["base_requests"].extend(base_buses)
f = open("load_make_base.in.json", "w")
f.write(json.dumps(make))
f.close()

stat_stops = []
for n in range(0, max_requests):
    stop = copy.deepcopy(stat_stop_request)
    stop["id"] = r.randrange(1000000000)
    stop["name"] = "stop" + str(r.randrange(0, max_stops))
    stat_stops.append(stop)

process = copy.deepcopy(settings_process)
process["stat_requests"] = stat_stops
f = open("load_process_requests_stops.in.json", "w")
f.write(json.dumps(process))
f.close()

stat_buses = []
for n in range(0, max_requests):
    bus = copy.deepcopy(stat_bus_request)
    bus["id"] = r.randrange(1000000000)
    bus["name"] = str(r.randrange(0, max_buses))
    stat_buses.append(bus)

process = copy.deepcopy(settings_process)
process["stat_requests"] = stat_buses
f = open("load_process_requests_buses.in.json", "w")
f.write(json.dumps(process))
f.close()

stat_routes = []
for n in range(0, max_requests):
    route = copy.deepcopy(stat_route_request)
    route["id"] = r.randrange(1000000000)
    route["from"] = "stop" + str(r.randrange(0, max_stops))
    route["to"] = "stop" + str(r.randrange(0, max_stops))
    stat_routes.append(route)

process = copy.deepcopy(settings_process)
process["stat_requests"] = stat_routes
f = open("load_process_requests_routes.in.json", "w")
f.write(json.dumps(process))
f.close()
