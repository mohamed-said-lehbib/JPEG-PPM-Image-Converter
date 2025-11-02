## Notre décodeur JPEG à nous 
Ce projet a pour objectif de construire un décodeur JPEG capable de lire une image compressée 
en format `.jpg` et de générer en sortie une image affichable au format `.ppm` ou `.pgm`. 
Il est développé en langage C.



## Membres de groupe :

### J'meili Makhtour
### Bensalem Abdellah
### lehbib Mohamed said

## ️ Fonctionnalités implémentées et division des tâches


| Tâche                                                    |
|----------------------------------------------------------|
| Lecture de l'en-tête JPEG (SOI, DQT, SOF0, DHT, SOS)     |
| Décodage Huffman des coefficients DC et AC               |
| Quantification inverse                                   |
| Zig-zag inverse                                          |
| Conversion YCbCr → RGB                                   | 
| Upsampling                                               | 
| Génération d’un fichier image `.ppm` ou `.pgm`           | 

## Démarche de développement
Nous avons adopté une approche incrémentale et progressive. Tout au long du projet, nous avons 
été amenés à ajuster et modifier de nombreux éléments au fur et à mesure de nos avancées.
Chaque étape nous a permis de mieux comprendre le format JPEG et d'améliorer progressivement notre décodeur.


