# by kadzielm 08.2022
import math
import numpy as np
import scipy
import control


def generate_kalman_gains(ts, ti, omega, harmonics_num, version):
    if version == '2cos':
        harmonics = np.zeros(harmonics_num)
        for i in range(harmonics_num):
            harmonics[i] = i
        a = np.matrix([[1, 0], [0, 0]])
        cosine = np.zeros(len(harmonics))
        for i in range(len(harmonics) - 1):
            cosine[i + 1] = math.cos(omega * ts * (i + 1))
            a_diag = np.matrix([[cosine[i + 1] * 2, -1], [1, 0]])
            a = scipy.linalg.block_diag(a, a_diag)

        q = np.matrix([[1, 0], [0, 1]])
        for i in range(len(harmonics) - 1):
            q_diag = np.matrix([[1, 0], [0, 1]])
            q = scipy.linalg.block_diag(q, q_diag)
        r = ((ti/ts) ** 2)
        h = np.zeros([1, len(harmonics) * 2])
        h[0, 0] = 1
        for i in range(len(harmonics) - 1):
            h[0, 2 + 2 * i] = 1
            h[0, 3 + 2 * i] = -1
        pp = control.dare(a.T, h.T, q, r)
        k_ricatti = pp[0] @ h.T @ np.linalg.inv(h @ pp[0] @ h.T + r)
        return k_ricatti, cosine
    elif version == 'sincos':
        harmonics = np.zeros(harmonics_num)
        for i in range(harmonics_num):
            harmonics[i] = i
        cosine = np.zeros(len(harmonics))
        sine = np.zeros(len(harmonics))
        a = np.matrix([[1, 0], [0, 0]])
        for i in range(len(harmonics) - 1):
            sine[i + 1] = np.sin(omega * ts * (i + 1))
            cosine[i + 1] = np.cos(omega * ts * (i + 1))
            a_diag = np.matrix([[cosine[i + 1], -sine[i + 1]], [sine[i + 1], cosine[i + 1]]])
            a = scipy.linalg.block_diag(a, a_diag)

        q = np.matrix([[1, 0], [0, 1]])
        for i in range(len(harmonics) - 1):
            q_diag = np.matrix([[1, 0], [0, 1]])
            q = scipy.linalg.block_diag(q, q_diag/((i + 1) ** 2))
        r = ((ti/ts) ** 2)
        h = np.zeros([1, len(harmonics) * 2])
        h[0, 0] = 1
        for i in range(len(harmonics) - 1):
            h[0, 2 + 2 * i] = 1
        pp = control.dare(a.T, h.T, q, r)
        k_riccati = pp[0] @ h.T @ np.linalg.inv(h @ pp[0] @ h.T + r)
        return k_riccati, sine, cosine
    else:
        print("Invalid version argument, choose from: '2cos' and 'sincos'")


def generate_mem_map(k_riccati, trigonometric, version):
    f = open("Mem1_K.mem", "w+")
    QMATH_SHIFT = 2
    if version == '2cos':
        for i in range(len(k_riccati)):
            f.write(float_to_mem(trigonometric, 0) + " //cosine " + str(i) + " harmonics " + str(trigonometric) + " \n")
            f.write("000000000\n")
            f.write(float_to_mem(k_riccati[i * 2], QMATH_SHIFT) + " //gain x1 " + str(i) + " harmonic\n")
            f.write(float_to_mem(k_riccati[i * 2 + 1], QMATH_SHIFT) + " //gain x2 " + str(i) + " harmonic\n")
        for i in range(512 - len(k_riccati) * 2):
            f.write("000000000\n")
    elif version == 'sincos':
        for i in range(int(len(k_riccati) / 2)):
            f.write(float_to_mem(trigonometric[1][i], QMATH_SHIFT) + " //cosine " + str(i) + " harmonic [" + str(trigonometric[1][i]) + "] \n")
            f.write(float_to_mem(trigonometric[0][i], QMATH_SHIFT) + " //sine " + str(i) + " harmonic [" + str(trigonometric[0][i]) + "] \n")
            f.write(float_to_mem(k_riccati[i * 2], QMATH_SHIFT) + " //gain x1 " + str(i) + " harmonic " + str(k_riccati[i * 2]) + " \n")
            f.write(float_to_mem(k_riccati[i * 2 + 1], QMATH_SHIFT) + " //gain x2 " + str(i) + " harmonic " + str(k_riccati[i * 2 + 1]) + " \n")
        for i in range(512 - len(k_riccati) * 2):
            f.write("000000000\n")
    else:
        print("Invalid version argument, choose from: '2cos' and 'sincos'")


def float_to_mem(float_data, signed):
    int_data = int(float_data * (2 ** (36 - signed)))
    string = "{:09X}".format((int_data & 0xFFFFFFFFF), '09X')
    return string


def generate_mem(ts, ti, omega, harmonics_num, version):
    kalman = generate_kalman_gains(ts, ti, omega, harmonics_num, version)
    if version == 'sincos':
        generate_mem_map(kalman[0], kalman[1:3], 'sincos')
    elif version == '2cos':
        generate_mem_map(kalman[0], kalman[1], 'sincos')
    else:
        print("Invalid version argument, choose from: '2cos' and 'sincos'")


def generate_header(ts, ti, omega, harmonics_num):
    kalman = generate_kalman_gains(ts, ti, omega, harmonics_num, 'sincos')
    f = open("C_code/Kalman_gains.h", "w+")
    f.write("#define KALMAN_GAIN " + chr(92) + "\n")
    for i in range(len(kalman[0])):
        if i == (len(kalman[0]) - 1):
            f.write("Kalman_gain_dc[" + str(i) + "] = " + str(kalman[0][i][0]) + "\n")
        else:
            f.write("Kalman_gain_dc[" + str(i) + "] = " + str(kalman[0][i][0]) + "," + chr(92) + "\n")
    f.write("#define TS " + str(ts) + "\n")
    f.write("#define OMEGA " + str(float(omega)) + "\n")
    f.write("#define KALMAN_HARMONICS " + str(harmonics_num) + "\n")


generate_mem(0.000016, 0.001, 2*math.pi*50, 21, 'sincos')
