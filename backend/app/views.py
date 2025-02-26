"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site 

from app import app, Config,  mongo, Mqtt
from flask import escape, render_template, request, jsonify, send_file, redirect, make_response, send_from_directory 
from json import dumps, loads 
from werkzeug.utils import secure_filename
from datetime import datetime,timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor
 



#####################################
#   Routing for your application    #
#####################################


# 1. CREATE ROUTE FOR '/api/set/combination'
@app.route('/api/set/combination',methods=["POST"])  
def set_combination():
    '''Saves a file to the uploads folder'''
    passcode = request.json.get('code')
    print(f"passcode: {passcode}")

    if request.method == "POST": 
        try: 
            success = mongo.updateCode(passcode)
            if success:
                return jsonify({"status": "complete", "data": "complete"})
            else:
                return jsonify({"status": "failed", "data": "failed"})

        except Exception as ex:
            print(f"set_combination error: f{str(ex)}")
    
# 2. CREATE ROUTE FOR '/api/check/combination'
@app.route('/api/check/combination', methods=['POST'])
def check_combination():
    if request.method == 'POST':
        try:
            passcode = request.form.get('passcode')
            if passcode:
                result = mongo.checkCode(passcode)
                if result != 0:
                    return jsonify({"status": "complete", "data": "complete"})
                else:
                    return jsonify({"status": "failed", "data": "failed"})
        except Exception as ex:
            print(f"check_combination error: f{str(ex)}")

# 3. CREATE ROUTE FOR '/api/update'
@app.route('/api/update', methods=['POST'])
def update_data():
    if request.method == 'POST' and request.is_json:
        try:
            json_data = request.get_json()
            timestamp = datetime.now().timestamp()
            timestamp = floor(timestamp)
            json_data['timestamp'] = timestamp
            Mqtt.publish('620163595_sub', mongo.dumps(json_data))
            Mqtt.publish('620163595', mongo.dumps(json_data))
            result = mongo.addData(json_data)
            if result:
                return jsonify({"status": "complete", "data": "complete"})
            else:
                return jsonify({"status": "failed", "data": "failed"})
        except Exception as ex:
            print(f"update_data error: f{str(ex)}")

        # Mqtt.publish('620163595_pub', json.dumps(json_data))


# 4. CREATE ROUTE FOR '/api/reserve/<start>/<end>'
@app.route('/api/reserve/<start>/<end>', methods=['GET'])
def get_data(start, end):
    if request.method == 'GET':
        try:
            start = int(start)
            end = int(end)
            data = mongo.getData(start, end)
            if data:
                # RETURN FILE IF IT EXISTS IN FOLDER
                return jsonify({"status": "found", "data": data})
                    # FILE DOES NOT EXIST
            else:
                return jsonify({"status": "failed", "data": 0})
        except Exception as ex:
            print(f"get_data error: f{str(ex)}")


# 5. CREATE ROUTE FOR '/api/avg/<start>/<end>'
@app.route('/api/avg/<start>/<end>', methods=['GET'])
def get_avg(start, end):
     if request.method == 'GET':
        try:
            start = int(start)
            end = int(end)
            avg = list(mongo.getAvg(start, end))
            # RETURN FILE IF IT EXISTS IN FOLDER
            if avg:
                return jsonify({"status": "found", "data": avg})
             # FILE DOES NOT EXIST
            else:
                return jsonify({"status": "failed", "data": 0})
        except Exception as ex:
            print(f"get_avg error: f{str(ex)}")

#Sample
@app.route('/api/file/get/<filename>', methods=['GET']) 
def get_images(filename):   
    '''Returns requested file from uploads folder'''
   
    if request.method == "GET":
        directory   = join( getcwd(), Config.UPLOADS_FOLDER) 
        filePath    = join( getcwd(), Config.UPLOADS_FOLDER, filename) 

        # RETURN FILE IF IT EXISTS IN FOLDER
        if exists(filePath):        
            return send_from_directory(directory, filename)
        
        # FILE DOES NOT EXIST
        return jsonify({"status":"file not found"}), 404


@app.route('/api/file/upload',methods=["POST"])  
def upload():
    '''Saves a file to the uploads folder'''
    
    if request.method == "POST": 
        file     = request.files['file']
        filename = secure_filename(file.filename)
        file.save(join(getcwd(),Config.UPLOADS_FOLDER , filename))
        return jsonify({"status":"File upload successful", "filename":f"{filename}" })





###############################################################
# The functions below should be applicable to all Flask apps. #
###############################################################


@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also tell the browser not to cache the rendered page. If we wanted
    to we could change max-age to 600 seconds which would be 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.errorhandler(405)
def page_not_found(error):
    """Custom 404 page."""    
    return jsonify({"status": 404}), 404



