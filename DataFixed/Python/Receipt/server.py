# -*- coding: utf-8 -*-

import logging
import flask
import optparse
import cv2
import json
import numpy as np
import cStringIO as StringIO
import imutils
import base64
app = flask.Flask(__name__)
from PIL import Image, ImageDraw, ImageFont
import FixedReceiptData
import exifutil
import traceback
import requests
import time
import codecs

@app.route('/', methods=['GET', 'POST'])
def receipt_ocr():
    string_buffer = None
    if flask.request.method == 'POST':
        string_buffer = flask.request.stream.read()

    if not string_buffer:
        return flask.render_template('index.html', has_result=False)    
    ret, _ = app.server.identify_receipt(string_buffer)
    try:
        ret = json.dumps(ret, indent=4, ensure_ascii=False)
    except Exception as exp:
        logging.error(exp)
    return flask.make_response(ret, 200)

@app.route('/demo', methods=['GET'])
def demo():
#     img = cv2.imread("./data/warmup.jpeg")
    img = open("./data/warmup.jpeg").read()
    return detect_and_render(img)

@app.route('/classify_upload', methods=['POST'])
def classify_upload():
    try:
        # We will save the file to disk for possible data collection.
        imagefile = flask.request.files['imagefile']
        img = imagefile.read()
    except Exception as err:
        logging.info('Uploaded image open error: %s', err)
        return flask.render_template(
            'index.html', has_result=True,
            result=(False, 'Cannot open uploaded image.')
        )
    return detect_and_render(img)


def detect_and_render(img0):
    img = exifutil.open_oriented_im(StringIO.StringIO(img0))
    oh, ow = img.shape[:2]
    ret, time_cost = app.server.identify_receipt(img0)
    if oh > ow:
        img = imutils.resize(img, width=800)
        scale = 800.0 / ow
    else:
        img = imutils.resize(img, height=800)
        scale = 800.0 / oh
    # print(ret)l
    for r in ret["regions"]:
        reg = r["region"]
        reg = [int(reg[0] * scale), int(reg[1] * scale),
               int(reg[2] * scale), int(reg[3] * scale)]
        if r["cls"] % 2 == 0:
            cv2.rectangle(img, (reg[0], reg[1]), (reg[2], reg[3]), (0, 0, 255), 1)
        else:
            cv2.rectangle(img, (reg[0], reg[1]), (reg[2], reg[3]), (255, 0, 0), 1)
    cv2_im = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    pil_im = Image.fromarray(cv2_im)
    draw = ImageDraw.Draw(pil_im)
    font = ImageFont.truetype("data/simsun.ttc", 16, encoding="utf-8")
    for r in ret["regions"]:
        if len(r["result"]) == 0 or len(r["result"][0]) == 0:
            continue
        reg = r["region"]
        reg = [int(reg[0] * scale), int(reg[1] * scale),
               int(reg[2] * scale), int(reg[3] * scale)]
        
        draw.text((reg[0], reg[3]),"%s: %s"%(r["cls"],r["result"][0]),(0,0,255),font=font)
    img = cv2.cvtColor(np.array(pil_im), cv2.COLOR_RGB2BGR)
    
    time_str = ['%.3f' % t for t in time_cost]
    
    type_info = json.dumps(ret.get("type", []),sort_keys=True)
    return flask.render_template(
        'index.html', has_result=True, result=[True, 'time={}'.format(time_str), "class = {}".format(type_info)],
        imagesrc=embed_image_html(img)
    )

def embed_image_html(image):
    cnt = cv2.imencode('.png', image)[1]
    b64 = base64.encodestring(cnt).replace('\n', '')
    return 'data:image/png;base64,' + b64

default_server_url = 'http://10.40.11.90:5004/raw'

class ReceiptServer(object):
    def __init__(self, server_url):
        self.server_url = server_url
        
    def identify_receipt(self, img):
        try:
            start_time = time.time()
            r = requests.post(self.server_url, data=img)
            ret2 = {}
            if r.status_code == 200:
                ret2 = r.json()
                ret2 = FixedReceiptData.FixedReceiptData(ret2)

            end_time = time.time()
            time_cost= end_time - start_time
            logging.info('identify_receipt takes: {0:.3}'.format(time_cost))
            return ret2, [time_cost]
        except Exception as err:
            traceback.print_exc()
            logging.info('identify_receipt error: %s', err)
            return None, [-1]
        
def warmup(_app):
    img_path = "data/warmup.jpeg"
    img_data = codecs.open(img_path).read()
    ret, time1 = _app.server.identify_receipt(img_data)
    logging.info(ret.keys())
    logging.info(time1)

def setup_app(app):
    app.server = ReceiptServer(default_server_url)
    logging.info("Start to warm up...")
    warmup(app)
    logging.info("Warm up finished, server is ready")

def start_from_terminal(app):
    """
    Parse command line options and start the server.
    """
    parser = optparse.OptionParser()
    parser.add_option(
        '-p', '--port',
        help="which port to serve content on",
        type='int', default=5009)
    opts, args = parser.parse_args()
    # Initialize classifier + warm start by forward for allocation
    setup_app(app)
    app.run(debug=False, processes=1, host='0.0.0.0', port=opts.port)

if __name__ == '__main__':
    logging.getLogger().setLevel(logging.INFO)
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    formatter = logging.Formatter(
        "%(asctime)s;%(levelname)s;%(message)s", "%Y-%m-%d %H:%M:%S")
    ch.setFormatter(formatter)
    logging.getLogger().addHandler(ch)
    start_from_terminal(app)
else:
    gunicorn_error_logger = logging.getLogger('gunicorn.error')
    app.logger.handlers = gunicorn_error_logger.handlers
    app.logger.setLevel(logging.INFO)
    setup_app(app)
