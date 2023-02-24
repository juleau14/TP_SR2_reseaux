#include <stdio.h>
#include "couche_transport.h"
#include "services_reseau.h"
#include "application.h"

/* ************************************************************************** */
/* *************** Fonctions utilitaires couche transport ******************* */
/* ************************************************************************** */

// RAJOUTER VOS FONCTIONS DANS CE FICHIER...

void envoyer_acquittement(int type) {
  paquet_t paquet_acquittement;
  paquet_acquittement.type = type;
  paquet_acquittement.lg_info = 0;
  paquet_acquittement.somme_ctrl = 0;
  vers_reseau(&paquet_acquittement);
  printf("Acquittement envoyé\n");
}

void init_paquet_avant_envoie(paquet_t * paquet) {

  paquet->somme_ctrl = 0;

  paquet->somme_ctrl ^= paquet->type;
  paquet->somme_ctrl ^= paquet->num_seq;
  paquet->somme_ctrl ^= paquet->lg_info;

  for (int i = 0; i < paquet->lg_info; i++) {

    paquet->somme_ctrl ^= paquet->info[i];

  }

  printf("Fin d'init du paquet.\n");

}

int verifier_somme_ctrl(paquet_t * paquet) {

  int nouvelle_somme = 0;

  nouvelle_somme ^= paquet->type;
  nouvelle_somme ^= paquet->num_seq;
  nouvelle_somme ^= paquet->lg_info;

  for (int i = 0; i < paquet->lg_info; i++) {

    nouvelle_somme ^= paquet->info[i];

  }

  if (nouvelle_somme != paquet->somme_ctrl) {
    envoyer_acquittement(NACK);
    return -1;
  }

  else {
    envoyer_acquittement(ACK);
    return 0;
  }

}



/* ===================== Fenêtre d'anticipation ============================= */

/*--------------------------------------*/
/* Fonction d'inclusion dans la fenetre */
/*--------------------------------------*/
int dans_fenetre(unsigned int inf, unsigned int pointeur, int taille) {

    unsigned int sup = (inf+taille-1) % SEQ_NUM_SIZE;

    return
        /* inf <= pointeur <= sup */
        ( inf <= sup && pointeur >= inf && pointeur <= sup ) ||
        /* sup < inf <= pointeur */
        ( sup < inf && pointeur >= inf) ||
        /* pointeur <= sup < inf */
        ( sup < inf && pointeur <= sup);
}
