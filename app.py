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

@app.route("/api/data", methods=["GET"])
def get_data():
    data = read_json()
    return jsonify(data)
@app.route("/api/data/update", methods=["POST"])
def update_data():   
    if not request.is_json:
        return jsonify({
            "error": "Request must contain JSON data"
        }), 400

    try:
        new_data = request.get_json()

    except Exception as error:
        print("JSON error:", error)

        return jsonify({
            "error": "Malformed JSON payload"
        }), 400

    if new_data is None:
        return jsonify({
            "error": "Empty JSON payload"
        }), 400

    if write_json(new_data):

        return jsonify({
            "message": "Data captured successfully",
            "data": new_data
        }), 200

    return jsonify({
        "error": "Unable to write data to dynamic.json"
    }), 500

    @app.route("/health", methods=["GET"])
def health():
    return jsonify({
        "status": "running"
    })
	@app.errorhandler(404)
def page_not_found(error):
    return jsonify({
        "error": "Page not found"
    }), 404
	@app.errorhandler(500)
def internal_server_error(error):
    return jsonify({
        "error": "Internal server error"
    }), 500
if __name__ == '__main__':
    app.run(debug=True)
