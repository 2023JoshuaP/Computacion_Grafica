import matplotlib.pyplot as plt
import numpy as np

def generate_histogram_filled(filename, name_histogram):
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
    plt.savefig(name_histogram + '_filled.png')
    plt.show()

generate_histogram_filled('histograma.txt', 'Histogram')