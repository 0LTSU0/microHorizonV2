import socket
import argparse
import plistlib
import math
import time
import sys

SEND_FREQ = 1

def read_file(file):
    with open(file, "rb") as plist_file:
        plist_data = plistlib.load(plist_file)
    return plist_data

def send_positions(positions, ip, port, mode, loop=False):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    prevPos = None
    for pos in reversed(positions): 
        posstr = str(pos["gpsLatitude"]) + ","
        posstr += str(pos["gpsLongitude"]) + ","
        posstr += str(pos["gpsSpeed"]) + ","
        if prevPos:
            posstr += str(get_heading_estimate(prevPos, pos)) #TODO: would be better to calculate from current to next
        else:
            posstr += "0"
        prevPos = pos
        print("sending position", posstr)
        sock.sendto(bytes(posstr, "utf-8"), (ip, port))
        if mode == "auto":
            time.sleep(SEND_FREQ)
        elif mode == "manual":
            input("Press enter to send next position")
    if loop:
        send_positions(positions, ip, port, mode, loop)

def get_heading_estimate(ppos, cpos):
    lat1 = math.radians(ppos["gpsLatitude"])
    lon1 = math.radians(ppos["gpsLongitude"])
    lat2 = math.radians(cpos["gpsLatitude"])
    lon2 = math.radians(cpos["gpsLongitude"])
    delta_lon = lon2 - lon1
    x = math.sin(delta_lon) * math.cos(lat2)
    y = math.cos(lat1) * math.sin(lat2) - math.sin(lat1) * math.cos(lat2) * math.cos(delta_lon)
    initial_bearing = math.atan2(x, y)
    initial_bearing = math.degrees(initial_bearing)
    compass_bearing = (initial_bearing + 360) % 360
    return compass_bearing


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument("--port", type=int)
    argparser.add_argument("--host", type=str)
    argparser.add_argument("--file", type=str)
    argparser.add_argument("--mode", type=str, default="auto")
    argparser.add_argument("--infinite", action="store_true")
    args = argparser.parse_args()
    if args.mode != "auto" and args.mode != "manual":
        sys.exit()
    posdata = read_file(args.file)
    send_positions(posdata.get("gpsRecords", []), args.host, args.port, args.mode, args.infinite)
