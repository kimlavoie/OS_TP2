#pragma once
// Auteur: Yves Chiricota 11/11/2000


// Implémentation de la partie physique de la simulation.
// Remarque: Les classes

typedef unsigned char 	byte;
typedef unsigned short	adresse;
typedef unsigned short	word;



// --------------------------------------------------------------------------------
//  Classe:	Memoire
//  Cette classe représente la mémoire physique (principale ou secondaire).
//
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class Memoire {
public:

	Memoire(int k);
	virtual ~Memoire();

	byte&	operator[](adresse);
	const byte&	operator[](adresse) const;
	void set(int adr, byte b) {memoire[adr] = b;}

private:

	int 	size;		// 2^k.
	byte*	memoire;	// Représente la mémoire physique.

};


// --------------------------------------------------------------------------------
//  Classe:	RAM
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class RAM : public Memoire {
public:
	RAM(int k) : Memoire(k) {}
};

// --------------------------------------------------------------------------------
//  Classe:	MemSecondaire
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class MemSecondaire : public Memoire {
public:
	MemSecondaire(int k) : Memoire(k) {}
};

