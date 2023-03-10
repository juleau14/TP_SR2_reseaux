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


    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 ) {

        printf("Taille msg : %d\n", taille_msg);

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;

        paquet.num_seq = numero_paquet;

        init_paquet_avant_envoie(&paquet);

        /* remise à la couche reseau */

        bien_recu = 0;

        while(bien_recu == 0) {                      // boucle d'envoi du paquet

          vers_reseau(&paquet);   // on envoie le paquet

          depart_temporisateur(1000);

          int result_attendre = attendre();

          if (result_attendre == -1) {    // si le paquet acq est arrivé

            arret_temporisateur();        // on arrete le tempo
            de_reseau(&paquet_acquittement) ;  // on recupere le paquet

            printf("Acquittement recu\n");
              
            bien_recu = 1;          // on indique que le paquet a été recu sans erreur
            
            if (numero_paquet == 0) {   // je change le num du prochain paquet a envoyer
              numero_paquet = 1;
            } else {
              numero_paquet = 0;
            }

          }

        } 



      /* lecture des donnees suivantes de la couche application */
      de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
