import cv2
import robo_cam_api
import time
import random

import face as f

capture = cv2.VideoCapture(0)
RESOLUTION = capture.get(cv2.CAP_PROP_FRAME_WIDTH), capture.get(cv2.CAP_PROP_FRAME_HEIGHT)
ROBOT_RESOLUTION = -49, 49
CENTER = 0.5, 0.5
TIME_BETWEEN_EXEC = 2
TIME_BETWEEN_SEARCH = 1

dot = [random.random(), random.random()]
# dot = [0.9, 0.9]

searching = True

latest_search_pos = [0, 0]

print(f"cam resolution: {RESOLUTION}")


def get_rel_from_img(img_coord: iter) -> tuple:
    if len(img_coord) != 2:
        raise Exception("The lenth of the relative coordinates != 2")
    return float(img_coord[0] / RESOLUTION[0]), float(img_coord[1] / RESOLUTION[1])


def get_img_coordinates(relative: iter) -> tuple:
    if len(relative) != 2:
        raise Exception("The lenth of the relative coordinates != 2")
    return int(RESOLUTION[0] * relative[0]), int(RESOLUTION[1] * relative[1])


def get_angle_coordinates(relative: iter) -> tuple:
    if len(relative) != 2:
        raise Exception("The lenth of the relative coordinates != 2")
    return int(ROBOT_RESOLUTION[0] * relative[0]), int(ROBOT_RESOLUTION[1] * relative[1])


def turn_by(relative: iter, dot: iter) -> tuple:
    robo_cam_api.move_relative(list(get_angle_coordinates(relative)))
    print(get_angle_coordinates(relative))
    return dot[0] + relative[0], dot[1] + relative[1]


def correct_robot():
    global latest_move
    global dot
    latest_move = time.time()

    print("correcting robot")
    print("prev dot: ", dot)
    relative = CENTER[0] - dot[0], CENTER[1] - dot[1]
    print("relative: ", relative)
    dot = turn_by(relative, dot)
    print("new dot: ", dot)
    print()

    if random.randint(0, 5) == 0:
        dot = [random.random(), random.random()]

def search():
    global latest_search_pos
    global latest_move
    latest_move = time.time()
    move_by = 30

    print("searching")
    """
    Es bewegt sich immer um move_by
    """
    if (latest_search_pos[1] / move_by) % 2 == 0:
        latest_search_pos[0] += move_by
        if latest_search_pos[0] > 180:
            latest_search_pos[0] = 180
            latest_search_pos[1] += move_by
    else:
        latest_search_pos[0] -= move_by
        if latest_search_pos[0] < 0:
            latest_search_pos[0] = 0
            latest_search_pos[1] += move_by

    if latest_search_pos[1] > 180:
        latest_search_pos = [0, 0]

    robo_cam_api.move_absolute(latest_search_pos)



"""
cascade = cv2.CascadeClassifier("data/haarcascade_frontalface_alt2.xml")
history = []

tolerance = (30, 30)
angle_translation = (-50, -40)
pixel_translation = (len(im), len(im[0]))
"""

robo_cam_api.move_absolute([90, 90])

face = f.Face()
latest_move = time.time()
while True:
    _, im = capture.read()

    """
    im_gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    faces = cascade.detectMultiScale(im_gray, scaleFactor=1.5, minNeighbors=1, minSize=(40, 40))

    for x, y, width, height in faces:
        # compute the center and add to the history
        center = x + int(width/2), y + int(height/2)
        history.append(center)

        cv2.rectangle(im, (x, y), (x+width, y+height), color=(0, 0, 255), thickness=2)

    for dot in history:
        image2 = cv2.circle(im, dot, radius=5, color=(0, 255, 0), thickness=-1)

    if len(history) > 50:
        history.pop(0)

    if len(history) <= 0:
        cv2.imshow("Kamera", im)
        continue


    importance_sum = 0
    coordinate = [0, 0]
    for importance, dot in zip(reversed(list(range(len(history)))),history):
        importance_sum += importance + 1
        coordinate[0] = dot[0]
        coordinate[1] = dot[1]
    coordinate[0] /= importance_sum
    coordinate[1] /= importance_sum

    print(cycle)

    cycle += 1
    if cycle % 10 == 0:
        move = coordinate[0] - int(pixel_translation[0] / 2), coordinate[1] - int(pixel_translation[1] / 2)
        move = int((move[0] / pixel_translation[0]) * angle_translation[0]), int((move[1] / pixel_translation[1]) * angle_translation[1])
        robo_cam_api.move_relative(list(move))

        cycle = 0
        history = []

    image2 = cv2.circle(im, dot, radius=10, color=(255, 0, 0), thickness=-1)
    cv2.imshow("Kamera", im)

    time.sleep(0.5)
    """

    r = face.analyze(im)
    if r == -1:
        searching = True
    else:
        searching = False
        dot = get_rel_from_img(r)

    if time.time() - latest_move >= TIME_BETWEEN_SEARCH and searching:
        search()

    if time.time() - latest_move >= TIME_BETWEEN_EXEC and not searching:
        correct_robot()

    im = cv2.circle(im, get_img_coordinates(dot), radius=5, color=(0, 255, 0), thickness=-1)

    cv2.imshow("Kamera", im)
    if cv2.waitKey(20) & 0xFF == ord('q'):
        break

capture.release()
