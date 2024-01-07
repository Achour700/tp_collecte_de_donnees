# Projet de Communication LoRa

## Description

Ce projet implémente une communication bidirectionnelle entre un collecteur de données (noeud collecteur) et des noeuds émetteurs utilisant la technologie LoRa. Les noeuds émetteurs envoient des données telles que
la température et l'humidité, et le collecteur gère l'identification des noeuds, la réception des données, et la gestion des sorties de réseau.

## Codes Source

### Noeud Collecteur

Le code source du noeud collecteur est disponible dans le fichier `noeud_collecteur.ino`. Ce code utilise la bibliothèque LoRa et met en œuvre la gestion de l'identification des noeuds,la réception des données,
et la gestion des sorties de réseau.

### Noeud Émetteur - Affectation d'ID

Le code source du noeud émetteur responsable de la demande d'identification est dans le fichier `emetteurX.ino`.
Ce noeud demande une identification au collecteur, et le collecteur répond en affectant un identifiant unique au noeud émetteur.

### Noeud Émetteur - Envoi de Données

Le code source du noeud émetteur responsable de l'envoi de données est dans le fichier `emetteurX.ino`.
Ce noeud envoie périodiquement des données (température et humidité) au collecteur.

## Configuration

### Configuration Matérielle

- Un module LoRa compatible (Arduino peo et mini pro).
- Carte Arduino compatible.

### Configuration Logicielle

1. Installer la bibliothèque LoRa dans l'IDE Arduino.
2. Charger les codes sources dans les cartes Arduino correspondantes.

## Utilisation

1. Charger le code du noeud collecteur sur une carte Arduino connectée au collecteur.
2. Charger le code du noeud émetteur de demande d'identification et d'envoi de données sur une carte Arduino connectée à un noeud.
3. Assurez-vous que les paramètres LoRa (fréquence, bande passante, etc.) sont configurés correctement pour tous les noeuds.
4. Alimenter les noeuds et observer la communication LoRa.

## Contribution

Les contributions sous forme de pull requests sont les bienvenues. Pour des problèmes ou des suggestions, veuillez ouvrir une issue.

## Licence

xxx

## Auteurs

- [Achour_HOUIMEL]

## Remerciements

Nous remercions la communauté Arduino et les contributeurs de la bibliothèque LoRa pour leur support.

## Problèmes Connus

Aucun problème connu à ce stade.

## Contact

XXXX
