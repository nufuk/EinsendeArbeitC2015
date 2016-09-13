/* Programm: Ein Abrechnungssytem fuer einen Internetprovider
** Einsendearbeit zum Kurz Programmieren in C, SoSe 2015
** Autor: Kurz, Norbert, 
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef enum {true, false} bool;

typedef struct {
	char tag[5];
	char monat[3];
	char jahr[3];
	char gesamtDatum[12]; //tt.mm.yyyy
} datum;

typedef struct {
	int tag;
	int laufendeNummer;
	char verbindungsdauer[10];
} verbindungsListe;

typedef struct element *ELEM_ZGR;      /* Zeigertyp fuer Stack-Element */
struct element                         /* Knotentyp fuer Stack-Element */
{
    verbindungsListe    verbindung;      /* einziges Nutzdatum */
    ELEM_ZGR       		next;      /* Zeiger auf naechstes Stack-Element */
};
static ELEM_ZGR first;    

typedef struct  {
	char kundennummer[10]; 
	char vorname[32];
	char nachname[32];
	char geburtstag[10];
	datum kundeSeit;
	float preisProMinute;
	float grundGebuehr;
	bool einzelNachweis;
	ELEM_ZGR ersteVerbindung;
} kunde;

//Suchbaum
typedef struct knoten *KNOTEN_ZGR;

struct knoten {
	kunde kundenEintrag;
	KNOTEN_ZGR left;
	KNOTEN_ZGR right;
};
KNOTEN_ZGR root;

void init_tree(void);
void show_tree(int order);
void show_tree_in(KNOTEN_ZGR curr, int start);
void show_tree_level(void);
void traverse_level(int target, int ncurr, KNOTEN_ZGR curr);
KNOTEN_ZGR sucheKundeVerbindung (KNOTEN_ZGR curr, int start, char kdnr[]);
void writeToFileKunde(KNOTEN_ZGR curr, FILE *fp, int start);
void writeToFileVerbindung(ELEM_ZGR verb, FILE *fp);
bool validiereEingabe() {
//Uebergebe im String was fuer ein Typ es sein muss und ueberpruefe ob dies der Fall ist.
}

bool kundenNummerBelegt(char kdnr[]){
	verbindungsListe liste;
	KNOTEN_ZGR kunde;
	kunde = sucheKundeVerbindung(root,1,kdnr);
	if(kunde != NULL) {
		printf("Diese Kundenummer existiert bereits, bitte noch mal versuchen\n");
		return false; 
	}
	return true;
}

void personErfassen() {
	kunde neuerKunde;
	KNOTEN_ZGR temp, curr; 

    /* neues Element im Heap allokieren und fuellen */	
	temp = (KNOTEN_ZGR)malloc(sizeof(struct knoten));
	if (temp == NULL) { printf("nicht initialisiert");}                    /* wenn malloc versagt abbrechen */
    
	printf("\n");
	printf("Bitte geben Sie den Vornamen des Kunden ein und bestaetigen Sie mit <Enter> \n");
	scanf("%s", &neuerKunde.vorname);
	printf("Bitte geben Sie den Nachnamen des Kunden ein und bestaetigen Sie mit <Enter> \n");
	scanf("%s", &neuerKunde.nachname);
	printf("Bitte geben Sie den Minutenpreis des Kunden ein im Format 0.00 (ohne Waehrungszeichen) und bestaetigen Sie mit <Enter> \n");
	scanf("%f", &neuerKunde.preisProMinute);
	printf("Bitte geben Sie die monatliche Grundgebuehr des Kunden ein im Format 0.00 (ohne Waehrungszeichen) und bestaetigen Sie mit <Enter> \n");
	scanf("%f", &neuerKunde.grundGebuehr);
	printf("Wuenscht der Kunde Einzelnachweise? 0 fuer ja, 1 fuer nein <Enter> \n");
	do {
		scanf("%d", &neuerKunde.einzelNachweis);
	} while(neuerKunde.einzelNachweis != 0 &&  neuerKunde.einzelNachweis != 1);
	
	
	printf("Kundennummer eintragen: <Enter> \n");
	do {
		scanf("%s", &neuerKunde.kundennummer);
	} while(kundenNummerBelegt(neuerKunde.kundennummer));

	printf("\nVielen Dank, Sie haben: %s eingetippt\n", neuerKunde.vorname);
	neuerKunde.ersteVerbindung = NULL;
	
	temp->kundenEintrag = neuerKunde;                  
    temp->left = temp->right = NULL;
	
	if (root == NULL) {
        root = temp;
    } else {
        for (curr = root;;) {
			//Check welche Kundenummer kleiner ist
			if (strcmp(neuerKunde.kundennummer,curr->kundenEintrag.kundennummer) < 0) {
                if  (curr->left != NULL) {
                     curr = curr->left; 
                } else {
                     curr->left = temp; 
                     break;
                }
            } else {
                if  (curr->right != NULL) {
                    curr = curr->right;
                } else {
                    curr->right = temp;
                    break;
                }
            }
        }
    }
}

void show_tree(int order) {
    printf("\nAktueller binaerer Suchbaum / ");
    switch(order) {
        case 0: printf("Symmetrische Ordnung (sortiert)\n");
                show_tree_level(); break;
        case 1: printf("Praeordnung, Teilbaum-Wurzel zuerst\n");
                show_tree_in(root,1); break;
        case 2: printf("Postordnung, Teilbaum-Wurzel zuletzt\n");
                show_tree_in(root,1); break;
        case 3: printf("Niveauordnung, pro Niveau von links nach rechts\n");
                show_tree_in(root,1); break;
    }
}

void show_tree_in(KNOTEN_ZGR curr, int start) {
    static int i;
    i = start ? 0 : i;
    if (curr != NULL) {
        show_tree_in(curr->left,0);
        if (++i%15 == 1 && i > 1) {
            printf("\n");
		}
		printf("%s\n", curr->kundenEintrag.vorname);
		printf("%s\n", curr->kundenEintrag.nachname);
		printf("%.2f\n", curr->kundenEintrag.preisProMinute);
		printf("%.2f\n", curr->kundenEintrag.grundGebuehr);
		printf("%s\n", curr->kundenEintrag.kundennummer);
        show_tree_in(curr->right,0);     
    }
}

int level_nodes_found; 
void show_tree_level(void) {
    int target_level; 

    printf("\nNiveau  Knoten/KdNr  (linker Nachfolger, rechter Nachfolger) (# = ohne Nachfolger)");
    printf("\n----------------------------------------------------------------------------------");

    for(target_level = 0;1;){    
        level_nodes_found = 0;  
        traverse_level(target_level, 0, root);

        if(!level_nodes_found) { 
            break;              
        }
		target_level++;         
    }
}

void traverse_level(int target, int ncurr, KNOTEN_ZGR curr) {
    if (curr == NULL) {
        return;
	}
    else if (ncurr == target){       
        if (level_nodes_found%6==0) {  
            printf("\n%3d     ",target);
		}
        level_nodes_found++;         

        printf("%s(",curr->kundenEintrag.kundennummer);   
        if (curr->left != NULL) {     
            printf("%s,",curr->left->kundenEintrag.kundennummer);
		} else {
            printf(" #,");
		}
        if (curr->right != NULL) {
            printf("%s); ",curr->right->kundenEintrag.kundennummer);
		} else {
            printf(" #); ");
		}
    }
    else if (ncurr < target) {         
        traverse_level(target, ncurr+1, curr->left);  
        traverse_level(target, ncurr+1, curr->right);  
    }
}

void verbindungErfassen() {
	verbindungsListe liste;
	KNOTEN_ZGR kunde;
	ELEM_ZGR temp, curr, iter;

	temp = (ELEM_ZGR)malloc(sizeof(struct element));
	if (temp == NULL) { printf("nicht initialisiert");}
	
	char kdnr[10];
	printf("\nHier koennen Sie eine neue Verbindung eintippen\n");
	printf("Bitte geben Sie dazu die Kennung des Kunden an\n");
	scanf("%s", &kdnr);
	kunde = sucheKundeVerbindung(root,1,kdnr);
	if(kunde == NULL) {
		printf("\nEs gibt kein Kunde mit dieser Nummer\n");
		return ;
	}
	curr = kunde->kundenEintrag.ersteVerbindung;
	
	printf("Bitte geben Sie den Monatstag an\n");
	scanf("%d", &liste.tag);
	printf("Bitte geben Sie die Verbindungsdauer an im Format 0:00\n");
	scanf("%s", &liste.verbindungsdauer);
	
	temp->verbindung = liste;                  
    temp->next = NULL;

    if (curr == NULL) {
		temp->verbindung.laufendeNummer = 1;
        kunde->kundenEintrag.ersteVerbindung = temp;
    } else {
        for (iter = curr;;) {
			if(iter->next != NULL) {
				iter = iter->next;
			} else {
				temp->verbindung.laufendeNummer = iter->verbindung.laufendeNummer + 1;
				iter->next = temp;
				break;
			}
        }
    } 
}

void verbindungenAusgeben() {
	verbindungsListe liste;
	KNOTEN_ZGR kunde;
	ELEM_ZGR temp, curr, iter;

	temp = (ELEM_ZGR)malloc(sizeof(struct element));
	if (temp == NULL) { printf("nicht initialisiert");}
	
	char kdnr[10];
	printf("\nHier koennen Sie eine neue Verbindung eintippen\n");
	printf("Bitte geben Sie dazu die Kennung des Kunden an\n");
	scanf("%s", &kdnr);
	kunde = sucheKundeVerbindung(root,1,kdnr);
	if(kunde == NULL) {
		printf("\nEs gibt kein Kunde mit dieser Nummer\n");
		return ;
	}
	curr = kunde->kundenEintrag.ersteVerbindung;	
	temp->verbindung = liste;                  
    temp->next = NULL;

    if (curr == NULL) {
		printf("\nDieser Kunde hat keine Verbindungen\n");
    } else {
        for (iter = curr;;) {
			printf("\nDieser Kunde hat folgende Verbindungen\n");
			if(iter->next != NULL) {
				printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
				printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
				printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
				iter = iter->next;
			} else {
				printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
				printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
				printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
				break;
			}
        }
    } 
}

void kundeAusgeben() {
	verbindungsListe liste;
	KNOTEN_ZGR kunde;
	ELEM_ZGR temp, curr, iter;

	temp = (ELEM_ZGR)malloc(sizeof(struct element));
	if (temp == NULL) { printf("nicht initialisiert");}
	
	char kdnr[10];
	printf("Bitte geben Sie dazu die Kundennummer an\n");
	printf("um mehr Informationen zu bekommen\n");
	scanf("%s", &kdnr);
	kunde = sucheKundeVerbindung(root,1,kdnr);
	if(kunde == NULL) {
		printf("\nEs gibt kein Kunde mit dieser Nummer\n");
		return ;
	}
	printf("Kundennummer: %s\n", kunde->kundenEintrag.kundennummer);
	printf("Vorname: %s\n",kunde->kundenEintrag.vorname);
	printf("Nachname: %s\n",kunde->kundenEintrag.nachname);
	printf("Preis Pro Einheit: %.2f\n",kunde->kundenEintrag.preisProMinute);
	printf("Grundgebuehr: %.2f\n",kunde->kundenEintrag.grundGebuehr);
	printf("Einzelnachweis %d\n", kunde->kundenEintrag.einzelNachweis);
	
	curr = kunde->kundenEintrag.ersteVerbindung;	
	temp->verbindung = liste;                  
    temp->next = NULL;

    if (curr == NULL) {
		printf("\nDieser Kunde hat keine Verbindungen\n");
    } else {
        for (iter = curr;;) {
			printf("\nDieser Kunde hat folgende Verbindungen\n");
			if(iter->next != NULL) {
				printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
				printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
				printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
				iter = iter->next;
			} else {
				printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
				printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
				printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
				break;
			}
        }
    } 
}

KNOTEN_ZGR sucheKundeVerbindung(KNOTEN_ZGR curr, int start, char kdnr[]) {
	KNOTEN_ZGR ret;
	ret = NULL;
    if (curr != NULL) {
        ret = sucheKundeVerbindung(curr->left,0,kdnr);      
		if(strcmp(kdnr,curr->kundenEintrag.kundennummer) == 0) {
			return curr;
		}
        ret = sucheKundeVerbindung(curr->right,0,kdnr);     
		return ret;
    }
	return ret;
}

void baumAnzeigen(){
	show_tree(0);
}

void rechnungErstellen() {
	verbindungsListe liste;
	KNOTEN_ZGR kunde;
	ELEM_ZGR temp, curr, iter;
	temp = (ELEM_ZGR)malloc(sizeof(struct element));
	if (temp == NULL) { printf("nicht initialisiert");}
	
	char kdnr[10];
	printf("Bitte Geben Sie die KdNummer ein fuer wen\n");
	printf("eine Rechnung erstellt werden soll\n");
	scanf("%s", &kdnr);
	kunde = sucheKundeVerbindung(root,1,kdnr);
	if(kunde == NULL) {
		printf("\nEs gibt kein Kunde mit dieser Nummer\n");
		return ;
	}
	
	if(kunde->kundenEintrag.einzelNachweis == 0) {
		printf("Dieser Kunde wuenschte Einzelnachweise\n");
	} else {
		printf("Dieser Kunde wuenschte keine Einzelnachweise\n");
	}
	printf("Vorname: %s\n",kunde->kundenEintrag.kundennummer);
	printf("Vorname: %s\n",kunde->kundenEintrag.vorname);
	printf("Nachname: %s\n",kunde->kundenEintrag.nachname);
	printf("Preis Pro Einheit: %.2f\n",kunde->kundenEintrag.preisProMinute);
	printf("Grundgebuehr: %.2f\n",kunde->kundenEintrag.grundGebuehr);
	
	curr = kunde->kundenEintrag.ersteVerbindung;	
	temp->verbindung = liste;                  
    temp->next = NULL;

	int minuten, sekunden;
	float preis = 0.0f;
	preis = preis + kunde->kundenEintrag.grundGebuehr;
    if (curr == NULL) {
		printf("\nDieser Kunde hat keine Verbindungen\n");
    } else {
        for (iter = curr;;) {
			printf("\nDieser Kunde hat folgende Verbindungen\n");
			sscanf(iter->verbindung.verbindungsdauer,"%d:%d",&minuten,&sekunden);
			if(iter->next != NULL) {
				if(kunde->kundenEintrag.einzelNachweis == 0) {
					printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
					printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
					printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
					float kosten = (minuten * kunde->kundenEintrag.preisProMinute) + ((sekunden / 60.0f) * kunde->kundenEintrag.preisProMinute);
					printf("Kosten fuer diesen Anruf: %.2f\n", kosten);
				}	
				preis = preis + (minuten * kunde->kundenEintrag.preisProMinute) + ((sekunden / 60) * kunde->kundenEintrag.preisProMinute);
				iter = iter->next;
			} else {
				if(kunde->kundenEintrag.einzelNachweis == 0) {
					printf("Verbindungsnummer: %d \n", iter->verbindung.laufendeNummer);
					printf("An folgendem Tag im Monat: %d \n", iter->verbindung.tag);
					printf("Verbindungsdauer: %s \n", &iter->verbindung.verbindungsdauer);
					float kosten = (minuten * kunde->kundenEintrag.preisProMinute) + ((sekunden / 60.0f) * kunde->kundenEintrag.preisProMinute);
					printf("Kosten fuer diesen Anruf: %.2f\n", kosten);
				}	
				preis = preis + (minuten * kunde->kundenEintrag.preisProMinute) + ((sekunden / 60) * kunde->kundenEintrag.preisProMinute);
				break;
			}
        }
    } 
	printf("Monatskosten: %.2f\n", preis);
}

void datenSpeichern() {
	printf("\n#### Speichert #####\n");
	char fileName[64] = "kunden.txt";
	FILE *fp;

	remove(fileName);
	if((fp = fopen(fileName, "a+")) == NULL) {
		printf("Die Datei kann nicht geoeffnet werden\n");
		exit(1);
	}

	writeToFileKunde(root, fp, 0);
	fclose(fp);
	printf("\n#### Speichern abgeschlossen #####\n");
}

void writeToFileKunde(KNOTEN_ZGR curr, FILE *fp, int start) {                 
    if (curr != NULL) {                    
        writeToFileKunde(curr->left,fp,0);      
		fprintf(fp, "kunde ");
		fprintf(fp, "%s ", curr->kundenEintrag.vorname);
		fprintf(fp, "%s ", curr->kundenEintrag.nachname);
		fprintf(fp, "%.2f ", curr->kundenEintrag.preisProMinute);
		fprintf(fp, "%.2f ", curr->kundenEintrag.grundGebuehr);
		fprintf(fp, "%s ", curr->kundenEintrag.kundennummer);
		fprintf(fp, "%d\n", curr->kundenEintrag.einzelNachweis);
		writeToFileVerbindung(curr->kundenEintrag.ersteVerbindung,fp);
        writeToFileKunde(curr->right,fp,0);     
    }
}
void writeToFileVerbindung(ELEM_ZGR verb, FILE *fp) {
	ELEM_ZGR curr;
    if (verb != NULL) {
        for (curr = verb;;) {
			if(curr->next != NULL) {
				fprintf(fp,"verbindungen ");
				fprintf(fp,"%d ", curr->verbindung.laufendeNummer);
				fprintf(fp,"%s ", &curr->verbindung.verbindungsdauer);
				fprintf(fp,"%d\n", curr->verbindung.tag);
				curr = curr->next;
			} else {
				fprintf(fp,"verbindungen ");
				fprintf(fp,"%d ", curr->verbindung.laufendeNummer);
				fprintf(fp,"%s ", &curr->verbindung.verbindungsdauer);
				fprintf(fp,"%d\n", curr->verbindung.tag);
				break;
			}
        }
    } 
}
void initTree() {
	root = NULL;
	kunde neuerKunde;
	char kdnr[10];
	char wert1[32],wert2[32],wert3[32],wert6[10];
	float wert4, wert5;
	int wert7;
	printf("\n#### Laden #####\n");
	FILE *fp;
	if((fp = fopen("kunden.txt", "r")) == NULL) {
		printf("Die Datei kann nicht geoeffnet werden\n");
		exit(1);
	}
	
	while((fscanf(fp,"%s %s %s %f %f %s %d\n",&wert1,&wert2,&wert3,&wert4,&wert5,&wert6,&wert7)) != EOF ) {
		if(strcmp("kunde", wert1) == 0) {
			strcpy(neuerKunde.vorname, wert2);
			strcpy(neuerKunde.nachname, wert3);
			neuerKunde.preisProMinute = wert4; 
			neuerKunde.grundGebuehr = wert5;
			strcpy(neuerKunde.kundennummer, wert6);	
			strcpy(kdnr, wert6);	
			neuerKunde.einzelNachweis = wert7;

			KNOTEN_ZGR curr, temp;
			temp = (KNOTEN_ZGR)malloc(sizeof(struct knoten));
			if (temp == NULL) { printf("nicht initialisiert");} 
			neuerKunde.ersteVerbindung = NULL;
			temp->kundenEintrag = neuerKunde;                  
			temp->left = temp->right = NULL;
			//printf("\ntext:%s\nVorname: %s\nNachname: %s\npreis: %.2f\ngebuehr: %.2f\nKdNr %s\nEinzelnachweis: %d\n",wert1,neuerKunde.vorname,neuerKunde.nachname,neuerKunde.preisProMinute,neuerKunde.grundGebuehr,neuerKunde.kundennummer,neuerKunde.einzelNachweis);
			if (root == NULL) {
				root = temp;
			} else {
				for (curr = root;;) {
					//Check welche Kundenummer kleiner ist
					if (strcmp(neuerKunde.kundennummer,curr->kundenEintrag.kundennummer) < 0) {
						if  (curr->left != NULL) {
							curr = curr->left; 
						} else {
							curr->left = temp;
							break;
						}
					} else {
						if  (curr->right != NULL) {
							curr = curr->right;
						} else {
							curr->right = temp;
							break;
						}
					}
				}
			}
		}
		if(strcmp("verbindungen", wert1) == 0) {
			verbindungsListe liste;
			KNOTEN_ZGR kunde;
			ELEM_ZGR temp, curr, iter;

			temp = (ELEM_ZGR)malloc(sizeof(struct element));
			if (temp == NULL) { printf("nicht initialisiert");}
			
			kunde = sucheKundeVerbindung(root,1,kdnr);
			curr = kunde->kundenEintrag.ersteVerbindung;
			
			strcpy(liste.verbindungsdauer, wert3);
			liste.tag = wert4;
			temp->verbindung = liste;                  
			temp->next = NULL;
			if (curr == NULL) {
				temp->verbindung.laufendeNummer = 1;
				kunde->kundenEintrag.ersteVerbindung = temp;
			} else {
				for (iter = curr;;) {
					if(iter->next != NULL) {
						iter = iter->next;
					} else {
						temp->verbindung.laufendeNummer = iter->verbindung.laufendeNummer + 1;
						iter->next = temp;
						break;
					}
				}
			}
		}
	}
	fclose(fp);
}

int main(void) {
	initTree();
	int auswahl = 0;
	while(auswahl != 7) {
		printf("\nInternetprovide 3000 Verwaltungssystem\n");
		printf("Bitte waehlen Sie aus den folgenden Optionen und bestaetigen Sie mit <Enter>\n");
		printf("1 Person erfassen\n");
		printf("2 Verbindung erfassen\n");
		printf("3 Rechnung erstellen\n");
		printf("4 Baum anzeigen mehr machen\n");
		printf("5 Verbindung eines Kunden anzeigen\n");
		printf("6 Person suchen und Anzeigen\n");
		printf("7 Programm beenden \n");
		scanf("%d", &auswahl);
		//Switch zur Auswahl
		switch(auswahl) {
			case 1: 	personErfassen(); break;
			case 2: 	verbindungErfassen(); break;
			case 3: 	rechnungErstellen(); break;
			case 4: 	baumAnzeigen(); break;
			case 5:		verbindungenAusgeben();break;
			case 6:		kundeAusgeben();break;
			case 7: 	printf("Auf wiedersehen und einen schoenen Tag noch\n"); datenSpeichern(); break;
			default:	printf("Falsche Auswahl, bitte nur zwischen 1 und 5\n"); break;
		}
	}
	return 0;
}