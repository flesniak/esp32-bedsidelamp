#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np

MAX_X = 16
MAX_Y = 47
TARGET_A = MAX_Y / np.log(MAX_X)
TARGET_B = 0
N_POINTS = 20
NOISE_A = 0.1
NOISE_B = 0.2
print(TARGET_A)
# create random data
xs = [np.random.uniform(MAX_X) for i in range(N_POINTS)]
ys = []
for i in range(N_POINTS):
    ys.append((TARGET_A + np.random.normal(scale=NOISE_A)) * np.log(xs[i]) + \
              TARGET_B + np.random.normal(scale=NOISE_B))

# plot raw data
plt.figure()
plt.scatter(xs, ys, color='b')

# do fit
tmp_A = []
tmp_B = []
for i in range(len(xs)):
    tmp_A.append([np.log(xs[i]), 1])
    tmp_B.append(ys[i])
B = np.matrix(tmp_B).T
A = np.matrix(tmp_A)
fit = (A.T * A).I * A.T * B
errors = B - A * fit
residual = np.linalg.norm(errors)

print(f"solution: {fit[0]} log(x) + {fit[1]} = y")
print(f"errors: {errors}")
print(f"residual: {residual}")

f1 = fit[0]
f2 = fit[1]
# plot fit
fit_x = np.arange(1, MAX_X+1)
# fit_y = [float(fit[0]) * np.log(x) + float(fit[1]) for x in fit_x]
fit_y = [float(f1) * np.log(x) + float(f2) for x in fit_x]
plt.plot(fit_x, fit_y, color='k')

print([round(abs(MAX_Y-n)) for n in reversed(fit_y)])

plt.xlabel('x')
plt.ylabel('y')
plt.show()