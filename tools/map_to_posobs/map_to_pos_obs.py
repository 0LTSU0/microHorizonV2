import socket
import argparse
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True
hp_ip = "127.0.0.1"
hp_port = 12364
sock = None

def send_pos_to_socket(d):
    posstr = ""
    posstr = str(d["lat"]) + ","
    posstr += str(d["lon"]) + ","
    posstr += str(13.9) + "," # some default speed of ~50kmh (TODO: implement way to set something in web gui)
    posstr += str(d["heading"])
    sock.sendto(bytes(posstr, "utf-8"), (hp_ip, hp_port))

@app.route("/")
def mainpage():
    return render_template("main.html", micro_horizon_ip=hp_ip, micro_horizon_port=hp_port)

@app.route("/postPosObs", methods=["POST"])
def postPosObs():
    data = request.get_json()
    if not data:
        return jsonify({'error': 'No JSON received'}), 400
    send_pos_to_socket(data)
    return jsonify({'status': 'success'}), 200

if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument("--port", type=int, default=12364)
    argparser.add_argument("--host", type=str, default="127.0.0.1")
    args = argparser.parse_args()
    hp_ip = args.host
    hp_port = args.port
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    app.run()