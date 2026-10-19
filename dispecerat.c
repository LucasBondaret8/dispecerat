///Bondaret Lucas 312CC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct echipaj {
    int ID;
    char tipul;
    int disponibilitatea;
}echipaj;
///vector de struct, maxim 50

typedef struct incident {
    int ID;
    char prioritate[7];
    char *descriere;
    char status[11];
    struct incident *urmator;
    struct incident *anterior;
}incident;
///lista dublu inlantuita circula cu santinela

typedef struct interventie {
    incident *incident;
    echipaj *echipaj;
    struct interventie *urmator;
    struct interventie *anterior;
}interventie;
///lista dublu inlantuita circulara cu santinela

typedef struct sistem {
    struct echipaj *echipaje;
    struct incident *incidente;
    struct interventie *interventii;
}sistem;
///optional in teorie dar bun pentru organizare, le tine pe toate intr un loc

incident* lista_goala_incidente() {
    incident *santinela = malloc(sizeof(incident));
    if(santinela != NULL) {
        santinela->urmator = santinela;
        santinela->anterior = santinela;
        santinela->ID = 0;
        strcpy(santinela->prioritate, "low");
        santinela->descriere = strdup("test incident");
        strcpy(santinela->status, "solved");
    }
    return santinela;
}
///asta i functia prin care dau nastere listei de incidente si folosesc dummy 

void adauga_lista_incidente(incident *santinela, incident *incident_adaugat) {
    strcpy(incident_adaugat->status, "queued");
    incident *ultimul_incident = santinela->anterior;
    incident_adaugat->urmator = santinela;
    incident_adaugat->anterior = ultimul_incident;
    ultimul_incident->urmator = incident_adaugat;
    santinela->anterior = incident_adaugat;
}
///iar asta e functia prin care adaug la lista de incidente, asta dupa ce am apelat mai intai functia anterioara de creat

interventie* lista_goala_interventii() {
    interventie *santinela = malloc(sizeof(interventie));
    if(santinela != NULL) {
        santinela->urmator = santinela;
        santinela->anterior = santinela;
        santinela->echipaj = NULL;
        santinela->incident = NULL;
    }
    return santinela;
}
///exact la fel ca la incidente am si pentru interventii, asta fiind functia care creeaza mai intai lista tot cu un dummy

void adauga_lista_interventii(interventie *santinela, echipaj *echipaj, incident *incident) {
    interventie *interventie_noua = (interventie *)malloc(sizeof(interventie));
    interventie_noua->echipaj = echipaj;
    interventie_noua->incident = incident;
    interventie *ultima_interventie = santinela->anterior;
    interventie_noua->urmator = santinela;
    interventie_noua->anterior = ultima_interventie;
    ultima_interventie->urmator = interventie_noua;
    santinela->anterior = interventie_noua;
}
///iar asta e functia care imi adauga interventiile in lista deja existenta

typedef struct coada_incidente {
    incident *incident_actual;
    struct coada_incidente *incidentul_urmator;
}coada_incidente;
///struct de coada pentru incidente, necesar pentru cozile de prioritati gen low medium high

typedef struct coada_echipaj {
    echipaj *echipaj_actual;
    struct coada_echipaj *echipajul_urmator;
}coada_echipaj;
///struct de coada pentru echipaj, pentru echipaje am doar o coada si buna

typedef struct stiva {
    interventie *interventia_actuala;
    struct stiva *interventia_urmatoare;
}stiva;
///struct de stiva pentru comanda de UNDO

coada_echipaj *adauga_echipaj_coada(coada_echipaj *coada_echipaje, echipaj *echipaj_adaugat) {
    coada_echipaj *echipaj_nou = malloc(sizeof(coada_echipaj));
    echipaj_nou->echipajul_urmator = NULL;
    echipaj_nou->echipaj_actual = echipaj_adaugat;
    if(coada_echipaje == NULL) {
        return echipaj_nou;
    }
    else {
        coada_echipaj *temporar = coada_echipaje;
        while(temporar->echipajul_urmator != NULL) {
            temporar = temporar->echipajul_urmator;
        }
        temporar->echipajul_urmator = echipaj_nou;
        echipaj_nou -> echipajul_urmator = NULL;
    }
    return coada_echipaje;
}
///functie care primeste coada si echipajul nou, nimic prea special doar adauga in coada

coada_incidente *adauga_incident_coada(coada_incidente *coada_incident, incident *incident_adaugat) {
    coada_incidente *incident_nou = malloc(sizeof(coada_incidente));
    incident_nou->incidentul_urmator = NULL;
    incident_nou->incident_actual = incident_adaugat;
    if(coada_incident == NULL) {
        return incident_nou;
    }
    else {
        coada_incidente *temporar = coada_incident;
        while(temporar->incidentul_urmator != NULL) {
            temporar = temporar->incidentul_urmator;
        }
        temporar->incidentul_urmator = incident_nou;
        incident_nou -> incidentul_urmator = NULL;
    }
    return coada_incident;
}
///la fel si pentru incident, doar o functie care primeste coada si incidentul pe care tre sa l adaug

int main() {
    sistem *sistem = malloc(sizeof(struct sistem));///Zona de alocare si initializare din prima faza
    sistem->incidente = lista_goala_incidente();
    sistem->interventii = lista_goala_interventii();
    coada_incidente *high = NULL;
    coada_incidente *medium = NULL;
    coada_incidente *low = NULL;
    coada_echipaj *coada_echipaje = NULL;
    stiva *stiva_dispatch_undo = NULL;
    int nr_echipaje;///Zona de citire si adaugare in coada a echipajelor
    scanf("%d", &nr_echipaje);
    sistem->echipaje = malloc(nr_echipaje * sizeof(echipaj));
    for(int i = 0; i < nr_echipaje; i++) {
        int ID;
        char tip;
        scanf("%d %c", &ID, &tip);
        sistem->echipaje[i].ID = ID;
        sistem->echipaje[i].tipul = tip;
        sistem->echipaje[i].disponibilitatea = 1;
        coada_echipaje = adauga_echipaj_coada(coada_echipaje, &sistem->echipaje[i]);
    }
    int nr_comenzi;///Zona de citire si executare a comenzilor, practic aicia i cam toata crema codului
    scanf("%d", &nr_comenzi);
    for(int i = 0; i < nr_comenzi; i++) {
        char comanda[50] = {0};
        scanf("%s", comanda);
        if(strcmp(comanda, "ADD_INCIDENT") == 0) {
            ///Sa va spun sincer mi se pare cam mult de explicat asa ca de aici incolo o sa ma duc in README si o sa zic cam tot ce se intampla
            int ID;
            char prioritate[7] = {0};
            scanf("%d %s", &ID, prioritate);
            char ghilimea;
            scanf(" %c", &ghilimea);
            char temporar[5000] = {0};
            char caracter;
            int j = 0;
            while(1) {
                scanf("%c", &caracter);
                if(caracter == '"') {
                    break;
                }
                else {
                    temporar[j] = caracter;
                    j++;
                }
            }
            temporar[j] = '\0';
            char *descriere = malloc((strlen(temporar) + 1) * sizeof(char));
            strcpy(descriere, temporar);
            incident *incident_temporar = malloc(sizeof(incident));
            incident_temporar->ID = ID;
            strcpy(incident_temporar->prioritate, prioritate);
            incident_temporar->descriere = descriere;
            adauga_lista_incidente(sistem->incidente, incident_temporar);
            if(strcmp(incident_temporar->prioritate, "high") == 0)
                high = adauga_incident_coada(high, incident_temporar);
             if(strcmp(incident_temporar->prioritate, "medium") == 0)
                medium = adauga_incident_coada(medium, incident_temporar);
             if(strcmp(incident_temporar->prioritate, "low") == 0)
                low = adauga_incident_coada(low, incident_temporar);
        }
        else if(strcmp(comanda, "DISPATCH") == 0) {
            if(coada_echipaje == NULL ||
            (high == NULL && medium == NULL && low == NULL)) {
                printf("INVALID OPERATION! ERROR 404\n");
            }
            else {
                echipaj *echipaj_dispatch = coada_echipaje->echipaj_actual;
                incident *incident_dispatch = NULL;
                if(high != NULL) {
                    incident_dispatch = high->incident_actual;
                }
                else if(medium != NULL) {
                    incident_dispatch = medium->incident_actual;
                }
                else {
                    incident_dispatch = low->incident_actual;
                }
            
            adauga_lista_interventii(sistem->interventii, echipaj_dispatch, incident_dispatch);
            echipaj_dispatch->disponibilitatea = 0;
            strcpy(incident_dispatch->status, "intervened");
            incident *temporar = sistem->incidente->urmator;
            while(temporar != sistem->incidente) {
                if(temporar->ID == incident_dispatch->ID) {
                    strcpy(temporar->status, "intervened");
                    break;
                }
                temporar = temporar->urmator;
            }
            coada_echipaj *echipaj_anterior = coada_echipaje;
            coada_echipaje = coada_echipaje->echipajul_urmator;
            free(echipaj_anterior);
            if(strcmp(incident_dispatch->prioritate, "high") == 0) {
                coada_incidente *incident_anterior = high;
                high = high->incidentul_urmator;
                free(incident_anterior);
            }
            if(strcmp(incident_dispatch->prioritate, "medium") == 0) {
                coada_incidente *incident_anterior = medium;
                medium = medium->incidentul_urmator;
                free(incident_anterior);
            }
            if(strcmp(incident_dispatch->prioritate, "low") == 0) {
                coada_incidente *incident_anterior = low;
                low = low->incidentul_urmator;
                free(incident_anterior);
            }
            stiva *top_nou = malloc(sizeof(stiva));
            top_nou->interventia_actuala = sistem->interventii->anterior;
            top_nou->interventia_urmatoare = stiva_dispatch_undo;
            stiva_dispatch_undo = top_nou;
        }
        }
        else if(strcmp(comanda, "UNDO_LAST_DISPATCH") == 0) {
            stiva *interventia_cautata = stiva_dispatch_undo;
            stiva *temporar = stiva_dispatch_undo;
            stiva *anterior = NULL; 
            while(temporar != NULL) {
                interventia_cautata = temporar;
                if(strcmp(interventia_cautata->interventia_actuala->incident->status, "intervened") == 0) {
                    interventia_cautata = temporar;
                    break;
                }
                else {
                    anterior = temporar;
                    temporar = temporar->interventia_urmatoare;
                }
                
            }
            if(interventia_cautata == NULL || strcmp(interventia_cautata->interventia_actuala->incident->status, "intervened") != 0)
                printf("INVALID OPERATION! ERROR 404\n");
            else {
                interventia_cautata->interventia_actuala->echipaj->disponibilitatea = 1;
                coada_echipaje = adauga_echipaj_coada(coada_echipaje, interventia_cautata->interventia_actuala->echipaj);
                strcpy(interventia_cautata->interventia_actuala->incident->status, "queued");
                incident *temporar = sistem->incidente->urmator;
                while(temporar != sistem->incidente) {
                    if(temporar->ID == interventia_cautata->interventia_actuala->incident->ID) {
                        strcpy(temporar->status, "queued");
                        break;
                    }
                    else
                        temporar = temporar->urmator;
                }
                coada_incidente *incident_pus_fata = malloc(sizeof(coada_incidente));
                incident_pus_fata->incident_actual = interventia_cautata->interventia_actuala->incident;
                if(strcmp(interventia_cautata->interventia_actuala->incident->prioritate, "high") == 0) {
                    incident_pus_fata->incidentul_urmator = high;
                    high = incident_pus_fata;
                }
                if(strcmp(interventia_cautata->interventia_actuala->incident->prioritate, "medium") == 0) {
                    incident_pus_fata->incidentul_urmator = medium;
                    medium = incident_pus_fata;
                }
                if(strcmp(interventia_cautata->interventia_actuala->incident->prioritate, "low") == 0) {
                    incident_pus_fata->incidentul_urmator = low;
                    low = incident_pus_fata;
                }
                interventia_cautata->interventia_actuala->anterior->urmator = interventia_cautata->interventia_actuala->urmator;
                interventia_cautata->interventia_actuala->urmator->anterior = interventia_cautata->interventia_actuala->anterior;
                if(anterior == NULL)
                    stiva_dispatch_undo = stiva_dispatch_undo->interventia_urmatoare;
                else
                    anterior->interventia_urmatoare = interventia_cautata->interventia_urmatoare;
                free(interventia_cautata->interventia_actuala);
                free(interventia_cautata);
            }
        }
        else if(strcmp(comanda, "SOLVED_INCIDENT") == 0) {
            int ID;
            scanf("%d", &ID);
            incident *temporar = sistem->incidente->urmator;
            while(temporar != sistem->incidente)
                if(temporar->ID == ID)
                    break;
                else
                    temporar = temporar->urmator;
            if(strcmp(temporar->status, "intervened") == 0) {
                strcpy(temporar->status, "solved");
                interventie *interventie_temporara = sistem->interventii->urmator;
                while(interventie_temporara != sistem->interventii) {
                    if(interventie_temporara->incident->ID == ID) {
                        interventie_temporara->echipaj->disponibilitatea = 1;
                        coada_echipaje = adauga_echipaj_coada(coada_echipaje, interventie_temporara->echipaj);
                        strcpy(interventie_temporara->incident->status, "solved");
                    }
                    interventie_temporara = interventie_temporara->urmator;
                }
            }
            else
                printf("INVALID OPERATION! ERROR 404\n");
        }
        else if(strcmp(comanda, "SHOW_INCIDENT") == 0) {
            int ID;
            scanf("%d", &ID);
            incident *temporar = sistem->incidente->urmator;
            while(temporar != sistem->incidente)
                if(temporar->ID == ID)
                    break;
                else
                    temporar = temporar->urmator;
            if(temporar == sistem->incidente)
                printf("INVALID OPERATION! ERROR 404\n");
            else
                printf("Incident %d has %s priority, the following description: \"%s\" and is %s\n", temporar->ID, temporar->prioritate, temporar->descriere, temporar->status);
        }
        else if(strcmp(comanda, "SHOW_UNIT") == 0) {
            int ID;
            scanf("%d", &ID);
            int gasit = 0;
            for(int i = 0; i < nr_echipaje; i++) {
                if(sistem->echipaje[i].ID == ID) {
                    if(sistem->echipaje[i].disponibilitatea == 0)
                        printf("Unit %d is type %c and is unavailable\n", sistem->echipaje[i].ID, sistem->echipaje[i].tipul);
                    else
                        printf("Unit %d is type %c and is available\n", sistem->echipaje[i].ID, sistem->echipaje[i].tipul);
                    gasit = 1;
                    break;
                }
            }
            if(gasit == 0)
                printf("INVALID OPERATION! ERROR 404\n");
        }
        else if(strcmp(comanda, "CHECK_UNITS_AVAILABILITY") == 0) {
            int nr_disponibile = 0;
            for(int i = 0; i < nr_echipaje; i++) {
                if(sistem->echipaje[i].disponibilitatea == 1)
                    nr_disponibile++;
            }
            printf("Number of available units: %d\n", nr_disponibile);
        }
        else if(strcmp(comanda, "SHOW_INTERVENTIONS") == 0) {
            interventie *temporar = sistem->interventii->urmator;
            if(temporar == sistem->interventii)
                printf("No intervention has been initiated\n");
            else
                while(temporar != sistem->interventii) {
                    printf("Incident %d was assigned to unit %d, and has the following status: \"%s\"\n", temporar->incident->ID, temporar->echipaj->ID, temporar->incident->status);
                    temporar = temporar->urmator;
                }
        }
        }
        while(high != NULL) {
            coada_incidente *temporar = high->incidentul_urmator;
            free(high);
            high = temporar;
        }
        while(medium != NULL) {
            coada_incidente *temporar = medium->incidentul_urmator;
            free(medium);
            medium = temporar;
        }
        while(low != NULL) {
            coada_incidente *temporar = low->incidentul_urmator;
            free(low);
            low = temporar;
        }
        while(coada_echipaje != NULL) {
            coada_echipaj *temporar = coada_echipaje->echipajul_urmator;
            free(coada_echipaje);
            coada_echipaje = temporar;
        }
        while(stiva_dispatch_undo != NULL) {
            stiva *temporar = stiva_dispatch_undo->interventia_urmatoare;
            free(stiva_dispatch_undo);
            stiva_dispatch_undo = temporar;
        }
        interventie *temporar = sistem->interventii->urmator;
        while(temporar != sistem->interventii) {
            interventie *next = temporar->urmator;
            free(temporar);
            temporar = next;
        }
        free(sistem->interventii);
        incident *temporar2 = sistem->incidente->urmator;
        while(temporar2 != sistem->incidente) {
            incident *next = temporar2->urmator;
            free(temporar2->descriere);
            free(temporar2);
            temporar2 = next;
        }
        free(sistem->incidente->descriere);
        free(sistem->incidente);
        free(sistem->echipaje);
        free(sistem);
    }
