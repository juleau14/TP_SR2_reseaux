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
#include <stdlib.h>

/* =============================== */
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
  init_reseau(EMISSION);
  int taille_fenetre;

  if (argc > 1) {
    taille_fenetre = atoi(argv[1]);  
  }

  else {
    taille_fenetre = 7;
  }

  paquet_t ack_paquet;
  paquet_t fenetre[16];
  unsigned char message[MAX_INFO];
  int taille_message;
  int borne_inf = 0;
  int curseur = 0;
  int recu;
  int index;

  de_application(message, &taille_message);

  while (taille_message > 0) {

    if (dans_fenetre(borne_inf, curseur, taille_fenetre)) {

      for (int i = 0; i < taille_message; i++) {
        fenetre[curseur].info[i] = message[i];
      }

      fenetre[curseur].type = DATA;
      fenetre[curseur].lg_info = taille_message;
      fenetre[curseur].num_seq = curseur;
      init_paquet_avant_envoie(&fenetre[curseur]);    // init de la somme de controle 

      vers_reseau(&fenetre[curseur]);

      if (curseur == borne_inf) {
        depart_temporisateur(400);
      }

      if (curseur == 15) {
        curseur = 0;
      } else {
        curseur++;
      }

      de_application(message, &taille_message);

    }

    else {

      recu = attendre();

      if (recu == -1) {

        de_reseau(&ack_paquet);

        if (verifier_somme_ctrl(&ack_paquet) == 0 && dans_fenetre(borne_inf, ack_paquet.num_seq, taille_fenetre)) {

          if (ack_paquet.num_seq == 15) {
            ack_paquet.num_seq = 0;
          } else {
            ack_paquet.num_seq++;
          }
          borne_inf = ack_paquet.num_seq;
          arret_temporisateur();

          if (borne_inf != curseur) {

            depart_temporisateur(400);

          }

        }

      }

      else {

        index = borne_inf;
        depart_temporisateur(400);

        while (index != curseur) {

          vers_reseau(&fenetre[index]);
          if (index == 15) {
            index = 0;
          } else {
            index++;
          }
        }

      }

    }

  }

  printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
  return 0;
}
