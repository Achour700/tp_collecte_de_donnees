import serial
import matplotlib.pyplot as plt
import time

# Configuration du port série
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Remplacez 'COMx' par le port série de votre Arduino

# Initialisation des listes pour stocker les données
temperature_data_1 = []
humidity_data_1 = []
temperature_data_2 = []
humidity_data_2 = []

# Définir le nombre maximal de données à afficher
max_data_points = 100

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))

def decode_trame(trame):
    trame_type = trame[0]

    if trame_type == '3':
        # Trame de données
        node_id = trame[1:8]
        temperature = int(trame[15:17])
        humidity = int(trame[17:19])

        # Stockage des données en fonction de l'émetteur
        if node_id == 'emetteur1':
            temperature_data_1.append(temperature)
            humidity_data_1.append(humidity)
        elif node_id == 'emetteur2':
            temperature_data_2.append(temperature)
            humidity_data_2.append(humidity)
            update_graph()
            plt.show()

def update_graph():
    ax1.clear()
    ax2.clear()

    ax1.plot(temperature_data_1, label='Temperature - Emetteur 1')
    ax1.plot(temperature_data_2, label='Temperature - Emetteur 2')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Temperature')
    ax1.legend()

    ax2.plot(humidity_data_1, label='Humidity - Emetteur 1')
    ax2.plot(humidity_data_2, label='Humidity - Emetteur 2')
    ax2.set_xlabel('Time')
    ax2.set_ylabel('Humidity')
    ax2.legend()

    fig.canvas.draw_idle()
    plt.show()

    plt.pause(0.1)

def close_event(event):
    plt.close()

fig.canvas.mpl_connect('close_event', close_event)

try:
    while plt.fignum_exists(fig.number):
        if ser.in_waiting > 0:
            trame = ser.readline().decode('latin-1').strip()
            print(f"Trame reçue : {trame}")
            decode_trame(trame)
except KeyboardInterrupt:
    pass  # Arrêter l'exécution du script avec Ctrl+C

plt.show()
