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
    int fin = 0; /* condition d'arrêt */
    int numero_paquet_attendu = 0; // numero du premier paquet que l'on va recevoir

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        // attendre(); /* optionnel ici car de_reseau() fct bloquante */
        de_reseau(&paquet);

        if (verifier_somme_ctrl(&paquet) == 0) {

          if (paquet.num_seq == numero_paquet_attendu) {

            printf("J'ai recu le bon numero de paquet \n");
            
            for (int i=0; i<paquet.lg_info; i++) {
                message[i] = paquet.info[i];
            }
            /* remise des données à la couche application */
            fin = vers_application(message, paquet.lg_info);

            if (numero_paquet_attendu == 0) {
              numero_paquet_attendu = 1;
            } else {
              numero_paquet_attendu = 0;
            }
          }

          else {
            printf("Je n'ai pas recu le bon numéro donc j'ignore le paquet\n");
          }

        }

        else {
          printf("Il y a un pb avec la somme de ctrl, j'ai envoyé un NACK.\n");
        }

        printf("FIN = %d", fin);
    }

    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
