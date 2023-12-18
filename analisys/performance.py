import pandas as pd
from matplotlib.backends.backend_pdf import PdfPages
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import A4
import os
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import fsolve
from scipy.integrate import quad
from scipy.interpolate import splrep, BSpline

# file_name = 'logs_flight_6_sd.TXT'
# file_name = 'logs_flight_10_D_motor.TXT'
# file_name = 'logs_flight_6_sd.TXT'
# file_name = 'logs_flight_13_SB.TXT'
# file_name = 'logs_flight_14_SB.TXT'
file_name = 'logs_flight_15_DX_12_03_2023.TXT'
file_folder = '../flight_logs/'
file_path = file_folder + file_name  # Replace 'path_to_your_file.csv' with your file path
name_without_extension = file_name.split('.')[0]

ROCKET_MASS = user_input = float(input("Please enter rocket mass in kg: "))

column_names = ['Ms', 'Code', 'A0', 'CA', 'MA', 'T', 'AX', 'AY', 'AZ', 'GX', 'GY', 'GZ', 'EE']
data = pd.read_csv(file_path, sep="\t", header=0, names=column_names)

def get_liftoff_index():
    return (data.index[data['CA'] >= 5].tolist()[0]) - 2

liftoff_index = get_liftoff_index()
data = data.drop(data.index[:liftoff_index])
data.reset_index(drop=True, inplace=True)

data['Ms'] -= data.loc[0, 'Ms']
# convert to seconds
data['S'] = data['Ms'] / 1000
# create a column for delta time
data['Delta'] = data['S'].diff().fillna(0)
data['Vca'] = data['CA'].diff().fillna(0) / data['Delta'] # velocity from CA
data['Aca'] = data['Vca'].diff().fillna(0) / data['Delta'] # acceleration from CA

print(data.head())
time = data['S']

t, c, k = splrep(time, data['CA'], s=len(time))

# Set boundary condition: making the first and second derivative zero at the beginning (time = 0)
c[0] = 0  
c[1] = 0
# c[2] = 0 # check if this is needed
c[-1] = c[-2]

# Recreate the BSpline with the modified control points
spl = BSpline(t, c, k)

def speed_function(t):
     return spl(t, nu=1)

def acceleration_function(t):
     return spl(t, nu=2)

def integrand(t):
    return ROCKET_MASS * acceleration_function(t)  # Mass * acceleration

speed_values = speed_function(time)
max_speed = np.max(speed_values)
max_speed_index = np.argmax(speed_values)
max_speed_time = time[max_speed_index]

roots = fsolve(acceleration_function, [0, max_speed_time])
scoped_power_time_range = np.linspace(0, roots[1], 1000)

# scope acceleration value between 0 and root near max speed
accel_values = acceleration_function(scoped_power_time_range)

data['V'] = speed_function(time)
data['Alt'] = spl(time)
data['Acc'] = acceleration_function(time)

# calculate the total impulse
total_impulse = quad(integrand, *[0, roots[1]])[0]
max_altitude = round(np.max(spl(data['S'])))
max_altitude_index = np.argmax(spl(data['S']))
max_altitude_time = time[max_altitude_index]

power_flight_time = round(roots[1], 2)
max_accel = round(np.max(accel_values), 2)
time_at_max_accel = round(scoped_power_time_range[np.argmax(accel_values)], 2)
average_descent_speed = round(np.mean(data.iloc[max_altitude_index:]['V']), 2)

info_text = (
    f'Rocket Mass): {ROCKET_MASS} kg\n'
    f'Max altitude: {max_altitude} Mts\n'
    f'Time at max altitude: {max_altitude_time.round(2)} s\n'
    f'Max Speed: {max_speed.round(2)} mts/s\n'
    f'Time at max speed: {max_speed_time.round(2)} s\n'
    f'Power flight time (s): {power_flight_time} s\n'
    f'Max Acceleration: {max_accel} mts/s^2\n'
    f'Time at max acceleration: {time_at_max_accel.round(2)} s\n'
    f'Total Impulse: {round(total_impulse, 2)} Ns\n'
    f'Average descent speed: {average_descent_speed} mts/s\n'
)

print(info_text)

fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(8, 8))
ax1.scatter(time, data['CA'], label='Original Data', s=1)
ax1.plot(time, spl(time), label='Fitted Altitude')
ax1.plot(time, spl(time, nu=1), label='Velocity (mts/s)', color="green")
ax1.scatter(time, data['Vca'], label='Measured Velocity (mts/s)', color="green", s=1)
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Altitude (mts)')
ax1.legend()
ax1.grid(True)

ax2.scatter(time, data['Aca'], label='Measured Acceleration (mts/s^2)', color="blue", s=1)
ax2.plot(time, acceleration_function(time), label='Acceleration', color="blue")
ax2.plot(scoped_power_time_range, acceleration_function(scoped_power_time_range), label='Power Flight', color="orange")
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Acceleration (mts/s^2)')
ax2.legend()
ax2.grid(True)

ax3.plot(data['S'], data['AX'] , label='Acceleration X', color="blue")
ax3.plot(data['S'], data['AY'] , label='Acceleration Y', color="Green")
ax3.plot(data['S'], data['AZ'] , label='Acceleration Z', color="Red")
ax3.set_xlabel('Time (s)')
ax3.set_ylabel('Acceleration (mts/s^2)')
ax3.legend()
ax3.grid(True)

plt.tight_layout()
# plt.show()

# # PDF file name
pdf_file = file_folder + name_without_extension + '_PERFORMANCE.pdf'

if os.path.exists(pdf_file):
    os.remove(pdf_file)

pdf = canvas.Canvas(pdf_file, pagesize=A4)

# Set up title font and style
pdf.setFont("Helvetica-Bold", 16)
text = "Rocket Flight Analisys"
text_width = pdf.stringWidth(info_text)

# Center the title on the page
pdf.drawCentredString(A4[0] / 2, A4[1] - 50, text)

# Add the information text
pdf.setFont("Helvetica", 12)
text_object = pdf.beginText(50, A4[1] - 100)
text_object.setTextOrigin(50, A4[1] - 100)
text_object.setFont("Helvetica", 12)

for line in info_text.split('\n'):
    text_object.textLine(line)

pdf.drawText(text_object)

# Save Matplotlib figure to a file and add it to the PDF
fig.savefig("temp_image.png", bbox_inches="tight")
pdf.drawImage("temp_image.png", 50, 50, width=480, height=480)

pdf.save()

plt.close(fig)

if os.path.exists("./temp_image.png"):
    os.remove("./temp_image.png")