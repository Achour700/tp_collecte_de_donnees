import serial
import matplotlib.pyplot as plt

# Paramètres de communication série
port = '/dev/ttyUSB0'  # Remplacez par le port série approprié (par exemple, 'COM3' sur Windows)
baudrate = 9600

# Ouvrir la connexion série
ser = serial.Serial(port, baudrate, timeout=1)

# Initialiser les listes pour stocker les données
temps_noeud1, temp_noeud1, humid_noeud1 = [], [], []
temps_noeud2, temp_noeud2, humid_noeud2 = [], [], []

# Créer la figure et les axes pour la température
fig_temp, (ax1_temp, ax2_temp) = plt.subplots(2, 1, sharex=True)

line_temp_noeud1, = ax1_temp.plot(temps_noeud1, temp_noeud1, label='Noeud 1', color='blue')
line_temp_noeud2, = ax2_temp.plot(temps_noeud2, temp_noeud2, label='Noeud 2', color='red')

ax1_temp.set_ylabel('Température Noeud 1', color='blue')
ax2_temp.set_ylabel('Température Noeud 2', color='red')
ax2_temp.set_xlabel('Temps')

# Ajouter une légende pour les températures
lines_temp = [line_temp_noeud1, line_temp_noeud2]
ax1_temp.legend(lines_temp, [line.get_label() for line in lines_temp], loc='upper left')

# Créer la figure et les axes pour l'humidité
fig_humid, (ax1_humid, ax2_humid) = plt.subplots(2, 1, sharex=True)

line_humid_noeud1, = ax1_humid.plot(temps_noeud1, humid_noeud1, label='Noeud 1', color='green')
line_humid_noeud2, = ax2_humid.plot(temps_noeud2, humid_noeud2, label='Noeud 2', color='orange')

ax1_humid.set_ylabel('Humidité Noeud 1', color='green')
ax2_humid.set_ylabel('Humidité Noeud 2', color='orange')
ax2_humid.set_xlabel('Temps')

# Ajouter une légende pour les humidités
lines_humid = [line_humid_noeud1, line_humid_noeud2]
ax1_humid.legend(lines_humid, [line.get_label() for line in lines_humid], loc='upper left')

# Activer le mode interactif
plt.ion()

# Fonction pour lire les données série
def lire_donnees_serie():
    while True:
        line = ser.readline().decode('latin-1').rstrip()  # Lire une ligne depuis le port série et décoder avec 'latin-1'
        if line:
            return line

# Boucle principale
try:
    while True:
        data = lire_donnees_serie()
        print(data)  # Afficher les données reçues depuis Arduino

        # Analyser les données et les ajouter aux listes
        if data.startswith('3'):
            temperature, humidity = int(data[15:17]), int(data[17:19])
            print("Temperature : ", temperature, " Humidite : ", humidity)

            try:
                node_id = int(data[7])
                print("ID de nœud à utiliser : ", node_id)

                if node_id == 2:
                    temps_noeud1.append(len(temps_noeud1) + 1)  # Ajouter un élément à la liste des temps_noeud1
                    temp_noeud1.append(temperature)  # Ajouter un élément à la liste des temp_noeud1
                    humid_noeud1.append(humidity)  # Ajouter un élément à la liste des humid_noeud1
                    line_temp_noeud1.set_data(temps_noeud1, temp_noeud1)
                    line_humid_noeud1.set_data(temps_noeud1, humid_noeud1)

                elif node_id == 3:
                    temps_noeud2.append(len(temps_noeud2) + 1)  # Ajouter un élément à la liste des temps_noeud2
                    temp_noeud2.append(temperature)  # Ajouter un élément à la liste des temp_noeud2
                    humid_noeud2.append(humidity)  # Ajouter un élément à la liste des humid_noeud2
                    line_temp_noeud2.set_data(temps_noeud2, temp_noeud2)
                    line_humid_noeud2.set_data(temps_noeud2, humid_noeud2)
                else:
                    print("ID de nœud invalide.")
                    
                # Ajuster la portée de l'axe x si nécessaire
                for ax_temp, ax_humid in zip([ax1_temp, ax2_temp], [ax1_humid, ax2_humid]):
                    ax_temp.relim()
                    ax_temp.autoscale_view()
                    ax_humid.relim()
                    ax_humid.autoscale_view()

                # Forcer la mise à jour des deux figures
                plt.draw()
                plt.pause(0.5)

            except IndexError:
                print("Erreur d'index lors de l'extraction de l'ID de nœud.")

except KeyboardInterrupt:
    ser.close()
    print("Connexion série fermée.")

# Désactiver le mode interactif avant de quitter
plt.ioff()
plt.show()
