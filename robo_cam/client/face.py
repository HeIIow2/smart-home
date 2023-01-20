import cv2

cascade = cv2.CascadeClassifier("data/haarcascade_frontalface_alt2.xml")

DIMENSION_TOLERANCE = 20
ALLOWING_FAILS = 30

class Face:
    def __init__(self):
        self.history = [(0.5, 0.5)]
        self.fails_in_a_row = 0

    def analyze(self, img):
        im_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        faces = cascade.detectMultiScale(im_gray, scaleFactor=1.3, minNeighbors=7, minSize=(30, 30))


        x = None
        y = None

        for x, y, width, height in faces:
            # compute the center and add to the history
            center = x + int(width / 2), y + int(height / 2)
            cv2.rectangle(img, (x, y), (x + width, y + height), color=(0, 0, 255), thickness=2)
            self.history.append(center)

            break

        # for dot in history:
        #     image2 = cv2.circle(im, dot, radius=5, color=(0, 255, 0), thickness=-1)

        while len(self.history) > 30:
            self.history.pop(0)

        if x is None or y is None:
            self.fails_in_a_row += 1
        else:
            self.fails_in_a_row = 0
        if self.fails_in_a_row > ALLOWING_FAILS:
            return -1
        return self.history[-1]
