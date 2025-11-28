import cv2
import numpy as np

def fast_detector(image, threshold=80, nonmax_suppression=True,
                  dark_thresh=None, min_area=None, show_mask=True):
    """
    FAST detector filtered by large dark region.

    - dark_thresh: if None use Otsu; otherwise a 0-255 value for THRESH_BINARY_INV
    - min_area: if None derive from image size (0.5% of image area)
    - show_mask: display the mask for debugging
    """
    if image is None:
        raise ValueError("Input image is None")

    # Grayscale + CLAHE to improve contrast for dark object detection
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8,8))
    gray_eq = clahe.apply(gray)
    gray_blur = cv2.GaussianBlur(gray_eq, (5, 5), 0)

    h, w = gray_blur.shape
    img_area = h * w
    if min_area is None:
        min_area = max(5000, int(img_area * 0.005))  # at least 5000 px or 0.5% of image

    # Create mask for dark regions
    if dark_thresh is None:
        _, mask = cv2.threshold(gray_blur, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    else:
        _, mask = cv2.threshold(gray_blur, dark_thresh, 255, cv2.THRESH_BINARY_INV)

    # Morphological cleanup
    ksize = max(3, int(min(w, h) / 200))  # scale kernel with image size
    if ksize % 2 == 0:
        ksize += 1
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (ksize, ksize))
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=2)

    # Optionally show mask for tuning
    if show_mask:
        cv2.imshow("dark_mask", cv2.resize(mask, (min(800,w), min(600,h))))

    # Find contours and pick the largest
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    largest_contour = None
    largest_area = 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > largest_area:
            largest_area = area
            largest_contour = cnt

    output_image = image.copy()
    circle_image = image.copy()

    # FAST on the processed grayscale
    fast = cv2.FastFeatureDetector_create(threshold, nonmax_suppression)
    keypoints = fast.detect(gray_blur, None)

    keypoints_filtered = []
    # If we have a contour, prefer it even if below min_area (but warn)
    if largest_contour is not None:
        if largest_area < min_area:
            print(f"Warning: largest contour area {largest_area} < min_area {min_area}. Using largest anyway.")
        # compute enclosing circle for largest contour
        pts = largest_contour.reshape(-1, 2).astype(np.float32)
        (x, y), radius = cv2.minEnclosingCircle(pts)
        center = (int(x), int(y))
        radius = int(radius)
        cv2.circle(circle_image, center, radius, (0, 255, 0), 2)
        cv2.circle(circle_image, center, 3, (255, 0, 0), -1)

        # filter keypoints inside mask
        for kp in keypoints:
            px, py = int(kp.pt[0]), int(kp.pt[1])
            if 0 <= px < w and 0 <= py < h and mask[py, px] != 0:
                keypoints_filtered.append(kp)
    else:
        # no contour found -> return empty filtered set
        print("No dark contour found.")
        keypoints_filtered = []

    output_image = cv2.drawKeypoints(image, keypoints_filtered, None, color=(0, 0, 255))

    return keypoints_filtered, output_image, circle_image

def increase_brightness(img, value=30):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)
    lim = 255 - value
    v[v > lim] = 255
    v[v <= lim] += value
    final_hsv = cv2.merge((h, s, v))
    img = cv2.cvtColor(final_hsv, cv2.COLOR_HSV2BGR)
    return img

if __name__ == "__main__":
    img = cv2.imread("/home/snekkie/DroneBoys/Images/blackjesper30.JPG")
    if img is None:
        raise SystemExit("Failed to load image.")

    bright = increase_brightness(img, value=20)
    denoised = cv2.fastNlMeansDenoisingColored(bright, None, 10, 10, 7, 21)

    # Try Otsu (dark_thresh=None), set show_mask=True to debug mask
    keypoints, output_image, circle_image = fast_detector(denoised,
                                                          threshold=90,
                                                          nonmax_suppression=True,
                                                          dark_thresh=None,
                                                          min_area=None,
                                                          show_mask=True)

    print(f"Detected {len(keypoints)} keypoints after filtering.")

    resize = cv2.resize(output_image, (1600, 1200))
    resize2 = cv2.resize(circle_image, (1600, 1200))

    cv2.imshow("FAST Keypoints (filtered)", resize)
    cv2.imshow("Detected dark region", resize2)
    cv2.waitKey(0)
    cv2.destroyAllWindows()