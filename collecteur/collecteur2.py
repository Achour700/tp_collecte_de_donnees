import serial
import matplotlib.pyplot as plt

# Paramètres de communication série
port = '/dev/ttyUSB0'  # Remplacez 'COMx' par le port série approprié (par exemple, 'COM3' sur Windows)
baudrate = 9600

# Ouvrir la connexion série
ser = serial.Serial(port, baudrate, timeout=1)

# Initialiser les listes pour stocker les données
temps = []
temperature_noeud1 = []
temperature_noeud2 = []

# Créer la figure et les axes
fig, ax = plt.subplots()
line_noeud1, = ax.plot(temps, temperature_noeud1, label='Noeud 1')
line_noeud2, = ax.plot(temps, temperature_noeud2, label='Noeud 2')

# Configurer la mise en page
ax.set_title('Température en fonction du temps')
ax.set_xlabel('Temps')
ax.set_ylabel('Température')
ax.legend()

# Fonction pour lire les données série
def lire_donnees_serie():
    while True:
        line = ser.readline().decode('utf-8').rstrip()
        if line:
            return line

# Boucle principale
try:
    while True:
        data = lire_donnees_serie()
        print(data)  # Afficher les données reçues depuis Arduino

        # Analyser les données et les ajouter aux listes
        if data.startswith('3'):
            temperature = int(data[15:17])  # Extraire la température
            temps.append(len(temps) + 1)  # Utiliser la position dans la liste comme temps
            if data[7] == '1':
                temperature_noeud1.append(temperature)
                line_noeud1.set_data(temps, temperature_noeud1)
            elif data[7] == '2':
                temperature_noeud2.append(temperature)
                line_noeud2.set_data(temps, temperature_noeud2)

            # Ajuster la portée de l'axe x si nécessaire
            ax.relim()
            ax.autoscale_view()

            # Rafraîchir le dessin
            plt.draw()
            plt.pause(0.1)

except KeyboardInterrupt:
    # Fermer la connexion série lorsqu'on appuie sur Ctrl+C
    ser.close()
    print("Connexion série fermée.")

plt.show()
