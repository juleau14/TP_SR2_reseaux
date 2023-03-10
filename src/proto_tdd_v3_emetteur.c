/*************************************************************
* proto_tdd_v0 -  émetteur                                   *
* TRANSFERT DE DONNEES  v0                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t paquet_acquittement; /* paquet qui va contenir les acquittements */
    int numero_paquet = 0;  /* numero du premier paquet que l'on va envoyer */
    int bien_recu;          /* variable indiquant si le paquet a ete recu correctement par le recepteur */
    int curseur = 0;            /* curseur d'envoi de la fenetre d'emission */
    int borne_inf;          /* borne inferieur de la fenetre d'emission */
    int borne_sup;          /* borne superieur de la fenetre d'emission */
    int taille_fenetre;     /* taille de la fenetre d'emission */
    int avancement_fenetre; /* valeur d'avancement de la fenetre */
    int N;                  /* valeur à laquelle le curseur doit retourner */
    int nb_acq_recus;       /* nb d'acq deja recu pour chaque envoie */
    int dernier_acqu_recu;   /* stock le num du dernier ack recu */
    int acqu_recu;           /* stock le num de l'ack recu */

    if (argc < 2) {
      taille_fenetre = 7;
    }

    else {
      taille_fenetre = argv[1];
      borne_inf = 0;
      borne_sup = taille_fenetre;
    }

    paquet_t fenetre[taille_fenetre];

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* création de la premiere fenetre d'emission */
    for (int i = 0; i < taille_fenetre; i++) {

      de_application(message, taille_msg);    // lecture des données

      /* creation du paquet */
      for (int j=0; i=j<taille_msg; i=j++) {  
          paquet.info[j] = message[j];
      }

      paquet.lg_info = taille_msg;
      paquet.type = DATA;

      paquet.num_seq = numero_paquet;

      init_paquet_avant_envoie(&paquet);

      fenetre[i] = paquet;  // remplissage de fenetre 

    }

    /* tant que l'émetteur a des données à envoyer */
    while ( (taille_msg != 0) || (curseur != borne_inf) ) {

        /* remise à la couche reseau */

        while (curseur < taille_fenetre) {
          vers_reseau(&fenetre[curseur]);
          curseur++;
        }

        nb_acq_recus = 0;
        depart_temporisateur(400);

        while (nb_acq_recus < taille_fenetre && attendre() == -1) {
          nb_acq_recus++;
          de_reseau(&paquet_acquittement);
          acqu_recu = paquet_acquittement.num_seq;
          
          if (acqu_recu == dernier_acqu_recu) {
            printf("Un paquet s'est perdu car j'ai recu deux ack identiques.\n");
            break;
          }
          dernier_acqu_recu = acqu_recu;
        }



      /* mise a jour de fenetre en fonction de l'avancement */
      de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
