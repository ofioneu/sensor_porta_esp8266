from flask import Flask
from flask import request
from flask import render_template
import json
from flask_socketio import SocketIO
from flask_socketio import send, emit
import time
 
app = Flask(__name__, static_url_path='')
app.config['SECRET_KEY'] = '@11tahe89!'
socketio = SocketIO(app)
 
@app.route('/postjson', methods = ['POST', 'GET'])
def getinformation():
    global a    
    content = str(request.get_data())
    content = content.replace('b\"','').replace('\"','')
    #content=content.replace("{'",'{"')
    a = json.dumps(content)
    print(a)

    @socketio.on('message')
    def menssagem(message):
        print(message)
        while True:     
            emit('message', a)
            time.sleep(1)
    return render_template('index.html')



    
    


    


if __name__ == '__main__':
    socketio.run(app,host='0.0.0.0', port= 5000)