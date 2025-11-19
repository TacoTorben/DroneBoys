import cv2 as cv
import numpy as np

#Den fungere ved at tage hele videoen, ikke live, og lave en gennemsnitlig fordeling af
def detect_anomalies_in_video(
        video_path,             #Path to video
        num_hue_bins=180,       #The amount of bins for colors, max 180, each bin corresponds to the angle on hue spectrum 0-180
        anomaly_threshold=1.5,  #How many std deviation above mean color distribution
        save_anomalies=False,   #If the anomaly frames should be saved, false by default (ikke implimenteret)
        output_directory="anomalous_frames" #the directory wherein the anomalous frames should be saved
):
    cap = cv.VideoCapture(video_path) #Load video

    hue_hist = []       #array to save hue histograms per frame
    frames = []         #array to save the anomalous frames in
    frame_indices = []  #array to save frame index in

    #Collect frame histogram per frame
    frame_index = 0
    while True: #ret (return value) = True if cap.read actually read something
        ret, frame = cap.read()
        if not ret: #break condition to break loop
            break

        #Convert frame to hsv
        hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
        h = hsv[:, :, 0] #only consider hue value

        #Make histogram of hue value
        hist = cv.calcHist([h], [0], None, [num_hue_bins], [0, 180]).ravel()

        #Normalize so brightness doesnt matter
        hist_sum = hist.sum()
        if hist_sum > 0:    #if statement to avoid divide by zero error
            hist = hist / hist_sum

        hue_hist.append(hist)
        frames.append(frame)
        frame_indices.append(frame_index)

        frame_index += 1

    cap.release()

    #if no frames found in video
    if len(hue_hist) == 0:
        print("No frames found")
        return

    hue_hists = np.stack(hue_hist, axis=0)  #Shape is (x_frames, num_hue_bins)

    #Compute the normal histogram (mean over all frames)
    mean_hist = np.mean(hue_hists, axis=0)

    #Compute distance of each individual frame from the mean of all frames
    difference_hist = hue_hists-mean_hist[None, :]
    distances = np.linalg.norm(difference_hist, axis=1) #Shape is (x frames)

    #Decide what is unusual/doesn't fit in
    dist_mean = distances.mean()
    dist_std = distances.std() if distances.std() > 0 else 1e-6 #Standard deviation distance calc, the if statement is to clear a bug

    #Any frame whose distance is greater than mean + threshold * std is anomaly
    threshold = dist_mean + anomaly_threshold * dist_std

    anomalous_frames = [index for index, d in zip(frame_indices, distances) if d > threshold] #sammensætter frame_idx og dist (zip) hvis d er større end threshold behold frame index

    print("Distance mean", dist_mean)
    print("Distance std", dist_std)
    print("Anomaly threshold", threshold)
    print("Anomalous frame indices", anomalous_frames)

    return anomalous_frames

    #write code to save anomaly frames for review

#Test
#'''

def resize_video(input_path, output_path, scale=0.22):
    cap = cv.VideoCapture(input_path)

    if not cap.isOpened():
        print("Cannot open video")
        return

    # Get original FPS
    fps = cap.get(cv.CAP_PROP_FPS)

    # Read 1 frame to know size
    ret, frame = cap.read()
    if not ret:
        print("Cannot read first frame")
        return

    # Compute new resolution
    new_w = int(frame.shape[1] * scale)
    new_h = int(frame.shape[0] * scale)

    # Setup VideoWriter
    fourcc = cv.VideoWriter_fourcc(*"mp4v")
    out = cv.VideoWriter(output_path, fourcc, fps, (new_w, new_h))

    # Write the resized first frame
    out.write(cv.resize(frame, (new_w, new_h)))

    # Process the rest
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frame_small = cv.resize(frame, (new_w, new_h))
        out.write(frame_small)

    cap.release()
    out.release()
    print("Done! Saved to:", output_path)

#view video, code by chatgpt
def play_video(video_path):
    cap = cv.VideoCapture(video_path)

    if not cap.isOpened():
        print("Could not open:", video_path)
        return

    print("Press 'q' to quit")
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        cv.imshow("Video Preview", frame)

        # 25 ms delay (~40 FPS). Press q to exit
        if cv.waitKey(25) & 0xFF == ord('q'):
            break


def show_specific_frames(video_path, frame_indices):
    cap = cv.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error: could not open video:", video_path)
        return

    # Sort and remove duplicates just in case
    frame_indices = sorted(set(frame_indices))

    for idx in frame_indices:
        # Set the current position to the frame index
        cap.set(cv.CAP_PROP_POS_FRAMES, idx)

        ret, frame = cap.read()
        if not ret:
            print(f"Warning: could not read frame {idx}")
            continue

        # Show the frame
        cv.imshow(f"Frame {idx}", cv.resize(frame, (1920, 1080)))
        print(f"Showing frame {idx}. Press any key for next, or 'q' to quit.")

        key = cv.waitKey(0) & 0xFF  # wait for a key press
        cv.destroyWindow(f"Frame {idx}")

        if key == ord('q'):
            break

    cap.release()
    cv.destroyAllWindows()


    cap.release()
    cv.destroyAllWindows()


def _make_grid_coords(h, w, rows, cols):
    """Return list of (x0, y0, x1, y1) for each grid cell."""
    cell_h = h // rows
    cell_w = w // cols
    coords = []

    for r in range(rows):
        for c in range(cols):
            y0 = r * cell_h
            x0 = c * cell_w
            # Last row/col takes the remainder pixels
            y1 = (r + 1) * cell_h if r < rows - 1 else h
            x1 = (c + 1) * cell_w if c < cols - 1 else w
            coords.append((x0, y0, x1, y1))

    return coords


def detect_grid_anomalies_HV(
        video_path,
        grid_rows=8,
        grid_cols=8,
        num_h_bins=16,
        num_v_bins=8,
        anomaly_k=2.5  # mean + k * std (global)
):
    cap = cv.VideoCapture(video_path)
    if not cap.isOpened():
        print("Could not open video:", video_path)
        return None

    # --- read first frame just to get size + grid ---
    ret, frame = cap.read()
    if not ret:
        print("Could not read first frame")
        return None

    h, w = frame.shape[:2]
    grid_coords = _make_grid_coords(h, w, grid_rows, grid_cols)
    num_cells = grid_rows * grid_cols
    feat_dim = num_h_bins * num_v_bins

    def compute_cell_features(frame_bgr):
        """Return list of feature vectors (one per cell): H+V 2D hist."""
        hsv = cv.cvtColor(frame_bgr, cv.COLOR_BGR2HSV)
        H = hsv[:, :, 0]
        V = hsv[:, :, 2]
        cell_feats = []

        for (x0, y0, x1, y1) in grid_coords:
            cell_H = H[y0:y1, x0:x1]
            cell_V = V[y0:y1, x0:x1]

            hist = cv.calcHist(
                [cell_H, cell_V],
                channels=[0, 1],
                mask=None,
                histSize=[num_h_bins, num_v_bins],
                ranges=[0, 180, 0, 256]
            ).ravel()

            s = hist.sum()
            if s > 0:
                hist = hist / s    # normalize
            cell_feats.append(hist)

        return cell_feats  # list of length num_cells, each (feat_dim,)

    # ---------- PASS 1: compute mean feature per cell ----------
    sum_feats = np.zeros((num_cells, feat_dim), dtype=np.float64)
    frame_count = 0

    # include the first frame
    cell_feats = compute_cell_features(frame)
    for i, fvec in enumerate(cell_feats):
        sum_feats[i] += fvec
    frame_count += 1

    # rest of frames
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        cell_feats = compute_cell_features(frame)
        for i, fvec in enumerate(cell_feats):
            sum_feats[i] += fvec
        frame_count += 1

    cap.release()

    if frame_count == 0:
        print("No frames found")
        return None

    mean_feats = sum_feats / frame_count  # (num_cells, feat_dim)

    # ---------- PASS 2: compute distances per cell per frame ----------
    cap = cv.VideoCapture(video_path)
    distances = []  # list of (num_cells,) arrays

    frame_idx = 0
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        cell_feats = compute_cell_features(frame)
        d_frame = np.zeros(num_cells, dtype=np.float64)
        for i, fvec in enumerate(cell_feats):
            diff = fvec - mean_feats[i]
            d_frame[i] = np.linalg.norm(diff)
        distances.append(d_frame)
        frame_idx += 1

    cap.release()

    if not distances:
        print("No frames in second pass")
        return None

    distances = np.stack(distances, axis=0)  # shape: (num_frames, num_cells)

    # --- global stats over all cells + frames ---
    all_d = distances.ravel()
    d_mean = all_d.mean()
    d_std = all_d.std() if all_d.std() > 0 else 1e-6
    threshold = d_mean + anomaly_k * d_std

    print(f"Global distance mean: {d_mean:.5f}")
    print(f"Global distance std:  {d_std:.5f}")
    print(f"Threshold (mean + {anomaly_k}*std): {threshold:.5f}")

    # ---------- build anomaly dictionary ----------
    anomalies = {}  # frame_idx -> list of cell indices
    num_frames = distances.shape[0]

    for f in range(num_frames):
        bad_cells = np.where(distances[f] > threshold)[0]
        if bad_cells.size > 0:
            anomalies[f] = bad_cells.tolist()

    print(f"Frames with any anomalous cells: {sorted(anomalies.keys())}")
    return {
        "anomalies": anomalies,
        "grid_coords": grid_coords,
        "frame_count": num_frames,
        "grid_rows": grid_rows,
        "grid_cols": grid_cols,
        "threshold": threshold
    }


def visualize_grid_anomalies(
        video_path,
        result,
        window_scale=0.6  # scale down window size for viewing
):
    if result is None:
        print("No result to visualize.")
        return

    anomalies = result["anomalies"]
    grid_coords = result["grid_coords"]

    if not anomalies:
        print("No anomalies to show.")
        return

    cap = cv.VideoCapture(video_path)
    if not cap.isOpened():
        print("Could not open video:", video_path)
        return

    print("Press any key for next anomalous frame, 'q' to quit")

    anomaly_frames = sorted(anomalies.keys())
    target_iter = iter(anomaly_frames)
    try:
        target = next(target_iter)
    except StopIteration:
        cap.release()
        return

    current_frame_idx = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        if current_frame_idx == target:
            h, w = frame.shape[:2]

            # draw faint full grid
            for (x0, y0, x1, y1) in grid_coords:
                cv.rectangle(frame, (x0, y0), (x1, y1), (60, 60, 60), 1)

            # draw red boxes on anomalous cells
            for cell_idx in anomalies[target]:
                x0, y0, x1, y1 = grid_coords[cell_idx]
                cv.rectangle(frame, (x0, y0), (x1, y1), (0, 0, 255), 2)

            # resize for display
            disp_w = int(w * window_scale)
            disp_h = int(h * window_scale)
            frame_disp = cv.resize(frame, (disp_w, disp_h))

            win_name = f"Anomalous frame {target}"
            cv.imshow(win_name, frame_disp)
            key = cv.waitKey(0) & 0xFF
            cv.destroyWindow(win_name)
            if key == ord('q'):
                break

            try:
                target = next(target_iter)
            except StopIteration:
                break

        current_frame_idx += 1

    cap.release()
    cv.destroyAllWindows()

def make_grid_cells(h, w, rows, cols):
    cell_h = h // rows
    cell_w = w // cols
    coords = []
    for r in range(rows):
        for c in range(cols):
            y0 = r * cell_h
            x0 = c * cell_w
            y1 = (r + 1) * cell_h if r < rows - 1 else h
            x1 = (c + 1) * cell_w if c < cols - 1 else w
            coords.append((x0, y0, x1, y1))
    return coords


def detect_grid_anomalies_background(
        video_path,
        grid_rows=8,
        grid_cols=8,
        num_h_bins=16,
        num_v_bins=8,
        anomaly_k=3.0
):
    cap = cv.VideoCapture(video_path)
    ret, frame = cap.read()
    if not ret:
        print("Video could not be read")
        return None

    h, w = frame.shape[:2]
    grid_coords = make_grid_cells(h, w, grid_rows, grid_cols)
    num_cells = grid_rows * grid_cols
    feat_dim = num_h_bins * num_v_bins

    def cell_hists_from_HV(H, V):
        """Compute all cell histograms from precomputed H,V."""
        feats = []
        for (x0, y0, x1, y1) in grid_coords:
            h_patch = H[y0:y1, x0:x1]
            v_patch = V[y0:y1, x0:x1]
            hist = cv.calcHist(
                [h_patch, v_patch],
                [0, 1],
                None,
                [num_h_bins, num_v_bins],
                [0, 180, 0, 256]
            ).ravel()
            s = hist.sum()
            if s > 0:
                hist = hist / s
            feats.append(hist)
        return feats  # list length num_cells, each (feat_dim,)

    # --- PASS 1: build per-cell model ---
    sums = np.zeros((num_cells, feat_dim), dtype=np.float64)
    count = 0

    while True:
        # use the first frame we already read, then the rest
        if count == 0:
            cur = frame
        else:
            ret, cur = cap.read()
            if not ret:
                break

        hsv = cv.cvtColor(cur, cv.COLOR_BGR2HSV)
        H = hsv[:, :, 0]
        V = hsv[:, :, 2]

        feats = cell_hists_from_HV(H, V)
        for i, fvec in enumerate(feats):
            sums[i] += fvec
        count += 1

    cap.release()
    if count == 0:
        print("No frames in video")
        return None

    means = sums / count  # background model per cell

    # --- PASS 2: detect changes vs own background ---
    cap = cv.VideoCapture(video_path)
    distances = []

    while True:
        ret, cur = cap.read()
        if not ret:
            break

        hsv = cv.cvtColor(cur, cv.COLOR_BGR2HSV)
        H = hsv[:, :, 0]
        V = hsv[:, :, 2]

        feats = cell_hists_from_HV(H, V)
        cell_d = np.zeros(num_cells, dtype=np.float64)
        for i, fvec in enumerate(feats):
            diff = fvec - means[i]
            cell_d[i] = np.linalg.norm(diff)

        distances.append(cell_d)

    cap.release()

    distances = np.vstack(distances)  # (num_frames, num_cells)
    flat = distances.ravel()
    d_mean = flat.mean()
    d_std = flat.std() if flat.std() > 0 else 1e-6
    threshold = d_mean + anomaly_k * d_std

    anomalies = {}
    for f, row in enumerate(distances):
        hits = np.where(row > threshold)[0]
        if hits.size > 0:
            anomalies[f] = hits.tolist()

    print(f"Global mean distance: {d_mean:.5f}")
    print(f"Global std distance:  {d_std:.5f}")
    print(f"Threshold:            {threshold:.5f}")
    print(f"Frames with anomalies: {sorted(anomalies.keys())}")

    return {
        "anomalies": anomalies,
        "grid_coords": grid_coords,
        "threshold": threshold
    }


'''
"../Videos/hue_blobs.avi"
#input_video = resize_video("../Videos/DJI_0121.MP4","../Rezised/DJI_0121_resized.mp4")

video_path = "../Rezised/DJI_0121_resized.mp4"
#detect_anomalies_in_video(video_path)

anomalous_frames, distances = detect_grid_anomalies_in_video(video_path)

show_specific_frames(video_path, anomalous_frames)

'''

#'''
video_path = "../Videos/DJI_0121.mp4"
result = detect_grid_anomalies_background(video_path)

visualize_grid_anomalies(video_path, result, window_scale=0.5)
#'''