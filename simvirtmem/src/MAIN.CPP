// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "simulateur.h"
#include <cmath>


static const int log2_sz_ram = 10;		// Taille m�moire principale 2^10	(16 frames)
static const int log2_sz_memsec = 14;	// Taille m�moire secondaire 2^14	(256 pages)

static Simulateur	sim(log2_sz_ram, log2_sz_memsec);

int main (int argc, char** argv)
{
    int 	nb_pages = 0;
	char*	fname = 0;

	if ( argc != 3 )
	{
		cout << " usage: memvirt nb_pages nom_de_fichier\n";
		return 1;
	}

	fname = argv[2];
	sscanf(argv[1], "%d", &nb_pages);

	if ( nb_pages*64 + (nb_pages / 32 + 1) > pow(2, log2_sz_memsec) )
	{
		cout << "Erreur: le nombre de pages demandees trop grand\n";
	}

	sim.alloc(nb_pages);
	if(!sim.start(fname))
	{
	    cout << "Erreur: impossible de lire le fichier de commandes" << endl;
	}

	return 0;
}
