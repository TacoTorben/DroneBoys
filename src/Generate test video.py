import cv2
import numpy as np
import random

# -----------------------------
# 1. Generate synthetic video
# -----------------------------
width, height = 1920, 1080
fps = 10
total_frames = 480  # 8 seconds

# Frames where anomalies are present
anomaly_start = 80
anomaly_end = 120

output_file = "../Videos/hue_blobs.avi"
fourcc = cv2.VideoWriter_fourcc(*"MJPG")
out = cv2.VideoWriter(output_file, fourcc, fps, (width, height))

if not out.isOpened():
    raise RuntimeError("Could not open VideoWriter")

random.seed(0)
np.random.seed(0)

# HSV constants
BASE_HUE = 60          # green-ish hue (0-179)
NORMAL_HUE_NOISE = 5   # normal blobs mostly around 60
ANOM_HUE = 0           # red-ish hue cluster
ANOM_HUE_NOISE = 3

SAT = 200
VAL = 200

NUM_NORMAL_BLOBS = 40
NORMAL_RADIUS_RANGE = (8, 25)

NUM_ANOM_BLOBS = 8
ANOM_RADIUS_RANGE = (30, 60)   # big so they affect histogram

def draw_hsv_blobs(frame_hsv, num_blobs, hue_center, hue_noise, radius_range):
    """
    Draws blobs with hue around hue_center on HSV frame.
    frame_hsv is (H, W, 3) in HSV.
    """
    h, w = frame_hsv.shape[:2]
    for _ in range(num_blobs):
        radius = random.randint(*radius_range)
        cx = random.randint(radius, w - radius)
        cy = random.randint(radius, h - radius)

        # sample hue with small noise, wrap to [0, 179]
        hue = int(hue_center + np.random.randint(-hue_noise, hue_noise + 1))
        hue = hue % 180

        # draw filled circle mask, then set HSV values inside
        mask = np.zeros((h, w), dtype=np.uint8)
        cv2.circle(mask, (cx, cy), radius, 255, thickness=-1)

        frame_hsv[mask == 255, 0] = hue
        frame_hsv[mask == 255, 1] = SAT
        frame_hsv[mask == 255, 2] = VAL

for i in range(total_frames):
    # start with flat background at BASE_HUE
    frame_hsv = np.zeros((height, width, 3), dtype=np.uint8)
    frame_hsv[..., 0] = BASE_HUE
    frame_hsv[..., 1] = SAT
    frame_hsv[..., 2] = VAL

    # normal blobs
    draw_hsv_blobs(
        frame_hsv,
        NUM_NORMAL_BLOBS,
        hue_center=BASE_HUE,
        hue_noise=NORMAL_HUE_NOISE,
        radius_range=NORMAL_RADIUS_RANGE,
    )

    # anomalous red-ish blobs in the anomaly window
    if anomaly_start <= i <= anomaly_end:
        draw_hsv_blobs(
            frame_hsv,
            NUM_ANOM_BLOBS,
            hue_center=ANOM_HUE,
            hue_noise=ANOM_HUE_NOISE,
            radius_range=ANOM_RADIUS_RANGE,
        )

    # convert HSV->BGR before writing to video
    frame_bgr = cv2.cvtColor(frame_hsv, cv2.COLOR_HSV2BGR)
    out.write(frame_bgr)

out.release()
print(f"Video saved to {output_file}")
