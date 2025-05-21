## Notre décodeur JPEG à nous 


## Membres de groupe :

### J'meili Makhtour
### Bensalem Abdellah
### Mohamed said lehbib 

## ️ Fonctionnalités implémentées et division des tâches


| Tâche                                                    | Membre responsable   |
|----------------------------------------------------------|----------------------|
| Lecture de l'en-tête JPEG (SOI, DQT, SOF0, DHT, SOS)     | Abdellah             |
| Décodage Huffman des coefficients DC et AC               | Makhtour             |
| Quantification inverse                                   | Makhtour             |
| Zig-zag inverse                                           | Abdellah             |
| Transformation inverse DCT                               | Abdellah             |
| Conversion YCbCr → RGB                                   | Mohamed Said         |
| Upsampling                                               | Mohamed Said         |
| Génération d’un fichier image `.ppm` ou `.pgm`           | Mohamed Said         |

##Images de test prises sur le web 




## Problèmes rencontrés 

Nous avons eu des difficultés, surtout au début, avec l’idée de départ du décodage Huffman, qui demande 
un traitement précis des bits. Par la suite, la gestion de l’upsampling a aussi posé problème.
En plus, la coordination de toutes les tâches pour décoder la première image (Invaders) a été compliquée. Après plusieurs 
ajustements, notre organisation s’est améliorée et le projet a bien avancé.


## Anecdotes:
On a passé près de deux jours à obtenir des images correctes mais mal colorées. En fait, l’erreur
était minime : on ne gérait pas correctement le coefficient DC initial dans le cas des images colorées.



