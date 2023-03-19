Indiquez dans ce fichier des éventuels commentaires à communiquer à l'enseignant.

J'ai eu des soucis pour faire fonctionner le makefile chez moi. Je n'ai donc pas pu tester la V3 pendant un
certains temps. 

La fonction dans le recepteur vers_application() ne renvoie jamais 1 ce qui bloque le programme dans le while et il ne se termine
pas, alors que l'emetteur, lui, termine normalement. Je ne vois pas d'ou cela peut venir. J'ai du faire une erreur 
dans l'algo de l'emetteur qui ne doit pas enovoyer les bonnes données pour les derniers paquets.