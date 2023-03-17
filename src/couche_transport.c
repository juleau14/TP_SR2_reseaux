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
    return -1;
  }

  else {
    return 0;
  }

}


void init_premiere_fenetre(paquet_t * fenetre, int * curseur, int taille_fenetre, int * taille_message) {
  *curseur = 0;
  int i = 0;
  char message[MAX_INFO];
  paquet_t paquet;

  de_application(message, taille_message);

  while (i < taille_fenetre && taille_fenetre != 0) {
    for (int i = 0; i < *taille_message; i++) {
      paquet.info[i] = message[i];
    }
    paquet.type = DATA;
    paquet.lg_info = *taille_message; 
    paquet.num_seq = i;
    init_paquet_avant_envoie(&paquet);

    de_application(message, taille_message);
    i++;
  }
}


void decaler_fenetre(paquet_t * fenetre, int nb_decalage, int taille_fenetre) {
  paquet_t paquet;
  char message[MAX_INFO];
  int taille_message;
  for (int i = 0; i < taille_fenetre; i++) {   
    if (i + nb_decalage < taille_fenetre) {   // si le paquet a decaler existe deja dans le fenetre alors on le decale simplement
      fenetre[i] = fenetre[i + nb_decalage];
    }
    else {                                  // sinon on ajoute un nouveau paquet
      de_application(message, &taille_message);
      if (taille_message != 0) {
        for (int j = 0; j < taille_message; j++) {
          paquet.info[j] = message[j];
        }
        paquet.lg_info = taille_message;
        paquet.num_seq = fenetre[i].num_seq + nb_decalage;
        paquet.type = DATA;
        init_paquet_avant_envoie(&paquet);
        fenetre[i] = paquet;
      }
    }
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
