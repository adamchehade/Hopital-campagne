Projet : Hôpital de Campagne - Surveillance IoT
Ce projet propose une solution IoT complète pour surveiller les conditions environnementales et gérer les accès dans un hôpital de campagne. À l'aide de capteurs connectés à une Raspberry Pi, les données sont transmises en temps réel via MQTT et affichées sur un site web pour une visualisation efficace et intuitive.

Objectif du projet
L'objectif principal est d'assurer une surveillance fiable des paramètres critiques dans un environnement hospitalier temporaire, tout en facilitant la gestion des accès grâce à des systèmes connectés.

Fonctionnalités
1. Collecte des données
Capteur de Température et Humidité (DHT11) :
Surveillance continue des conditions climatiques.
Capteur de Mouvement (PIR) :
Détection des mouvements dans les zones sensibles.
Lecteur RFID (PN532) :
Gestion des accès sécurisés pour le personnel autorisé.
2. Communication et Sécurité
Les données des capteurs sont collectées par une Raspberry Pi.
Transmission via le protocole MQTT, avec authentification par nom d'utilisateur et mot de passe pour garantir la sécurité des données.
3. Visualisation des données
Site Web en temps réel :
Interface utilisateur développée avec Python Flask.
Affichage en temps réel des données de température, humidité, détection de mouvement et événements RFID.
Historique des accès RFID avec date et heure.
Alertes automatiques :
Notifications en cas de détection de mouvement ou d'accès non autorisé.
Architecture du projet
Capteurs connectés :

Les capteurs DHT11, PIR et le lecteur RFID PN532 collectent les données essentielles.
Traitement des données :

Les données sont centralisées sur une Raspberry Pi, qui les transmet au serveur via MQTT.
Affichage web :

Les données sont visualisées en temps réel sur un site web auto-actualisé.
Le site inclut un historique des événements et une interface utilisateur simple et attrayante.
Cas d'utilisation
Suivi en temps réel des paramètres environnementaux dans un hôpital de campagne.
Gestion sécurisée des accès grâce à l'identification RFID.
Détection rapide des intrusions dans les zones sensibles pour améliorer la sécurité.
