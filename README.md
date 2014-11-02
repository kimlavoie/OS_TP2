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

Pour l'instant, seul l'algorithme FIFO est implémenté.

#### Jeu de tests

Les générateurs de fichier de commandes ont été implémentés en Python. Pour les exécuter, utiliser les commandes:

```python generateur1.py memory_available > cmd.txt```

```python generateur2.py memory_available repetition > cmd.txt```

Où memory_available est la mémoire déclarée lors de l'appel de simvirt, et repetition est le nombre de répétitions de l'algorithme (voir spécifications dans "d2.doc").

NB: les algorithmes de la spécification n'étaient pas clairs, alors j'ai pris quelques libertés quant à leur implémentation.

### TODO

* Implémenter algorithme de l'horloge
* Refactorer le code (nettoyer le code)
* Documenter le code
* Tester le programme avec le jeu de tests
