# Use an official Python runtime as a parent image
FROM python:3.9-slim

# Set the working directory in the container to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install any dependencies needed by the script
RUN pip install --no-cache-dir -r requirements.txt

# Make script.py executable
RUN chmod +x main.py

# Define environment variable
ENV NAME search

# Run script.py when the container launches
CMD ["python", "main.py"]