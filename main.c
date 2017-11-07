#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int Client(int tube[], char * nomFichier);
int Traiteur(int tube[], char * nomFichier);

int main(int argc, char *argv[]) {
	int pid;
	int tube[2];

	pipe(tube);
	pid = fork();

	if(pid != 0 ) { //Pere
		Client(tube, argv[1]) ;
	}
	else { //Fils
		Traiteur(tube, argv[1]) ;
	}
	return 0;
}

int Client(int tube[], char * nomFichier) {
	char * message;

	long taille = 0;
	FILE * fichier;
	char mode;

	printf("Quelle transformation voulez-vous appliquer ?\n 1: Mirroir\n 2: Supprimer les voyelles\n");
	scanf("%c", &mode); //recuperation du mode aupres de l'utilisateur

	fichier = fopen(nomFichier, "r"); //Ouverture fichier
	if(fichier != NULL) {
		fseek(fichier, 0, SEEK_END);
		taille = ftell(fichier);
		rewind(fichier);				//Replacement au debut
		message = (char*)malloc(taille * sizeof(char));

		fread(message, sizeof(char), taille, fichier);

		//Gestion envoie pipe
		//Envoie des instructions : taille, valeur parametre, tableau
		write(tube[1], &taille, sizeof(taille));
		write(tube[1], &mode, sizeof(char));
		write(tube[1], message, taille*sizeof(char));
		wait(NULL);

		//Fermeture fichier + pipe
		fclose(fichier);
		close(tube[1]);
	}
	return 0;
}

int Traiteur(int tube[], char * nomFichier) {
	char * message;
	char * tableau2;

	long taille=0;
	FILE *fichier;
	char mode;
	int nbvoyelle;

	fichier = fopen(strcat(nomFichier,".res"), "w"); //Ouverture / Creation fichier
	//Receptions des infos
	read(tube[0],&taille,sizeof(taille));
	read(tube[0],&mode, sizeof(char));
	message = (char*)malloc(taille * sizeof(char));
	read(tube[0], message, taille*sizeof(char));

	switch(mode) { //Opperations sur le message
		case '1':
			tableau2= (char*)malloc(taille * sizeof(char));
			for(int i =0; i< taille; i++) {
				tableau2[i] = message[taille-i-1] ;
			}
			break;

		case '2':
			nbvoyelle = 0;
			for(int i =0; i< taille; i++) {
				if(message[i] == 'a' || message[i] == 'e' ||
				message[i] == 'i' || message[i] == 'o' ||
				message[i] == 'u' || message[i] == 'y')
				{
					nbvoyelle ++;
				}
			}
			tableau2 = (char*)malloc((taille-nbvoyelle) * sizeof(char));
			int j = 0;
			for(int i =0; i< taille; i++) {
				if(message[i] != 'a' && message[i] != 'e' &&
				message[i] != 'i' && message[i] != 'o' &&
				message[i] != 'u' && message[i] != 'y')
				{
					tableau2[j] = message[i];
					j++;
				}
			}
			taille = taille-nbvoyelle;
			break;
	}

	fwrite(tableau2,sizeof(char), taille,fichier);
	//Fermeture fichier et pipe
	fclose(fichier);
	close(tube[0]);
	return 0;
}
