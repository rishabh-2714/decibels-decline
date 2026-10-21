import os
import requests
import json
from flask import Flask, render_template, jsonify
from datetime import datetime
# Define absolute paths for local environment
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
JSON_PATH = os.path.join(BASE_DIR, 'dynamic.json')
TEMPLATE_DIR = os.path.join(BASE_DIR, "templates")
STATIC_DIR = os.path.join(BASE_DIR, "static")
STATIC_JSON_PATH = os.path.join(STATIC_DIR, "json", "main.json")

# ESP32 Access Point IP address
ESP32_URL = "http://192.168.4.1"

app = Flask(__name__, template_folder=TEMPLATE_DIR, static_folder=STATIC_DIR)

def read_json():
    if not os.path.exists(JSON_PATH):
        return {
            "sensor_value": 0,
            "decibels": 0.0,
            "update_count": 0
        }

    try:
        with open(JSON_PATH, "r", encoding="utf-8") as file:
            data = json.load(file)

        if not isinstance(data, dict):
            raise ValueError("Invalid JSON structure")

        return data

    except (OSError, ValueError):
        app.logger.warning("Could not read dynamic.json")
        return {
            "sensor_value": 0,
            "decibels": 0.0,
            "update_count": 0
        }

def write_json(data):
    try:
        previous_data = read_json()
        count = int(previous_data.get("update_count", 0))

        # Update only if the sensor readings have changed
        readings_changed = (
            data["sensor_value"] != previous_data.get("sensor_value")
            or round(data["decibels"], 1)
            != round(float(previous_data.get("decibels", 0)), 1)
        )

        if readings_changed:
            data["update_count"] = count + 1
            data["updated_at"] = datetime.now().isoformat(
                timespec="seconds"
            )

            temp_path = JSON_PATH + ".tmp"

            with open(temp_path, "w", encoding="utf-8") as file:
                json.dump(data, file, indent=4)

            # Replace the old file after a successful write
            os.replace(temp_path, JSON_PATH)

        else:
            data["update_count"] = count

        return True

    except (OSError, ValueError, TypeError) as error:
        app.logger.error("Error updating dynamic.json: %s", error)
        return False
        
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

        esp_data = response.json()

        data = {
            "sensor_value": int(esp_data["sensor_value"]),
            "decibels": float(esp_data["decibels"])
        }

        if not write_json(data):
            return jsonify({
                "error": "Unable to update dynamic.json"
            }), 500

        
        saved_data = read_json()

        return jsonify({
            "sensor_value": data["sensor_value"],
            "decibels": data["decibels"],
            "update_count": saved_data.get("update_count", 0),
            "updated_at": saved_data.get("updated_at")
        })

    except requests.RequestException as error:
        app.logger.error("ESP8266 connection error: %s", error)
        return jsonify({
            "error": "ESP8266 is not reachable"
        }), 503

    except (ValueError, KeyError, TypeError):
        return jsonify({
            "error": "Invalid sensor data received"
        }), 502

@app.route("/api/counter", methods=["GET"])
def get_counter():
    data = read_json()

    return jsonify({
        "update_count": data.get("update_count", 0),
        "updated_at": data.get("updated_at")
    })
    
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
@app.errorhandler(404)
def page_not_found(error):
    return jsonify({
        "error": "Page not found"
    }), 404
    
if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )
