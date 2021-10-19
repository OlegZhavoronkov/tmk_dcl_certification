"""To run this script: python3 motion_detector.py -f path/to/frames/folder """
from os.path import join
from os import walk

import argparse
import imutils
import cv2
import numpy as np

FRAME_WIDTH = 500
DILATE_ITERATIONS = 2
MIN_CONTOURE_AREA = 600
DEFAULT_PROCESSED_FOLDER = "./processed/"
BLOCK_SIZE_THRESH = 57
C_THRESH = 6  # Constant subtracted from the mean or weighted mean
MAX_VALUE_THRESH = 255
GAMMA = 2
MANDREL_HEIGHT = 85


def adjust_gamma(image, gamma=1.0):
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def detect_mandrel_edge():
    """This function detects whether the the beginning or the end of pipe is in the frame"""
    ap = argparse.ArgumentParser()
    ap.add_argument("-f", "--frames", help="path to frames")
    args = vars(ap.parse_args())
    frame_name = []
    for (dirpath, dirnames, filenames) in walk(args['frames']):
        frame_name.extend(filenames)
        break
    frame_name = sorted(frame_name)
    prev_frame = cv2.imread(join(args['frames'], frame_name[0]))
    prev_frame = adjust_gamma(prev_frame, GAMMA)
    prev_frame = imutils.resize(prev_frame, width=FRAME_WIDTH)
    is_mandrel_edge = ["empty"]
    for fnum in range(1, len(frame_name)):
        frame = cv2.imread(join(args['frames'], frame_name[fnum]))
        frame = adjust_gamma(frame, GAMMA)
        frame = imutils.resize(frame, width=FRAME_WIDTH)
        frameDelta = cv2.absdiff(prev_frame, frame)
        frameDelta = cv2.cvtColor(frameDelta, cv2.COLOR_BGR2GRAY)
        thresh = cv2.adaptiveThreshold(frameDelta, MAX_VALUE_THRESH, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                       cv2.THRESH_BINARY_INV, BLOCK_SIZE_THRESH, C_THRESH)
        prev_frame = frame
        thresh = cv2.dilate(thresh, None, iterations=DILATE_ITERATIONS)
        cnts = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        cnts = imutils.grab_contours(cnts)
        frame_copy = frame.copy()
        if not cnts:
            is_mandrel_edge.append("empty")
        else:
            is_mandrel_edge.append("move")
        for c in cnts:
            if cv2.contourArea(c) < MIN_CONTOURE_AREA:
                continue
            (x, y, w, h) = cv2.boundingRect(c)

            cv2.rectangle(frame_copy, pt1=(x, y), pt2=(x + w, y + h), color=(0, 255, 0), thickness=2)

            if (x + w > FRAME_WIDTH / 2) and (x < FRAME_WIDTH / 4) and is_mandrel_edge[fnum - 1] == "empty":
                is_mandrel_edge.append("begin")
                print(frame_name[fnum])
                print("begin")
            elif (x + w > FRAME_WIDTH / 2) and (x < FRAME_WIDTH / 4) and (h > MANDREL_HEIGHT):
                is_mandrel_edge.append("end")
                print(frame_name[fnum])
                print("end")

        str_name = (DEFAULT_PROCESSED_FOLDER + frame_name[fnum])
        cv2.imwrite(str_name, frame_copy)
        #cv2.imshow("Security Feed", frame_copy)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
    cv2.destroyAllWindows()


if __name__ == "__main__":
    detect_mandrel_edge()
