# Use an official Python runtime as a parent image
FROM python:3.8

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY /analisys /app

# Install any needed packages specified in requirements.txt
RUN pip install --no-cache-dir -r analisys/requirements.txt

# Define environment variable
ENV MPLCONFIGDIR=/tmp/matplotlib

# Command to run the Python script
CMD ["python", "analisys/performance.py"]