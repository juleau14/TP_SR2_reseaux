/*************************************************************
* proto_tdd_v0 -  récepteur                                  *
* TRANSFERT DE DONNEES  v0                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

/*************************************************************
* proto_tdd_v0 -  récepteur                                  *
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
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t acquittement;
    int fin = 0; /* condition d'arrêt */
    int numseq_attendu = 0; // numero du premier paquet que l'on va recevoir
    int dernier_numseq_correct = 0;

    acquittement.lg_info = 0;
    acquittement.type = ACK;

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    while ( fin == 0 ) {

      de_reseau(&paquet);


      if ((verifier_somme_ctrl(&paquet) == 0) && paquet.num_seq == numseq_attendu) {

        dernier_numseq_correct = paquet.num_seq;

        acquittement.num_seq = paquet.num_seq;
        init_paquet_avant_envoie(&acquittement);

        vers_reseau(&acquittement);

        for (int i = 0; i <  paquet.lg_info; i++) {
          message[i] = paquet.info[i];
        }

        fin = vers_application(message, paquet.lg_info);

        printf("fin : %d\n", fin);

        if (numseq_attendu == 15) {
          numseq_attendu = 0;
        } else {
          numseq_attendu++;
          }

      }

      else {

        acquittement.num_seq = dernier_numseq_correct;
        init_paquet_avant_envoie(&acquittement);
        vers_reseau(&acquittement);

      }
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
