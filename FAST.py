import cv2
import numpy as np

def fast_detector(image, threshold=60, nonmax_suppression=True):
    if image is None:
        raise ValueError("Input image is None")

    # Initialize the FAST detector
    fast = cv2.FastFeatureDetector_create(threshold, nonmax_suppression)

    # Detect keypoints
    keypoints = fast.detect(image, None)

    # Draw keypoints on the image
    output_image = cv2.drawKeypoints(image, keypoints, None, color=(0, 0, 255))

    # Prepare circle image (copy of output)
    circle_image = output_image.copy()

    if len(keypoints) > 0:
        # Extract (x, y) positions of keypoints
        pts = np.array([kp.pt for kp in keypoints], dtype=np.float32)

        # Get minimal enclosing circle
        (x, y), radius = cv2.minEnclosingCircle(pts)

        center = (int(x), int(y))
        radius = int(radius)

        # Draw the circle and center
        cv2.circle(circle_image, center, radius, (0, 255, 0), 2)
        cv2.circle(circle_image, center, 3, (255, 0, 0), -1)

    return keypoints, output_image, circle_image

def compression(input_image):
    scale_factor = 0.204

    # Resize image
    resized_image = cv2.resize(
        input_image,
        None,
        fx=scale_factor,
        fy=scale_factor,
        interpolation=cv2.INTER_LINEAR
    )

    # JPEG compression parameters (quality 30)
    params = [cv2.IMWRITE_JPEG_QUALITY, 30]

    # Encode to JPEG in memory
    success, buf = cv2.imencode('.jpg', resized_image, params)
    if not success:
        raise RuntimeError("JPEG encoding failed")

    # Decode back to image
    compressed = cv2.imdecode(buf, cv2.IMREAD_COLOR)

    # Apply Gaussian Blur
    compressed = cv2.GaussianBlur(compressed, (3, 3), 0.8)

    return compressed


# Example usage:
if __name__ == "__main__":
    img = cv2.imread("/home/snekkie/DroneBoys/Images/crazy.JPG")
    if img is None:
        raise SystemExit("Failed to load image.")
    img = compression(img)
    blurred = cv2.GaussianBlur(img, (5, 5), 0)
    keypoints, output_image, circle_image = fast_detector(blurred)
    resize = cv2.resize(output_image, (1600, 1200))
    resize2 = cv2.resize(circle_image, (1600, 1200))

    print(f"Detected {len(keypoints)} keypoints.")

    #cv2.imshow("FAST Keypoints", resize)
    cv2.imshow("FAST Keypoints + Enclosing Circle", resize2)
    cv2.waitKey(0)
    cv2.destroyAllWindows()