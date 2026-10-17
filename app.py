import os
import requests
from flask import Flask, render_template, jsonify

# Define absolute paths for local environment
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
JSON_PATH = os.path.join(BASE_DIR, 'dynamic.json')
TEMPLATE_DIR = os.path.join(BASE_DIR, "templates")
STATIC_DIR = os.path.join(BASE_DIR, "static")
STATIC_JSON_PATH = os.path.join(STATIC_DIR, "json", "main.json")

# ESP32 Access Point IP address
ESP32_URL = "http://192.168.4.1"

app = Flask(__name__, template_folder=TEMPLATE_DIR, static_folder=STATIC_DIR)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/data", methods=["GET"])
def get_data():
    try:
        response = requests.get(
            ESP32_URL + "/api/data",
            timeout=3
        )
        response.raise_for_status()
        data = response.json()

        return jsonify({
            "sensor_value": int(data["sensor_value"]),
            "decibels": float(data["decibels"])
        })

    except requests.RequestException as error:
        app.logger.error("ESP32 connection error: %s", error)
        return jsonify({
            "error": "ESP32 is not reachable"
        }), 503

    except (ValueError, KeyError, TypeError):
        return jsonify({
            "error": "Invalid sensor data received"
        }), 502

@app.route("/health", methods=["GET"])
def health():
    return jsonify({
        "status": "running",
        "esp32": ESP32_URL
    })

@app.errorhandler(500)
def internal_server_error(error):
    return jsonify({
        "error": "Internal server error"
    }), 500

if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )
