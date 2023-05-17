import matplotlib.pyplot as plt
import numpy as np

ser = [0.2, 16.4, 14.4, 312.7]
omp = [0.2, 5.8, 4.4, 90.4]
spup = [1, 2.82, 3.27, 3.45]
N = [155, 1073, 2813, 9797]
X_labels = [155, 1073, 2813, 9797] 

xpoints = np.array(N)
plt.title("SPEEDUP")

# ypoints = np.array(ser)
# plt.plot(xpoints, ypoints, marker="o", label="SERIAL")

ypoints = np.array(spup)
plt.plot(xpoints, ypoints, marker="o") #, label="OPENMP")

# plt.legend()

plt.xlabel("N")
plt.ylabel("SPEEDUP")

plt.xticks(N, X_labels)
plt.savefig("speedup.png")