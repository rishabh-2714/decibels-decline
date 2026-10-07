# Shaurya, please add Flask here. For mapping, here is the repository structure. Please write full paths so that we can use PythonAnywhere as a host.
"""
Mapping:
/root
│
│——/static
│   │
│   │——/json/static.json
│   │——/css/style.css
│   │——/js/main.js
│——/templates/index.html
│——app.py #this file
│——dynamic.json
│——decibels-mihir.ino
"""
import os
import json
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Define absolute paths for PythonAnywhere hosting environments
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
JSON_PATH = os.path.join(BASE_DIR, 'dynamic.json')
TEMP_DIR=os.path.join(BASE_DIR,"templates")
STATIC_DIR=os.path.join(BASE_DIR,"static")
STATIC_JSON_PATH=os.path.join(STATIC_DIR,"json","static.json")
app = Flask(_name_,template_folder=TEMPLATE_DIR,static_folder=STATIC_DIR)
def read_json():
    if not os.path.exists(JSON_PATH):
        try:
            with open(JSON_PATH, "w", encoding="utf-8") as file:
                json.dump({}, file, indent=4)
        except OSError as error:
            print("Error creating dynamic.json:", error)
            return {}

	try:
            with open(JSON_PATH, "r", encoding="utf-8") as file:
            data = json.load(file)
            return data

    except json.JSONDecodeError:
        print("Error: dynamic.json contains invalid JSON.")
        return {}

    except OSError as error:
        print("Error reading dynamic.json:", error)
        return {}
def write_json(data):
    try:
        with open(JSON_PATH, "w", encoding="utf-8") as file:
            json.dump(
                data,
                file,
                indent=4,
                ensure_ascii=False
            )

        return True

    except OSError as error:
        print("Error writing dynamic.json:", error)
        return False
@app.route("/")
def index():

    return render_template("index.html")

@app.route('/api/data', methods=['GET'])
def get_data():
    # Endpoint for the frontend JS to pull the latest JSON data loop
    return jsonify(read_json())

@app.route('/api/data/update', methods=['POST'])
def update_data():
    # Endpoint for any WiFi-enabled Arduino to send raw payloads
    new_data = request.get_json()
    if not new_data:
        return jsonify({"error": "Malformed or empty JSON payload"}), 400
        
    write_json(new_data)
    return jsonify({"message": "Data captured successfully"})

if __name__ == '__main__':
    app.run(debug=True)
