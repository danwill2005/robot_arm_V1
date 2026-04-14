import customtkinter as ctk
import serial
import time # Added for a tiny delay

# 1. Functions first
def slider_event(value, servo_id):
    if ser and ser.is_open:
        try:
            # We send <S1:3000>
            packet = f"<S{servo_id}:{int(value)}>" 
            ser.write(packet.encode('utf-8'))
            print(f"Sent: {packet}")
            
            # Optional: a tiny sleep (1ms) to let the Arduino breathe
            time.sleep(0.001) 
        except Exception as e:
            print(f"Write error: {e}")

def send_test_button():
    if ser and ser.is_open:
        ser.write(b"<S1:3000>\n")
        print("Test packet sent")

# 2. Serial Setup
try:
    # Changed to 115200 for smoother slider performance
    ser = serial.Serial('COM5', 9600, timeout=0.1) 
    print("Serial port opened successfully!")
except Exception as e:
    print(f"Serial Error: {e}")
    ser = None

# 3. GUI Setup
app = ctk.CTk()
app.title("6-Axis Robot Controller")
app.geometry("400x600")

label = ctk.CTkLabel(app, text="Robot Arm Control", font=("Arial", 20))
label.pack(pady=20)

btn = ctk.CTkButton(app, text="Test Connection", command=send_test_button)
btn.pack(pady=10)

# Create a list to hold our slider objects if we need to access them later
sliders = []

# Loop 6 times to create 6 labels and 6 sliders
for i in range(1, 7):
    # Label for the Servo
    label = ctk.CTkLabel(app, text=f"Servo {i}", font=("Arial", 14))
    label.pack(pady=(10, 0))

    # 1. Set standard defaults for every slider
    from_value = 1000
    to_value = 4500
    set_position = 3000
    
    if(i == 2):
        set_position = 1000
    elif (i == 3):
        from_value = 4500
        to_value = 3000
        set_position = 4500
    elif(i==6):
        from_value = 1000
        to_value = 3200
        set_position = 3000


    # The Slider
    # Lambda captures the current 'i' as 's_id' so the function knows which servo is which
    slider = ctk.CTkSlider(
        app, 
        from_= from_value, 
        to= to_value, 
        number_of_steps=100, 
        command=lambda val, s_id=i: slider_event(val, s_id)
    )
    slider.set(set_position) # Set to middle position
    slider.pack(pady=5)
    sliders.append(slider)


app.mainloop()

if ser:
    ser.close()
