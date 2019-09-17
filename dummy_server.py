# https://pytorch.org/tutorials/intermediate/flask_rest_api_tutorial.html

from flask import request
from flask import Flask, jsonify

app = Flask(__name__)

def get_prediction(*,image_bytes):
    with open("dummy_recv.png", 'wb') as f:
        f.write(image_bytes)
    return 0, "dog"

@app.route('/predict', methods=['POST'])
def predict():
    if request.method == 'POST':
        img_bytes = request.data
        #print(img_bytes)
        class_id, class_name = get_prediction(image_bytes=img_bytes)
        return jsonify({'class_id': class_id, 'class_name': class_name})

if __name__ == '__main__':
    app.run(port=4698)