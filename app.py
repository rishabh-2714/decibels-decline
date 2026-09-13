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

def read_json():
    # Safely read raw contents of the JSON file
    if not os.path.exists(JSON_PATH):
        with open(JSON_PATH, 'w') as f:
            json.dump({}, f)
    try:
        with open(JSON_PATH, 'r') as f:
            return json.load(f)
    except json.JSONDecodeError:
        return {}

def write_json(data):
    # Safely overwrite the JSON file with new structural payloads
    with open(JSON_PATH, 'w') as f:
        json.dump(data, f, indent=4)

@app.route('/')
def index():
    # Renders the base template shell
    return render_template('index.html')

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
