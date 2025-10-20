import cv2 as cv, numpy as np, matplotlib.pyplot as plt


class average_hue:
    def calculate_average_hue(img):
        def remove_dominant_hues(
            bgr,
            k=1,                 # how many dominant hues to remove (1 = single most common)
            hue_bandwidth=8,     # +/- hue range around each peak (in OpenCV hue units: 0..179)
            s_min=20,            # ignore low-saturation pixels when *detecting* dominant hues
            v_min=20,            # ignore low-value (dark) pixels when *detecting* dominant hues
            return_hues=False
        ):
            # Convert to HSV
            hsv = cv.cvtColor(bgr, cv.COLOR_BGR2HSV)
            H, S, V = hsv[:,:,0], hsv[:,:,1], hsv[:,:,2]

            # Mask to consider only "colored" pixels for histogram
            detect_mask = (S >= s_min) & (V >= v_min)

            # Hue histogram over [0,180), 180 bins (OpenCV hue range)
            hist = cv.calcHist([H[detect_mask]], [0], None, [180], [0,180]).flatten()

            if hist.sum() == 0:
                # No colored pixels detected; nothing to remove
                if return_hues:
                    return bgr.copy(), []
                return bgr.copy()

            # Find top-k hue peaks
            peak_bins = np.argsort(hist)[::-1][:k]
            dominant_hues = peak_bins.tolist()

            # Build a mask for pixels whose hue is within hue_bandwidth (circularly) of any peak
            # Circular distance on a 0..179 ring
            H_int = H.astype(np.int16)
            mask = np.zeros_like(H, dtype=bool)
            for peak in dominant_hues:
                diff = np.abs(H_int - int(peak))
                circ_diff = np.minimum(diff, 180 - diff)
                mask |= (circ_diff <= hue_bandwidth)

            # Desaturate those pixels (remove the color)
            hsv_out = hsv.copy()
            hsv_out[:,:,1][mask] = 0

            # Convert back to BGR
            out = cv.cvtColor(hsv_out, cv.COLOR_HSV2BGR)

            if return_hues:
                return out, dominant_hues
            return out


        # --- Example usage ---
        result, hues = remove_dominant_hues(img, k=1, hue_bandwidth=8, return_hues=True)
        return hues
    
    