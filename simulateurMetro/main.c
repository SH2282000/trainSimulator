#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h> //was trying to get emoji from a file #FAIL
#include <sys/ioctl.h> //get window size
#include <signal.h> //LIB POUR key_pressed
#include <termios.h> //LIB POUR key_pressed
#include <fcntl.h>//LIB POUR key_pressed

//#include <wchar.h> //was trying to get emoji from a file #FAIL
#include <string.h>
#include <pthread.h> //multi-thread programming


#define SPECHARQTY 100 // QUANTITE DE CARACTERES SPECIAUX STOCKABLES
#define HMAP 53  // HAUTEUR DE LA STATION
#define WMAP 179 // LARGEUR DE LA STATION

struct SpeChar
{
  char id; //IDENTIFIANT DU CARACTERE DANS LE FICHIER TXT
  char character[100]; //CARACTERE SPECIAL EN LUI MEME STOCKER SOUS FORME DE CHAINE DE CHAR
};
struct Pos
{
  int x, y; // PERMER DE STOCKER N'IMPORTE QUELLE COORDONNÉES
};
typedef struct Pos Pos; 

struct Passager // PERMER DE STOCKER N'IMPORTE QUELLE PASSAGER AVEC SES COORDONNÉES AINSI QUE SON SUCCESSEUR
{
  Pos pos;
  struct Passager *next;
};
struct FirstPassager // PERMER DE STOCKER LE PREMIER PASSAGER DE CHAQUE COTE DU QUAI
{
  struct Passager *first;
};
typedef struct SpeChar SpeChar; 
typedef struct Passager Passager; 
typedef struct FirstPassager FirstPassager; 

/*void assignement(SpeChar specialChar[], char singleSpeChar[], char id)
{
  int i = 0;
  do{
    i++;
  }while(specialChar[i].id!=-1);

  specialChar[i].id = id;
  strcpy(specialChar[i].character, singleSpeChar);
}*/

char *search(SpeChar specialChar[], char id) // PERMER DE RETOURNER UN CARACTERE SPECIAL A PARTIR DE SON IDENTIFIANT
{
  int i = 0; 
  while(specialChar[i].id!=id && i<SPECHARQTY) i++;
  if(i<SPECHARQTY) return specialChar[i].character;
  else return " ";
}

int displayObject(unsigned char **mapObject, Pos pos, Pos posWin, SpeChar specialChar[], int *height) // PERMER D'AFFICHER DIFFERENTS OBJETS TELS QUE LA MAP OU LES TRAINS 
{
  for (int x = 0; x < *height; x++)
  {
    for (int y = 0; mapObject[x][y]!='\0'; y++) //CHAQUE FIN DE LIGNE SE TERMINE PAR \0 COMME SPECIFIÉ DANS LOADOBJECT
    {
      //printf("\033[%d;%dH ─────── ▄ ▄ ▄ ──────── ▄ ▄ ▄ ─────── ▄ ▄ ▄ ─────── ▄ ▄ ▄ ─────    \n",pos_x,pos_y);
      if ((x + pos.x)>=posWin.x && (y+pos.y)>=posWin.y && (x + pos.x)<HMAP+posWin.x && (y+pos.y)<WMAP+posWin.y) // VERIFIE QUE LE CURSEUR SE TROUVE TOUJOURS DANS LA MAP AVANT D'AFFICHER
        printf("\033[%d;%dH%s", x + pos.x, y+pos.y, search(specialChar, mapObject[x][y]));
    }
  }
  return 0;
}

unsigned char **loadObject(unsigned char **mapObject, char *pathFile, int *height)
{
  FILE *imagetxt = fopen(pathFile, "r");
  if(imagetxt==NULL)
  {
    printf("incorrect path of file\n");
    exit(-1);
  }
  
  printf("loading object\n");
  char reader = 0;
  int x = 0, y = 0;

  while((reader=fgetc(imagetxt))!=-1) // CHARGE LE CONTENU DU FICHIER ET LE MET DANS UN TABLEAU ALLOUÉ DYNAMIQUEMENT EN X ET EN Y
  {
    if (reader == '\n')
    {
      mapObject = (unsigned char **) realloc(mapObject, (x+2)*sizeof(unsigned char**)); //increment size of mapped object on x
      printf("try if %d\n", x);
      mapObject[x][y+1] = '\0';
      y=0;
      x++;
    }
    else 
    {
      *(mapObject+x) = (unsigned char *) realloc(*(mapObject+x), (y+2)*sizeof(unsigned char)); //increment size of mapped object on y
      mapObject[x][y] = (unsigned char)reader;
      printf("try else %d %lc\n", y, mapObject[x][y]);
      y++;
    }
    //printf("_%c_ x: %d, y: %d\n", mapObject[x][y], x, y);
  }
  *height = x+1;


  printf("trying to display raw  %d\n", *height);
  for (int i = 0; i < *height; ++i) // PERMET JUSTE DE VERIFIER QUE TOUT EST BIEN CHARGER, EFACABLE SANS SOUCI #DEBUGGAGE
  {
    for (int j = 0; mapObject[i][j]!='\0'; ++j)
    {
      printf("%lc", mapObject[i][j]);
    }
    printf("\n");
  }
  return mapObject;
}
void defineTranslator(SpeChar specialChar[]) // PERMET D'ATTRIBUER CHAQUE CARACTERE SPECIABLE AFFICHABKE DANS LE TERMINAL À UN IDENTIFIANT LISIBLE DANS LE FICHIER
{
  specialChar[0].id = 'a';
  strcpy(specialChar[0].character, "│");
  specialChar[1].id = 'b';
  strcpy(specialChar[1].character, "┘");
  specialChar[2].id = 'c';
  strcpy(specialChar[2].character, "└");
  specialChar[3].id = 'd';
  strcpy(specialChar[3].character, "┌");
  specialChar[4].id = 'e';
  strcpy(specialChar[4].character, "─");
  specialChar[5].id = 'f';
  strcpy(specialChar[5].character, "┬");
  specialChar[6].id = 'g';
  strcpy(specialChar[6].character, "├");
  specialChar[7].id = 'h';
  strcpy(specialChar[7].character, "┤");
  specialChar[8].id = 'i';
  strcpy(specialChar[8].character, "┐");
  specialChar[9].id = 'j';
  strcpy(specialChar[9].character, "┴");
  specialChar[10].id = 'k';
  strcpy(specialChar[10].character, "═");
  specialChar[11].id = 'l';
  strcpy(specialChar[11].character, "║");
  specialChar[12].id = 'm';
  strcpy(specialChar[12].character, "╬");
  specialChar[13].id = 'n';
  strcpy(specialChar[13].character, "╦");
  specialChar[14].id = 'o';
  strcpy(specialChar[14].character, "╩");
  specialChar[15].id = 'p';
  strcpy(specialChar[15].character, "▒");
  specialChar[16].id = 'q';
  strcpy(specialChar[16].character, "▄");

  specialChar[17].id = 'r';
  strcpy(specialChar[17].character, "◯");  

  //doors
  specialChar[18].id = 's';
  strcpy(specialChar[18].character, "═");

  //doors
  specialChar[19].id = 't';
  strcpy(specialChar[19].character, " ");

  specialChar[26].id = ' ';
  strcpy(specialChar[26].character, " ");
  specialChar[27].id = '\n';
  strcpy(specialChar[27].character, "\n");
}
void displayRailOnly(unsigned char **mapStation, Pos posWin, Pos posTrain, SpeChar specialChar[]) // PERMET D'AFFICHER LES RAILS UNIQUEMENT OÙ LE TRAIN EST PASSÉ POUR ÉVITER DE RECHARGER TOUT L'AFFICHAGE
{
  int wTrain = 120, hTrain = 5;
  for (int x = 19; x <= 36; x++) // AFFICHE EN X DE 19 À 36 UNIQUMENT OÙ SONT SITUÉS LES RAILS
  {
    for (int y = 0; mapStation[x][y]!='\0'; y++) 
    {
      if(x+posWin.x<posTrain.x || y+posWin.y<posTrain.y || x+posWin.x>posTrain.x+hTrain || y+posWin.y>posTrain.y+wTrain) // AFFICHE UNIQUE OU LE TRAIN N'EST PAS POSITIONNÉ
        printf("\033[%d;%dH%s", x+posWin.x , y+posWin.y, search(specialChar, mapStation[x][y]));
    }
  }
}

void createPassengers(FirstPassager *firstUp, FirstPassager *firstDown, Pos posWin) // PERMET DE CREER UNE CHAINE DE PASSGER
{

	Passager *newPassagerUp = malloc(sizeof(Passager));
	newPassagerUp->pos.x = posWin.x + 3;
	newPassagerUp->pos.y = posWin.y + 83;
	newPassagerUp->next = NULL;

	Passager *newPassagerDown = malloc(sizeof(Passager));
	newPassagerDown->pos.x = posWin.x + 50;
	newPassagerDown->pos.y = posWin.y + 129;
	newPassagerDown->next = NULL;

	if(firstUp->first == NULL) // SI LA CHAINE N'EXITE PAS ON L'A CRÉE 
	{
		firstUp->first = newPassagerUp;
	}
	else // SINON ON AJOUTE JUSTE DES NOUVEAUX PASSAGERS
	{
		Passager *passagerUp = firstUp->first;
		while(passagerUp->next!=NULL)
		  passagerUp = passagerUp->next;
		passagerUp->next = newPassagerUp;
	}
	if(firstDown->first == NULL) // IDEM DE L'AUTRE COTÉ DU QUAI
	{
		firstDown->first = newPassagerDown;
	}
	else
	{
		Passager *passagerDown = firstDown->first;
		while(passagerDown->next!=NULL)
		  passagerDown = passagerDown->next;
		passagerDown->next = newPassagerDown;
	}
}

void moveDispPassengers(FirstPassager *firstUp, FirstPassager *firstDown, unsigned char **mapStation, Pos posWin) // PERMET DE FAIRE AFFICHER CHACUN DES PASSAGER APRÈS L'AVOIR DÉPLACÉ
{
	if(firstUp->first == NULL)
		printf("no Passenger up anymore\n");
	else if(firstDown->first == NULL)
		printf("no passenger down anymore\n");
	else
	{
		Passager *passagerUp = firstUp->first;
		Passager *passagerDown = firstDown->first;
		do{
		    Pos newPosUp = {passagerUp->pos.x,passagerUp->pos.y};
		    Pos newPosDown = {passagerDown->pos.x,passagerDown->pos.y};
		    fflush(stdout);
		    switch(rand()%4){ // SELECTIONNE UNE DIRECTION ALEATOIREMENT
		      case 0:
		        newPosUp.x = passagerUp->pos.x-1;
		        newPosDown.x = passagerDown->pos.x+1;
		        break;
		      case 1:
		        newPosUp.y = passagerUp->pos.y+9;
		        newPosDown.y = passagerDown->pos.y+9;
		        break;
		      case 2:
		        newPosUp.x = passagerUp->pos.x+4;
		        newPosDown.x = passagerDown->pos.x-4;
		        break;
		      case 3:
		        newPosUp.y = passagerUp->pos.y-10;
		        newPosDown.y = passagerDown->pos.y-16;
		        break;
		    }
		    //printf("trying\n");
		    if(newPosUp.x>posWin.x && newPosUp.y>posWin.y &&newPosUp.x<posWin.x+HMAP && newPosUp.y<posWin.y+WMAP && mapStation[newPosUp.x-posWin.x][newPosUp.y-posWin.y]=='t') // AFFICHE L'EMOJI AU NOUVEL EMPLACEMENT ET LE SUPPRIME A SON ANCIEN EMPLACEMENT
		    {
		      printf("\033[%d;%dH%s", passagerUp->pos.x,  passagerUp->pos.y, " ");
		      passagerUp->pos.x = newPosUp.x;
		      passagerUp->pos.y = newPosUp.y;
		      printf("\033[%d;%dH%s", passagerUp->pos.x,  passagerUp->pos.y, "😊");
		    }
		   	if(newPosDown.x>posWin.x && newPosDown.y>posWin.y &&newPosDown.x<posWin.x+HMAP && newPosDown.y<posWin.y+WMAP && mapStation[newPosDown.x-posWin.x][newPosDown.y-posWin.y]=='t') // IDEM DE L'AUTRE COTÉ
		    {
		      printf("\033[%d;%dH%s", passagerDown->pos.x,  passagerDown->pos.y, " ");
		      passagerDown->pos.x = newPosDown.x;
		      passagerDown->pos.y = newPosDown.y;
		      printf("\033[%d;%dH%s", passagerDown->pos.x,  passagerDown->pos.y, "😊");
		    }
		    usleep(50000);

		    // ON PASSE AU PASSAGER À AFFICHER DES DEUX COTÉS DU QUAI
		  	if(passagerUp->next!=NULL) 
		    	passagerUp = passagerUp->next;
			if(passagerDown->next!=NULL)
		    	passagerDown = passagerDown->next;

		}while(passagerUp->next!=NULL && passagerDown != NULL); // TANT QU'IL RESTE DES PASSAGERS A AFFICHER ET A DEPLACER
	}
}
int disappearPassengers(int numTrain, FirstPassager *firstUp, FirstPassager *firstDown, unsigned char **mapStation, Pos posWin) //PERMET DE SUPPRIMER LES PASSAGERS LORQU'UN TRAIN ARRIVE
{
	Passager *oldPassager = malloc(sizeof(Passager));
	oldPassager = NULL;

	switch(numTrain){
		case -1: // QUAI DU HAUT
			oldPassager = firstUp->first; // ON SELECTIONNE LE PREMIER PASSAGER
			if(oldPassager->next == NULL)
			{
				printf("\033[%d;%dH%s", oldPassager->pos.x,  oldPassager->pos.y, " "); // SI C'EST LE DERNIER PASSGER ON LE SUPPRIME
				firstUp->first = NULL;
				free(oldPassager);
				return 0;
			}
			else
			{
				Passager *passagerUp = oldPassager->next;
				while(passagerUp->next!=NULL) // s'IL EN RESTE ON LES SUPPRIME UN A UN EN MEME TEMPS QU'ON LES DEPLACE VERS LE TRAIN
				{
					fflush(stdout);

					Pos newPos = {passagerUp->pos.x,passagerUp->pos.y};
					oldPassager = passagerUp;
					newPos.x = passagerUp->pos.x+(4*-numTrain);
				    if(mapStation[newPos.x-posWin.x][newPos.y-posWin.y]=='t') // DEPLACEMENT
				    {
				    	printf("\033[%d;%dH%s", passagerUp->pos.x,  passagerUp->pos.y, " ");
						passagerUp->pos.x = newPos.x;
						passagerUp->pos.y = newPos.y;
					    printf("\033[%d;%dH%s", passagerUp->pos.x,  passagerUp->pos.y, "😊");
					}
					passagerUp = passagerUp->next;
					usleep(1000);
				} 
				printf("\033[%d;%dH%s", passagerUp->pos.x,  passagerUp->pos.y, " ");
				oldPassager->next = NULL; //SUPRESSION
				free(passagerUp);
				return 1;
			}
			break;
		case 1: // IDEM QUAI DU BAS
			oldPassager = firstDown->first;
			if(oldPassager->next == NULL)
			{
				printf("\033[%d;%dH%s", oldPassager->pos.x,  oldPassager->pos.y, " ");
				firstDown->first = NULL;
				free(oldPassager);
				return 0;
			}
			else
			{
				Passager *passagerDown = oldPassager->next;
				while(passagerDown->next!=NULL)
				{
					fflush(stdout);

					Pos newPos = {passagerDown->pos.x,passagerDown->pos.y};
					oldPassager = passagerDown;
					newPos.x = passagerDown->pos.x+(4*-numTrain);
				    if(mapStation[newPos.x-posWin.x][newPos.y-posWin.y]=='t')
				    {
				    	printf("\033[%d;%dH%s", passagerDown->pos.x,  passagerDown->pos.y, " ");
						passagerDown->pos.x = newPos.x;
						passagerDown->pos.y = newPos.y;
					    printf("\033[%d;%dH%s", passagerDown->pos.x,  passagerDown->pos.y, "😊");
					}
					passagerDown = passagerDown->next;
					usleep(1000);
				} 
				printf("\033[%d;%dH%s", passagerDown->pos.x,  passagerDown->pos.y, " ");
				oldPassager->next = NULL;
				free(passagerDown);
				return 1;
			}
			break;
	}
}
void trainInStation(int withPassenger, int numTrain, unsigned char **mapTrain, unsigned char **mapStation, SpeChar specialChar[], int hMapTrain, Pos posWin, FirstPassager *firstUp, FirstPassager *firstDown, Passager *passagerUp, Passager *passagerDown)
{
  Pos pos;
  int lenghtRail = 120 + WMAP; //lenght of train + railroad lenght

  //============================AJOUTER LE SON ICI============================

  switch(numTrain) //ON REGARDE LE NUM DU TRAIN QUI ARIIVE
  {
    case 1: // TRAIN DU HAUT
      pos.x = posWin.x+19; // pos train in x
      for (int i = 0; i < ((float)lenghtRail/2)+20; ++i) //TRAIN ARRIVE
      {
        pos.y = i-123;
        displayObject(mapTrain, pos, posWin, specialChar, &hMapTrain); // ON AFFICHE LE TRAIN
        displayRailOnly(mapStation, posWin, pos, specialChar); // ON REAFFCHE LES RAILS DERRIERE
        usleep(500*(i+10)*((float)i/120)); // FONCTION QUI PERMET DE DIMINUER LE DEPLACEMENT AU FUR ET A MESURE QUE LE TRAIN ARRIVE
      }

      if(withPassenger) //SI MODE PASSAGER ALORS FAIRE DISPARAITRE PASSAGE UN A UN SINON JUSTE ATTENDRE 1S ET REFAIRE PARTIR LE TRAIN
      	while(disappearPassengers(-1, firstUp, firstDown, mapStation, posWin));
      else sleep(1);
      
      for (int i = ((float)lenghtRail/2)+20; i < ((float)lenghtRail)+64; ++i)//TRAIN PART
      {
        pos.y = i-123;
        displayObject(mapTrain, pos, posWin, specialChar, &hMapTrain);
        displayRailOnly(mapStation, posWin, pos, specialChar);
        usleep((float)25000/((i-(((float)lenghtRail/2)))/((float)i/10)));
      }
      break;
    case 2:// IDEM TRAIN DU BAS
      pos.x = posWin.x+32; // pos train in x
      for (int i = ((float)lenghtRail)+120; i > ((float)lenghtRail/2)+20 ; --i)
      {
        pos.y = i-123;
        displayObject(mapTrain, pos, posWin, specialChar, &hMapTrain);
        displayRailOnly(mapStation, posWin, pos, specialChar);
        usleep((float)25000/((i-(((float)lenghtRail/2)))/((float)i/10)));
      }
      
      if(withPassenger)
      	while(disappearPassengers(1, firstUp, firstDown, mapStation, posWin) );
      else sleep(1);
      
      for (int i = ((float)lenghtRail/2)+20; i >0 ; --i)
      {
        pos.y = i - 123;
        displayObject(mapTrain, pos, posWin, specialChar, &hMapTrain);
        displayRailOnly(mapStation, posWin, pos, specialChar);
        usleep(i*100);
      }
      break;
  }
}

void drawSquare(Pos begin, struct winsize w, SpeChar specialChar[]) // PERMET D'AFFICHER LE RECTANGLE CONTENANT LA MAP
{
  for (int i = begin.y-2; i < WMAP+begin.y+2; ++i)
  {
    for (int j = begin.x-1; j < HMAP+begin.x+1; ++j)
    {
      if(i<begin.y || i>=WMAP+begin.y || j<begin.x || j>=HMAP+begin.x)
        printf("\033[%d;%dH%s", j, i, search(specialChar, 'p'));
    }
  }
}
char key_pressed() // RECUP KEY APPUYÉ 
{
	struct termios oldterm, newterm;
	int oldfd; char c, result = 0;
	tcgetattr (STDIN_FILENO, &oldterm);
	newterm = oldterm; newterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newterm);
	oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl (STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
	c = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldterm);
	fcntl (STDIN_FILENO, F_SETFL, oldfd);
	if (c != EOF) {
		ungetc(c, stdin); result = getchar();
	}
	return result;
}
int main()
{
  struct winsize w;
  time_t t;
  srand((unsigned) time(&t));
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);//get window size

  //usage of special char necessite a special table
  SpeChar specialChar[SPECHARQTY] = {'\0','\0'}; //TABLE CONTENANT LES CARARCTERE SPECIAUX AINSI QUE LEURS IDENTIFIANTS RESPECTIFS
  defineTranslator(specialChar); // PERMET D'ATTRIBUER CHAQUE CARACTERE SPECIABLE AFFICHABKE DANS LE TERMINAL À UN IDENTIFIANT LISIBLE DANS LE FICHIER
  printf("\033[2J\033[20;80HHi!\033[23;80HTo enter mode without people,   press A\033[26;80HTo enter mode with people,      press B\033[30;80HTo quit,                        press Q"); // AFFICHAGE DU MENU
	char key = 'n';
	do{
		key=key_pressed();
	}while(key != 'a' && key != 'b' && key != 'q');// ATTEND QUE L'UN DES BOUTONS DU MENU SOIT PRÉSSÉ

  printf("\e[?25l");// to hide the cursor

  //loading all the objects
  for (int i = 0; i < 100; i++) //test o
  {
    printf("%c %s \n", specialChar[i].id, specialChar[i].character);
  }

  unsigned char **mapStation; // MAP DE LA STAION 
  mapStation = (unsigned char **) malloc(sizeof(unsigned char*));
  *mapStation = (unsigned char *) malloc(sizeof(unsigned char));
  unsigned char **mapTrain1; // MAP DU TRAIN DU HAUT 
  mapTrain1 = (unsigned char **) malloc(sizeof(unsigned char*));
  *mapTrain1 = (unsigned char *) malloc(sizeof(unsigned char));
  unsigned char **mapTrain2; // MAP DU TRAIN DU BAS 
  mapTrain2 = (unsigned char **) malloc(sizeof(unsigned char*));
  *mapTrain2 = (unsigned char *) malloc(sizeof(unsigned char));
  
  int hMapStation, hMapTrain;
  printf("height attributed\n");
  mapStation = loadObject(mapStation, "station.txt", &hMapStation); //ON CHARGES LES FICHIERS DANS LES MAP
  printf("train1\n");
  mapTrain1 = loadObject(mapTrain1, "train1.txt", &hMapTrain);
  printf("train2\n");
  mapTrain2 = loadObject(mapTrain2, "train2.txt", &hMapTrain);
  printf("debug finished, 0 error\n");
  
  printf("\033[2J"); // CLEAR L'ÉCRAN
  Pos posWin;//center the window
  posWin.x = (int)((float)(w.ws_row-HMAP)); // DETERMINATION DU COIN HAUT-GAUCHE DE LA FENÊTRE D'AFFICHAGE
  posWin.y = (int)((float)(w.ws_col-WMAP)/2); 
  Pos pos = {0,0}; 
  drawSquare(posWin, w, specialChar); // PERMET D'AFFICHER LE RECTANGLE CONTENANT LA MAP DE LA STATION
  displayObject(mapStation, posWin, posWin, specialChar, &hMapStation); // AFFICHAGE DE LA STATION EN ELLE-MEME


  //ALLOCATION DU PREMIERS PASSAGER POUR LE QUAI DU HAUT ET CELUI DU BAS  
  Passager *passagerUp = malloc(sizeof(Passager));
  passagerUp->pos.x = posWin.x + 3;
  passagerUp->pos.y = posWin.y + 83;
  passagerUp->next = NULL;
  FirstPassager *firstUp = malloc(sizeof(FirstPassager));
  firstUp->first = passagerUp;

  Passager *passagerDown = malloc(sizeof(Passager));
  passagerDown->pos.x = posWin.x + 50;
  passagerDown->pos.y = posWin.y + 129;
  passagerDown->next = NULL;
  FirstPassager *firstDown  = malloc(sizeof(FirstPassager));
  firstDown->first = passagerDown;

  switch(key) // SELON LLE CHOIX DU MODE (AVEC/SANS PASSAGERS)
  {
  	case 'a' : //SANS
  		  while(1)
		  {
		    int i = 6;
		    while(i>=0) // CYCLE DE 6 TEMPS ENTRE LES DEUX TRAIN AFFICHÉS POUR QUE LES DEUX TRAINS SOIT AFFICHÉS A TROIS MINUTES D'INTERVALLES
		    {
		        fflush(stdout);
		        printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+6, posWin.y+1, i, posWin.x+4, posWin.y+1, i+6);
		        if(i>=3) // PERMET DE N'UTILISER QU'UN COMPTEUR POUR L'AFFICHAGE SUR LES DEUX PANNEAUX DES DEUX QUAIS
		          printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+46, posWin.y+175, i-3, posWin.x+48, posWin.y+175, i+3);
		        else printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+46, posWin.y+175, i+3, posWin.x+48, posWin.y+175, i+9);

		        fflush(stdout);
		        if(i==0)
		          trainInStation(0, 1, mapTrain1, mapStation, specialChar, hMapTrain, posWin, firstUp, firstDown, passagerUp, passagerDown); // AFFICHAGE DU TRAIN DU HAUT
		        if(i==3)
		          trainInStation(0, 2, mapTrain2, mapStation, specialChar, hMapTrain, posWin, firstUp, firstDown, passagerUp, passagerDown); // AFFICHAGE DU TRAIN DU BAS
		        if(i!=0 && i!=3)//TEMPS D'ATTENTE 
		          sleep(5);
		        i--;
		    }
		  }
  		break;
  	case 'b': // AVEC
		  while(1)
		  {
		    int i = 6;
		    while(i>=0)
		    {
		        fflush(stdout);
		        printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+6, posWin.y+1, i, posWin.x+4, posWin.y+1, i+6);
		        if(i>=3)
		          printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+46, posWin.y+175, i-3, posWin.x+48, posWin.y+175, i+3);
		        else printf("\033[%d;%dH%2d\033[%d;%dH%2d", posWin.x+46, posWin.y+175, i+3, posWin.x+48, posWin.y+175, i+9);

		        fflush(stdout);
		        if(i==0)
		        {
		          trainInStation(1, 1, mapTrain1, mapStation, specialChar, hMapTrain, posWin, firstUp, firstDown, passagerUp, passagerDown);
		          
		        }
		        if(i==3)
		          trainInStation(1, 2, mapTrain2, mapStation, specialChar, hMapTrain, posWin, firstUp, firstDown, passagerUp, passagerDown);
		        if(i!=0 && i!=3)//AFFICHAGE DES PASSAGERS AU FUR ET A MESURE DU TEMPS D'ATTENTE
			      for (int i = 0; i < 6; ++i)
				  {
				    createPassengers(firstUp, firstDown, posWin);
				    moveDispPassengers(firstUp, firstDown, mapStation, posWin);
				  }
		        i--;
		    }
		  }
  		break;
  	default: // ON QUITTE SI Q APPUYÉ
  		exit(-1);
  		break;
  }
  
  //displayObject(mapTrain2, pos, posWin, specialChar, &hMapTrain);
  printf("\033[%d;%dH",0,0);
  printf("\e[?25h");//re-enable cursor

  return 0;
}
