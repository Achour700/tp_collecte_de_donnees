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
            elif data[7] == '2':
                temperature_noeud2.append(temperature)

            # Tracer les courbes
            plt.plot(temps, temperature_noeud1, label='Noeud 1')
            plt.plot(temps, temperature_noeud2, label='Noeud 2')
            plt.xlabel('Temps')
            plt.ylabel('Température')
            plt.legend()
            plt.show()

except KeyboardInterrupt:
    # Fermer la connexion série lorsqu'on appuie sur Ctrl+C
    ser.close()
    print("Connexion série fermée.")

