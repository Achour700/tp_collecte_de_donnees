import matplotlib.pyplot as plt
from serial import Serial

# Définissez le port série correspondant à votre Arduino
ser = Serial('/dev/ttyUSB0', 9600)  # Remplacez '/dev/ttyUSB0' par le port série réel

# Initialisation des listes pour stocker les données
temps = []
temperature = []
humidite = []  # Nouvelle liste pour stocker l'humidité

# Fonction pour lire les données série et les ajouter aux listes
def lire_donnees_serie():
    ligne = ser.readline().decode('utf-8').strip()
    if ligne:
        split_data = ligne.split()
        if len(split_data) >= 2:  # Check if there are at least two elements in the split result
            temps.append(len(temps) + 1)  # Utilisez une simple numérotation temporelle
            temperature.append(float(split_data[0]))
            humidite.append(float(split_data[1]))
        else:
            print("Invalid data format:", ligne)

# Lecture continue des données série et mise à jour des graphiques
try:
    while True:
        lire_donnees_serie()

        # Mise à jour du premier graphique (température)
        plt.figure(1)
        plt.plot(temps, temperature, marker='*', linestyle='-', color='b')
        plt.xlabel('Temps')
        plt.ylabel('Température')
        plt.title('Tracé de la température au fil du temps')

        # Mise à jour du deuxième graphique (humidité)
        plt.figure(2)
        plt.plot(temps, humidite, marker='*', linestyle='-', color='r')
        plt.xlabel('Temps')
        plt.ylabel('Humidité')
        plt.title('Tracé de l\'humidité au fil du temps')

        plt.pause(1)  # Rafraîchissement du graphique toutes les secondes

except KeyboardInterrupt:
    print("Arrêt de la lecture des données série.")
    ser.close()
    plt.show()  # Affiche les graphiques à la fin
