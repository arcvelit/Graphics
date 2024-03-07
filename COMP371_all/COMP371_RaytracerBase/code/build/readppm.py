import sys
import os
import matplotlib.pyplot as plt
import numpy as np

def read_ppm(file_path):
    with open(file_path, 'rb') as f:
        # Read the PPM header
        magic_number = f.readline().decode().strip()
        if magic_number != 'P6':
            raise ValueError("Invalid PPM file format")

        width, height = map(int, f.readline().decode().split())
        max_value = int(f.readline().decode().strip())

        # Read the binary pixel data
        pixels_raw = f.read()
    
    # Convert the binary data to a NumPy array
    pixels = np.frombuffer(pixels_raw, dtype=np.uint8)

    # Reshape the array to represent the image
    image = pixels.reshape((height, width, 3))

    return image

def plot_ppm(image, title):
    plt.imshow(image)
    plt.title(title)
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]

    try:
        ppm_image = read_ppm(file_path)
        # Extract only the filename (excluding directories)
        title = os.path.basename(file_path)
        plot_ppm(ppm_image, title)
    except Exception as e:
        print(f"An error occurred: {e}")
