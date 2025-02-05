# **GTK_ClientManager**  

GTK_ClientManager est une application de gestion de clients développée en C avec GTK pour l'interface graphique. Elle permet d'ajouter, modifier, supprimer et afficher des clients enregistrés dans un fichier texte.  

## 🚀 **Fonctionnalités**  
✅ Interface graphique intuitive avec GTK  
✅ Ajout de nouveaux clients  
✅ Modification des informations d’un client  
✅ Suppression d’un client existant  
✅ Affichage et tri des clients dans une liste  
✅ Recherche de clients par code ou nom  

## 🛠️ **Technologies utilisées**  
- **Langage** : C  
- **Bibliothèque GUI** : GTK4 , cairo
- **Stockage** : Fichier texte (`client.txt`,`compte.txt`)  
## 📁 **Structure du projet**

```bash
GTK_ClientManager/
├── src/         # Code source (.c et .h)
├── files/       # Fichiers de données (ex: client.txt, compte.txt)
├── obj/         # Fichiers objets compilés (.o)
├── README.md    # Documentation du projet
└── Makefile     # Script de compilation
```

## 🔧 **Installation et exécution**  
### **1️⃣ Prérequis**  
- Installer GTK4 :  
  ```bash
  sudo apt install libgtk-4-dev  # Ubuntu / Debian
  brew install gtk4              # macOS
- Installer cairo :  
  ```bash
  sudo apt install libcairo2-dev  # Ubuntu / Debian  
  brew install cairo                      # macOS

### **2️⃣ Compilation et exécution**
- Cloner le dépôt :
  ```bash
  git clone https://github.com/JAAFOURAMouez/GTK_ClientManager.git
  cd GTK_ClientManager
  
### **📌 Améliorations possibles**
- Ajouter une base de données SQLite pour stocker les clients
- Ajouter un mode sombre pour l’interface
- Générer des rapports PDF des clients
