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
    int taille_fenetre_emission = argv[1];  /* taille de la fenetre passée en argument */ 
    int pointeur_courant = 0;               /* pointeur courant dans la fenetre d'emission

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 ) {

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;

        init_paquet_avant_envoie(&paquet);

        /* remise à la couche reseau */

        int bien_recu = 0;
        int timer_ecoule = 0;
        paquet_t paquet_acquittement;

        do {

          vers_reseau(&paquet);   // on envoie le paquet

          depart_temporisateur(400);

          int result_attendre = attendre();

          if (result_attendre == -1) {    // si le paquet acq est arrivé
            de_reseau(&paquet_acquittement);  // on le recupere 
            arret_temporisateur();
            bien_recu = 0;
            timer_ecoule = 0;
          }

          else {  // si le timer est ecoule
            bien_recu = 0;
            timer_ecoule = 1;
            arret_temporisateur();
          }

          if (timer_ecoule != 1) {    // si le timer n'est pas ecoule et que le paquet est arrivé dans les temps 

            printf("Acquittement recu\n");

            if (paquet_acquittement.type == ACK) {    // si ACK
              bien_recu = 1;          // on indique que le paquet a été recu sans erreur
            }

            else if (paquet_acquittement.type == NACK) {  // sinon si NACK
              bien_recu = 0;          // on indique que le paquet n'a pas ete recu correctement
            }

            else {                    // sinon
              printf("Mauvais type recu.\n");
              bien_recu = 0;
            }
          
          }

        } while (bien_recu == 0);


      /* lecture des donnees suivantes de la couche application */
      de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
