from flask import Flask, Response, render_template
import cv2

app = Flask(__name__)

# working code for video streaming =============================
#  remaining to work with the end part of opencv


camera = cv2.VideoCapture(0)
"""
    Creates video object using the webcam (0)
        
        Video Capture of cv2 library is used to create an camera object
    which references the webcam.
"""
def gen_frames():
    """
        Function to get frames using the opencv
    """ 
    while True:
        success, frame = camera.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    app.run(debug=True)




# @app.route("/hello")
# def hello_world():
#     return "<p>Hello, World!</p>"



# @app.route("/")
# def stream_video():
#     get_image_frame()
#     return "<p>Hello, World!</p>"

# def get_image_frame():
#     import cv2 as cv
#     video = cv.VideoCapture(0)
#     while True:
#         return_value, frame = video.read()
#         cv.imshow("Image window", frame)
#         key = cv.waitKey(1)
#         if key == ord("q"):
#             break
#     video.release()
#     cv.destroyAllWindows()
#     cv.waitKey(1)


# @app.route("/check")
# def check():
#     return Response(gen_function())

# @app.route("/")
# def check2():
#     return Response(gen_template_function())

# def gen_function():
#     yield "<p>Hello, World!</p>"
#     for i in range(19):
#         yield "<p>Hello, World! middle</p>"
#     yield "<p>Hello, World! foooter</p>"

# def gen_template_function():
#     yield render_template("header.html")
#     for i in range(19):
#         yield render_template("body.html")
#     yield render_template("footer.html")


# from flask import Flask, render_template, Response
# from camera import Camera

# app = Flask(__name__)

# @app.route('/')
# def index():
#     return render_template('index.html')

# def gen(camera):
#     while True:
#         frame = camera.get_frame()
#         yield (b'--frame\r\n'
#                b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# @app.route('/video_feed')
# def video_feed():
#     return Response(gen(Camera()),
#                     mimetype='multipart/x-mixed-replace; boundary=frame')

# if __name__ == '__main__':
#     app.run(host='0.0.0.0', port= 8989, debug=True)