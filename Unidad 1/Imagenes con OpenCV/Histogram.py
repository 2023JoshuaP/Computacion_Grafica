import matplotlib.pyplot as plt
import numpy as np
import os

def display_histogram(filename, name_histogram, output_folder):
    dates = np.loadtxt(filename)
    intensitys = dates[:, 0]
    frecuencys = dates[:, 1]
    
    plt.figure(figsize=(10, 5))
    plt.fill_between(intensitys, frecuencys, color='black', alpha=0.8)
    
    plt.yscale('log')
    plt.title(name_histogram)
    plt.xlabel('Intensity')
    plt.ylabel('Frequency (log scale)')
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    
    plt.tight_layout()
    output_path = os.path.join(output_folder, name_histogram + '_filled.png')
    plt.savefig(output_path)
    plt.show()

name_file = input("Enter the name of the file: ")
input_folder = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/Equalization Histogram Images/"
output_folder = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/Equalization Histogram Images/Histograms/"

file_original = os.path.join(input_folder, f"histogram_{name_file}_original.txt")
file_equalized = os.path.join(input_folder, f"histogram_{name_file}_equalized.txt")
file_binarized = os.path.join(input_folder, f"histogram_{name_file}_binarized.txt")

display_histogram(file_original, f"Histogram {name_file} Original", output_folder)
display_histogram(file_equalized, f"Histogram {name_file} Equalized", output_folder)
display_histogram(file_binarized, f"Histogram {name_file} Binarized", output_folder)