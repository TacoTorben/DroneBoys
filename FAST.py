import cv2
import numpy as np

saturation_scale = 8
gamma = 0.5
kernel_size = 1


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

#In order to find black clothing, we increase the multiple parameters of the image:
def saturation_increase(self, input_image, saturation_scale):
    hsv_image = cv2.cvtColor(input_image, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv_image)
    s = np.clip(s.astype(np.float32) * saturation_scale, 0, 255).astype(np.uint8)
    enhanced_hsv = cv2.merge([h, s, v])
    output_image = cv2.cvtColor(enhanced_hsv, cv2.COLOR_HSV2BGR)
    return output_image
'''
def brightness_contrast(self, input_image, contrast, brightness):
        alpha = contrast / 100.0     # Contrast control
        beta = brightness - 100      # Brightness control
        output = cv2.convertScaleAbs(input_image, alpha=alpha, beta=beta)
        return output
'''
def gamma_correction(self, input_image, gamma):
    if gamma < 0:
        raise ValueError("Gamma should be non-negative")

    # Build Lookup Table (LUT)
    lut = np.array([
        np.clip((i / 255.0) ** gamma * 255.0, 0, 255)
        for i in range(256)
    ]).astype("uint8")
    # Apply LUT
    output_image = cv2.LUT(input_image, lut)
    return output_image

def median_filter(self, input_image, kernel_size):
    # Apply median blur
    output_image = cv2.medianBlur(input_image, kernel_size)
    return output_image

def bilateral_filter(self, input_image, d, sigma_color, sigma_space):
    output_image = cv2.bilateralFilter(
        input_image,
        d,
        sigma_color,
        sigma_space
    )
    return output_image


if __name__ == "__main__":
    img = cv2.imread("/home/snekkie/DroneBoys/Images/crazy.JPG")
    if img is None:
        raise SystemExit("Failed to load image.")
    img = compression(img)
    #img = saturation_increase(None, img, saturation_scale)
    #img = gamma_correction(None, img, gamma)
    #img = median_filter(None, img, kernel_size)
    #img = bilateral_filter(None, img, d=9, sigma_color=75, sigma_space=75)
    img = cv2.GaussianBlur(img, (5, 5), 0)
    keypoints, output_image, circle_image = fast_detector(img)
    #resize = cv2.resize(output_image, (1600, 1200))
    resize2 = cv2.resize(circle_image, (1600, 1200))

    print(f"Detected {len(keypoints)} keypoints.")

    #cv2.imshow("FAST Keypoints", resize)
    cv2.imshow("FAST Keypoints + Enclosing Circle", resize2)
    cv2.waitKey(0)
    cv2.destroyAllWindows()



    """
    Blackshirt:
    Compression -> Saturation Increase (8) -> Gamma Correction (0.5) -> Bilateral Filter (9,75,75)

    Coloredshirt:
    Compression -> Gaussian Blur (5x5)
    """
    