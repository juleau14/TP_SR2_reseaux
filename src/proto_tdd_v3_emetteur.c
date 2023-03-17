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
    int curseur = 0;        /* curseur d'envoi de la fenetre d'emission */
    int borne_inf;          /* borne inferieur de la fenetre d'emission */
    int borne_sup;          /* borne superieur de la fenetre d'emission */
    int taille_fenetre;     /* taille de la fenetre d'emission */
    int avancement_fenetre; /* valeur d'avancement de la fenetre */
    int N;                  /* valeur à laquelle le curseur doit retourner */
    int nb_acq_recus;       /* nb d'acq deja recus normalement pour chaque envoie */
    int dernier_acqu_recu;  /* stock le numseq du dernier ack recu */
    int acqu_recu;          /* stock le numseq de l'ack recu */

    if (argc < 2) {
      taille_fenetre = 7;
    }

    else {
      taille_fenetre = argv[1];
      borne_inf = 0;
      borne_sup = taille_fenetre;
    }

    paquet_t fenetre[taille_fenetre];
    paquet_t paquet;

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    init_premiere_fenetre(fenetre, &curseur, taille_fenetre, &taille_msg);
    
    while (taille_msg != 0 || curseur != borne_inf) {   // tant qu'on lit encore des données

      while (curseur < taille_fenetre) {
        if (curseur == 0) {
          depart_temporisateur(400);
        }

        vers_reseau(&fenetre[curseur]);

        curseur++;

      }

      if (attendre() == -1) {
        
      }

      }

    }


    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
