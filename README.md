# Systèmes d'exploitation - TP2
## Mémoire virtuelle

### Introduction

Ce projet contient ma solution au TP2 sur l'implémentation d'une mémoire virtuelle.

### Auteur

Kim Lavoie

### Fonctionnalités implémentées

#### Gestion de la mémoire virtuelle

Le système de mémoire virtuelle est complet. Le projet gère la conversion des adresses virtuelles en adresses physiques, les défauts de page et la swap. La TLB n'a pas été implémentée.

J'ai aussi fait un système de log, qui écrit dans "log.txt" lorsqu'une de mes exceptions est lancée.

Les algorithmes FIFO et Horloge sont fonctionnels.

#### Jeu de tests

Les générateurs de fichier de commandes ont été implémentés en Python. Pour les exécuter, utiliser les commandes:

```python generateur1.py memory_available > cmd.txt```

```python generateur2.py memory_available repetition > cmd.txt```

Où memory_available est la mémoire déclarée lors de l'appel de simvirt, et repetition est le nombre de répétitions de l'algorithme (voir spécifications dans "d2.doc").

NB: les algorithmes de la spécification n'étaient pas clairs, alors j'ai pris quelques libertés quant à leur implémentation.

### À noter

* Le projet Code::Block fait un define DEBUG quand il compile en mode Debug. Si vous compilez en Release, ce define n'existe pas, et rien ne s'affiche (et c'est volontaire).
* Pour utiliser l'algorithme de l'horloge, il faut faire un define RP_CLOCK_ALGORITHM, sinon FIFO est utilisé par défaut. Dans le projet, le mode Debug est avec FIFO et Release est avec Horloge.
* Pour exécuter en ligne de commandes: `simvirtmem quantite-memoire fichier-commande`
