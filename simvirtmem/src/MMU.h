#pragma once
// Auteur: Yves Chiricota 11/11/2000


#include <list>
#include <sstream>
#include "memoire.h"
#include "Logger.h"
#include "remplacement.h"
#include "exceptions.h"

class Frame;
class ReplacementPolicy;

//Pas implémenté (manque de motivation ...)
class TLB{};

// --------------------------------------------------------------------------------
//  struct:	TableElem
//  Signification des bits de val.
//  15,14,...,2,1,0
//  15: Présence
//  14: Modifié
//  13: Référencé
//  12,11: inutilisés
//  10..6: adresse
//  5..0: inutilisés
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
struct TableElem {
	TableElem(word _val = 0) : val(_val) {}
	word	val;

	// Si Present() == vrai, représente une adresse physique,
	// sinon, c'est l'adresse d'une page dans la mémoire secondaire.
	adresse	PageBase() const { return val & 0x07C0; }


	bool	Present() const { return val & 0x8000; }
	bool	Modified() const { return val & 0x4000; }
	bool	Referenced() const { return val & 0x2000; }


};

typedef TableElem PTE;
typedef TableElem PDE;

// --------------------------------------------------------------------------------
//  Classe:	DirPages
//  Répertoire de pages
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class DirPages {
public:

	DirPages(int sz = 32) : size(sz), elems(new PDE[sz]) {}

	PDE& getPDE(unsigned int i) { return elems[i];}
	void setPDE(unsigned int i, PDE pde) { elems[i] = pde;}


private:

	int 	size;
	PDE*	elems;

};

// --------------------------------------------------------------------------------
//  Classe:	MMU
//  Implémentation de l'unité de gestion de de mémoire matérielle.
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class MMU {
public:
	MMU(RAM* ram, MemSecondaire* memsec);

	bool	virtalloc(int nb_pages);
	byte	read(adresse);
	bool	write(byte, adresse);

	// Fonction d'échange de pages
	void	load(int page, int frame);
	void	store(int page, int frame);

    adresse getRamAddress(adresse a, bool writeOp);
    adresse TLBLookup(adresse a);
    adresse pageWalk(adresse a, bool writeOp);
    adresse getPageTable(adresse a);
    adresse getFrameFromPT(adresse pt, adresse a);
    adresse getRamFromFrame(adresse frame, adresse a);
    adresse swap(adresse virtualAddress);
    bool accessed(Frame* frame);
    void set_unaccessed(Frame* frame);
    void updatePTE(adresse pt, adresse a, bool writeOp);
    void updatePDE(adresse a, bool writeOp);
private:
	DirPages	dirpages;	// Répertoire de pages. Note: dans un vrai système, ce tableau
							// serait conservé en mémoire principale.

	RAM* ram;
	MemSecondaire* memsec;

	// Kim Lavoie
	TLB tlb;
	adresse endAddress = 0;
	ReplacementPolicy* replacementPolicy;
};



