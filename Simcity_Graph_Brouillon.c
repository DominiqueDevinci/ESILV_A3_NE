#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5

//#define CLEAR_CONSOLE system("clear")
#define color_default() printf("\033[00m")
//#define color_default() printf("\033[0m")
#define color_green() printf("\033[32m")
#define color_red() printf("\033[31m")

int count;
char *sommets="ABCDEF";
bool verbose=true;

struct _maillon{
	int poids;
	char caractere;
	int idSommetAdj;
	int idSommetParent;
	struct _maillon* next;
};

typedef struct _maillon maillon; //maillon est alias

maillon* ajoutMaillon(maillon* liste, maillon* newElement, bool sort){
	if(liste == NULL){
		return newElement;
	}
	else{
		if(sort&&newElement->poids<liste->poids){
			newElement->next=liste;
			return newElement;
		}else{
			maillon* temp = liste;
			while(temp->next != NULL && (temp->poids < newElement->poids||!sort)){
				temp = temp->next;
			}
			newElement->next = temp->next;
			temp->next = newElement;
			return liste;
		}
	}
}

maillon* supprimerMaillon(maillon* liste, maillon* aSuppr){
	if(aSuppr==liste){
		return liste->next;
	}else{
		maillon *tmp=liste;
		while(tmp->next!=NULL&&tmp->next->caractere!=sommets[aSuppr->idSommetParent]){
			tmp=tmp->next;
		}
		if(tmp->next!=NULL){
			tmp->next=tmp->next->next;
		}
		return liste;
	}
}

maillon* creerArrete(maillon** tableau, char caractere, int idSommet, int poids, int idSommetPere){
	maillon* arrete = (maillon*)malloc(sizeof(maillon));
	arrete->poids = poids;
	arrete->caractere = caractere;
	arrete->idSommetAdj = idSommet;
	arrete->next = NULL;
	arrete->idSommetParent=idSommetPere;
	return arrete;
}

maillon** creerGraphe(){
	maillon** tableau = (maillon**)malloc(6*sizeof(maillon*));
	for(int i = 0; i < 6; i++){
		tableau[i] = NULL;
	}
	tableau[A] = ajoutMaillon(tableau[A], creerArrete(tableau,'B', B, 3, A), true);
	tableau[A] = ajoutMaillon(tableau[A], creerArrete(tableau,'C', C, 5, A), true);
	tableau[A] = ajoutMaillon(tableau[A], creerArrete(tableau,'D', D, 9, A), true);
	
	tableau[B] = ajoutMaillon(tableau[B], creerArrete(tableau,'A', A, 3, B), true);
	tableau[B] = ajoutMaillon(tableau[B], creerArrete(tableau,'C', C, 3, B), true);
	tableau[B] = ajoutMaillon(tableau[B], creerArrete(tableau,'D', D, 4, B), true);
	tableau[B] = ajoutMaillon(tableau[B], creerArrete(tableau,'E', E, 7, B), true);
	
	tableau[C] = ajoutMaillon(tableau[C], creerArrete(tableau,'A', A, 5, C), true);
	tableau[C] = ajoutMaillon(tableau[C], creerArrete(tableau,'B', B, 3, C), true);
	tableau[C] = ajoutMaillon(tableau[C], creerArrete(tableau,'D', D, 2, C), true);
	tableau[C] = ajoutMaillon(tableau[C], creerArrete(tableau,'E', E, 6, C), true);
	tableau[C] = ajoutMaillon(tableau[C], creerArrete(tableau,'F', F, 8, C), true);
	
	tableau[D] = ajoutMaillon(tableau[D], creerArrete(tableau,'A', A, 9, D), true);
	tableau[D] = ajoutMaillon(tableau[D], creerArrete(tableau,'B', B, 4, D), true);
	tableau[D] = ajoutMaillon(tableau[D], creerArrete(tableau,'C', C, 2, D), true);
	tableau[D] = ajoutMaillon(tableau[D], creerArrete(tableau,'E', E, 2, D), true);
	tableau[D] = ajoutMaillon(tableau[D], creerArrete(tableau,'F', F, 2, D), true);
	
	tableau[E] = ajoutMaillon(tableau[E], creerArrete(tableau,'B', B, 7, E), true);
	tableau[E] = ajoutMaillon(tableau[E], creerArrete(tableau,'C', C, 6, E), true);
	tableau[E] = ajoutMaillon(tableau[E], creerArrete(tableau,'D', D, 2, E), true);
	tableau[E] = ajoutMaillon(tableau[E], creerArrete(tableau,'F', F, 5, E), true);
	
	tableau[F] = ajoutMaillon(tableau[F], creerArrete(tableau,'D', D, 2, F), true);
	tableau[F] = ajoutMaillon(tableau[F], creerArrete(tableau,'E', E, 5, F), true);
	tableau[F] = ajoutMaillon(tableau[F], creerArrete(tableau,'C', C, 8, F), true);
	
	return tableau;
}

char *spaces(int nb){
	char *spaces=(char*)malloc((nb+2)*sizeof(char));
	for(int i=0; i<nb; i++){
		spaces[i]=' ';
	}
	
	spaces[nb]='|';
	spaces[nb+1]='\0';
	return spaces;
}
char *getPathStr(maillon* path){
	int c=0;
	maillon *compt=path;
	
	if(path==NULL)
	{
		return "";
	}	
	while(compt->next!=NULL){
		c++;
		compt=compt->next;
	}
	char *str=(char*)malloc(sizeof(char)*(c*2+1));
	compt=path->next;
	c=0;
	while(compt!=NULL){
		str[c]=compt->caractere;
		str[c+1]='-';
		c+=2;
		compt=compt->next;
	}
	str[c]='\0';
	//printf("Chemin : %s \n", str);
	return str;
}
void afficherArbre(int **mins, maillon* sommetInitial, maillon* sommetCourant, const maillon* sommetCible, maillon** sommetsInutilises, int sommePoids, int deep, maillon *path, char **shortest_way){
	
	for(int i = 0; i < 6; i++){ //supprime le sommet de depart des sommetInutilises
		if(sommetsInutilises[i] == sommetCourant){
			sommetsInutilises[i] = NULL;
		}
	}
	maillon *arrete=sommetCourant;
	int tmpMin;
	while(arrete != NULL){
		count++;
		int sommetAdj=arrete->idSommetAdj;
		//printf("Parcours du sommet %c\n", sommets[sommetAdj]);
		if(sommetsInutilises[sommetAdj]!=NULL){ // évite de tourner en boucle
			
			tmpMin=mins[sommetInitial->idSommetParent][arrete->idSommetAdj];
			if(verbose){
				printf("%s %c (%d) ", spaces(deep*4), arrete->caractere, sommePoids+arrete->poids);
			}
			// recherche du min pour le couple (sommetInitial, sommetCourant)
			
			if(tmpMin==-1||sommePoids+arrete->poids<tmpMin){ // mins [C][A]
				mins[sommetInitial->idSommetParent][arrete->idSommetAdj]=sommePoids+arrete->poids;
				
				maillon *tmp=(maillon*)malloc(sizeof(maillon));
				tmp->caractere=sommets[sommetCourant->idSommetParent];
				tmp->poids=0;
				tmp->idSommetAdj=0;
				tmp->idSommetParent=0;
				tmp->next=NULL;

				path=ajoutMaillon(path, tmp, false);
				if(verbose){
					printf("\n");
				}
				if(sommetsInutilises[sommetAdj] != sommetCible){ // arrête la récursvité lorsque l'on a atteint la cible
					maillon** copySommetsInutilises=(maillon**)malloc(sizeof(maillon*)*6);
					memcpy(copySommetsInutilises, sommetsInutilises, sizeof(maillon*)*6); 
					afficherArbre(mins, sommetInitial, sommetsInutilises[sommetAdj], sommetCible, copySommetsInutilises, sommePoids+arrete->poids, deep+1, path, shortest_way);
					
				}else{
					(*shortest_way)=getPathStr(path);
				}
				path=supprimerMaillon(path, sommetCourant);
			}else{
				if(verbose){
					printf(" --|X|\n");
				}
			}
			
			
		}
		
		arrete = arrete->next;	
	}
	
}
void search_min(maillon** graphe, int depart, int cible){
	int **mins=(int**)malloc(6*sizeof(int*));
	for(int i=0; i<6; i++){
		mins[i]=(int*)malloc(6*sizeof(int));
		for(int j=0; j<6; j++){
			mins[i][j]=-1;
		}
	}
	count=0;
	maillon *path=NULL;
	
	maillon *mdepart=(maillon*)malloc(sizeof(maillon));
	mdepart->caractere=sommets[depart];
	mdepart->next=NULL;
	
	path=ajoutMaillon(path,mdepart, false);
	char *shortest_way="";
	afficherArbre(mins, graphe[depart], graphe[depart], graphe[cible], graphe, 0, 0, path, &shortest_way);
	printf("Nombre de noueds parcourus : %d\nLe chemin minimum est %s%c et est mesure %d !\n", count, shortest_way, sommets[cible], mins[depart][cible]);
}
int main(int argc, char **argv)
{
	maillon** graphe = creerGraphe();
	verbose=true;
	search_min(graphe, A, F);
	search_min(graphe, B, F);
	search_min(graphe, C, F);
	return 0;
}