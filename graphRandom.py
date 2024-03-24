import matplotlib.pyplot as plt
import numpy as np

# Define filename
filename = "test.txt"

# Read data from file
data = np.loadtxt(filename)

# Plot the distribution as a histogram
plt.hist(data)
plt.xlabel("Data values")
plt.ylabel("Frequency")
plt.title("Distribution of data in " + filename)
plt.show()
