import time
import random
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Initialize Firebase
cred = credentials.Certificate("path/to/serviceAccountKey.json")  # Replace with your Firebase service account key path
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://holesols-default-rtdb.firebaseio.com/'  # Replace with your Firebase Realtime Database URL
})

def read_ultrasonic_sensor():
    """Simulate reading from an ultrasonic sensor."""
    # Replace this with actual sensor reading logic if using real sensors
    return random.randint(10, 50)  # Random distance between 10 and 50 cm

def read_vibrator_sensor():
    """Simulate reading from a vibrator sensor."""
    # Replace this with actual sensor reading logic if using real sensors
    return random.choice([0, 1])  # Randomly return 0 or 1 to simulate vibration

def update_firebase(distance, vibration):
    """Update Firebase Realtime Database with sensor data."""
    ref = db.reference('manhole')
    ref.update({
        'distance': distance,
        'vibration': vibration
    })

def main():
    """Main function to continuously read sensors and update Firebase."""
    while True:
        distance = read_ultrasonic_sensor()
        vibration = read_vibrator_sensor()

        print(f"Ultrasonic Distance: {distance} cm")
        print(f"Vibration Sensor: {vibration}")

        # Update Firebase with the latest sensor readings
        update_firebase(distance, vibration)

        time.sleep(1)  # Delay for 1 second before the next reading

if __name__ == "__main__":
    main()
