import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Configuration du port série
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Remplacez 'COMx' par le port série de votre Arduino

# Initialisation des listes pour stocker les données
emitters_data = {'emetteur1': {'temperature': [], 'humidity': []}, 'emetteur2': {'temperature': [], 'humidity': []}}

# Définir le nombre maximal de données à afficher
max_data_points = 100

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))

def decode_trame(trame):
    trame_type = trame[0]

    if trame_type == '3':
        # Trame de données
        node_id = trame[1:9].strip()
        temperature = int(trame[15:17])
        humidity = int(trame[17:19])

        # Stockage des données en fonction de l'émetteur
        emitters_data[node_id]['temperature'].append(temperature)
        emitters_data[node_id]['humidity'].append(humidity)

def update_graph(frame):
    ax1.clear()
    ax2.clear()

    for emitter, data in emitters_data.items():
        # Tronquer les données si nécessaire
        temperature_display = data['temperature'][-max_data_points:]
        humidity_display = data['humidity'][-max_data_points:]

        ax1.plot(temperature_display, label=f'Temperature - {emitter}')
        ax2.plot(humidity_display, label=f'Humidity - {emitter}')

    for ax in [ax1, ax2]:
        ax.set_xlabel('Time')
        ax.legend()

def close_event(event):
    plt.close()

fig.canvas.mpl_connect('close_event', close_event)
ani = FuncAnimation(fig, update_graph, interval=100, save_count=1000)

while plt.fignum_exists(fig.number):
    try:
        if ser.in_waiting > 0:
            trame = ser.readline().decode('latin-1').strip()  # Utilisez 'latin-1' pour l'encodage
            print(f"Trame reçue : {trame}")
            decode_trame(trame)
        plt.pause(0.1)
    except KeyboardInterrupt:
        break  # Arrêter l'exécution du script avec Ctrl+C

plt.show()
