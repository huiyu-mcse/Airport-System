/*
 * File:  proj2.c
 * Author:  Hui Yu 103412
 * Description: project 2.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_AIRPORT 40  /* número máximo de areoportos */
#define MAX_FLIGHTS 30000   /* número máximo de voos */

#define DIM_ID_AIRPORT 4    /* dimensão de ID do aeroporto */
#define DIM_ID_FLIGHT 7    /* dimensão máximo de ID do voo */
#define DIM_COUNTRY 31  /* dimensão máximo de nome de pais */
#define DIM_CITY 51    /* dimensão máximo de nome de cidade */
#define DIM_HASH_T 21841  /* dimensão do hashtable */

#define TRUE 1        /* verdadeiro */
#define FALSE 0			/* falso */
#define N_EXIST -1       /*não existe*/

/* Tipos de Dados */

typedef struct {
    char ID_A[DIM_ID_AIRPORT];
    char COUNTRY[DIM_COUNTRY];
    char CITY[DIM_CITY];
    int FLIGHTS;
} AIRPORT;

AIRPORT airports[MAX_AIRPORT];

typedef struct {
    int hours;
    int minutes;
} hour;

typedef struct {
    int day;
    int month;
    int year;
} date;

typedef struct R{
    char ID_F[DIM_ID_FLIGHT];
    date DATE;
    char * ID_R;
    int CAP;
} pR;

typedef struct node{
    pR rsv;
    struct node * next;
} * link;

typedef struct {
    char ID_F[DIM_ID_FLIGHT];
    char ID_A_DEP[DIM_ID_AIRPORT];
    char ID_A_ARR[DIM_ID_AIRPORT];
    date DATE;
    hour HOUR;
    hour DURATION;
    date ARRIVAL_DATE;
    hour ARRIVAL_HOUR;
    int CAPACITY;
    int cap_r;
}  FLIGHT;

typedef struct FLT{
    FLIGHT flt;
    struct FLT * next;
} * FLTS;

/*variáveis globais*/

int number_of_airports = 0; /* número de aeroportos introduzidos */
int number_of_flights = 0; /* número de voos introduzidos */

FLTS flights = NULL; /* lista de voos */

FLTS last_flt = NULL; /* último voo da lista*/

link rsvs[DIM_HASH_T]; /* hashtable de reservas */

date system_date = {1, 1, 2022}; /* data atual do sistema */
date system_date_lim = {31, 12, 2023};  /*data limite de funcionamento*/

/* Funcões de leitura */

date read_date() {
	date d;
	scanf("%d-%d-%d", &d.day, &d.month, &d.year);
	return d;
}

hour read_hour() {
	hour h;
	scanf("%d:%d", &h.hours, &h.minutes);
	return h;
}

int read_capacity() {
    int c;
    scanf("%d", &c);
    return c;
}

int read_next_word(char str[]) {
	char c = getchar();
	int i = 0;
	while (c == ' ' || c == '\t')
		c = getchar();
	while (c != ' ' && c != '\t' && c != '\n') {
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
	return (c == '\n');
}

int read_pointer_word(char **str) {
	char c = getchar();
	int i = 0;
    *str = (char*) malloc(sizeof(char));
	while (c == ' ' || c == '\t')
  		c = getchar();
	while (c != ' ' && c != '\t' && c != '\n') {
      *str = (char*) realloc(*str, sizeof(char) * (i + 2));
  		(*str)[i++] = c;
  		c = getchar();
	}
	(*str)[i] = '\0';
	return (c == '\n');
}

void read_line(char str[]) {
	char c = getchar();
	int i = 0;
	while (c == ' ' || c == '\t')
		c = getchar();
	while (c != '\n') {
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
}

void read_to_the_end() {
	char c = getchar();
	while (c != '\n')
		c = getchar();
}

/* procurar o ID do aeroporto */
int find_airport_ID(char id[]) {
    int i;
    for (i = 0; i < number_of_airports; i++) {
        if (!strcmp(airports[i].ID_A, id))
            return i;
    }
    return N_EXIST;
}

/* verificar ID do aeroporto */
int invalid_ID(char id[]) {
	int i;
	for (i = 0; id[i] != '\0'; i++)
		if (!(id[i] >= 'A' && id[i] <= 'Z'))
			return TRUE;
	return FALSE;
}

/* comando 'a' */
void case_a() {
    AIRPORT a;

    read_next_word(a.ID_A);
    read_next_word(a.COUNTRY);
    read_line(a.CITY);

    if (strlen(a.ID_A) != 3 || invalid_ID(a.ID_A))
        puts("invalid airport ID");
    else if (number_of_airports == MAX_AIRPORT)
        puts("too many airports");
    else if (find_airport_ID(a.ID_A) != N_EXIST)
        puts("duplicate airport");
    else {
        strcpy(airports[number_of_airports].ID_A, a.ID_A);
        strcpy(airports[number_of_airports].COUNTRY, a.COUNTRY);
        strcpy(airports[number_of_airports].CITY, a.CITY);
        airports[number_of_airports].FLIGHTS = 0;
        number_of_airports++;
        printf("airport %s\n", a.ID_A);
    }
}

/* MergeSort para aeroportos */
void merge_a(AIRPORT a[], int left, int m, int right, AIRPORT a_aux[]) {
    int i, j, k;
    for (i = m+1; i > left; i--)
        a_aux[i-1] = a[i-1];
    for (j = m; j < right; j++)
        a_aux[right+m-j] = a[j+1];
    for (k = left; k <= right; k++)
        if (strcmp(a_aux[j].ID_A, a_aux[i].ID_A) < 0 || i == m+1)
            a[k] = a_aux[j--];
        else
            a[k] = a_aux[i++];
}

void mergesort_a(AIRPORT a[], int left, int right, AIRPORT a_aux[]) {
    int m = (right+left)/2;
    if (right <= left)
        return;
    mergesort_a(a, left, m, a_aux);
    mergesort_a(a, m+1, right, a_aux);
    merge_a(a, left, m, right, a_aux);
}

/* lista de todos os aeroportos pela ordem alfabetica do código */
void list_all_airports() {
    int i;
    AIRPORT * a = (AIRPORT *) malloc(sizeof(AIRPORT) * number_of_airports);
    AIRPORT * a_aux = (AIRPORT *) malloc(sizeof(AIRPORT)* number_of_airports);

    for(i = 0; i < number_of_airports; i++)
        a[i] = airports[i];

    mergesort_a(a, 0, number_of_airports - 1, a_aux);

    for(i = 0; i < number_of_airports; i++) {
        printf("%s %s %s %d\n", a[i].ID_A, a[i].CITY,
                a[i].COUNTRY, a[i].FLIGHTS);
    }
    free(a);
    free(a_aux);
}


/* comando 'l' */
void case_l() {
    char **id;
    int i = 0, j;
    int index, last = 0;
    id = (char**) malloc(sizeof(char*));
    id[i] = (char*) malloc(sizeof(char) * DIM_ID_AIRPORT);
    last = read_next_word(id[i]);
    if(strlen(id[i]) == 0)
        list_all_airports();
    else {
        while(strlen(id[i]) != 0) {
            index = find_airport_ID(id[i]);
            if (index == N_EXIST)
                printf("%s: no such airport ID\n", id[i]);
            else
                printf("%s %s %s %d\n", airports[index].ID_A,
                airports[index].CITY, airports[index].COUNTRY,
                airports[index].FLIGHTS);
            if (last == FALSE) {
                id = (char**) realloc(id, sizeof(char*) * (++i + 1));
                id[i] = (char*) malloc(sizeof(char) * DIM_ID_AIRPORT);
                last = read_next_word(id[i]);
            }
            else break;
        }
    }
    for (j = 0; j <= i; j++)
        free(id[j]);
    free(id);
}

/* verificar o ID do voo introduzido */
int invalid_ID_FLIGHT(char id_f[]) {
    int i;

    if(strlen(id_f) < 3 || strlen(id_f) > 6)
        return TRUE;
    for(i = 0; i < 2; i++) {
        if(id_f[i] < 'A' || id_f[i] > 'Z')
            return TRUE;
    }
    for(i = 2; id_f[i] != '\0'; i++) {
        if(id_f[i] < '0' || id_f[i] > '9')
            return TRUE;
    }
    return FALSE;
}

/* verificar a data do voo introduzido */
int invalid_date(date d) {
    if((d.year > system_date.year) ||
        (d.year == system_date.year && d.month > system_date.month)||
        (d.year == system_date.year && d.month == system_date.month&&
        d.day >= system_date.day)) {
            if((d.year < system_date.year + 1) ||
                (d.year == system_date.year + 1 && d.month < system_date.month)
                || (d.year == system_date.year + 1 &&
                d.month == system_date.month && d.day <= system_date.day))
                    return FALSE;
    }
    return TRUE;
}

/* verificar se as duas datas são iguais */
int same_date(date d1, date d2) {
    return (d1.day == d2.day && d1.month == d2.month && d1.year == d2.year);
}

/* procurar o o voo com mesmo codigo e na mesma data */
int find_same_flt(char ID_F[], date d) {
    FLTS F_aux;
    for(F_aux = flights; F_aux != NULL; F_aux = F_aux->next) {
        if(strcmp(F_aux->flt.ID_F, ID_F) == 0) {
            if(same_date(F_aux->flt.DATE, d))
                return TRUE;
        }
    }
    return N_EXIST;
}

/* verificar a duração do voo */
int invalid_duration(hour h) {
    return (h.hours > 12 || (h.hours == 12 && h.minutes > 0));
}

/* verificar a capacidade do voo */
int invalid_capacity(int capacity) {
    return (capacity < 10);
}

/* calcular a data e a hora de chegada */
void arrival_date(FLTS f) {
    int days_of_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int minutes, hours;
    minutes = f->flt.HOUR.minutes + f->flt.DURATION.minutes;
    hours = f->flt.HOUR.hours + f->flt.DURATION.hours;
    f->flt.ARRIVAL_DATE.day = f->flt.DATE.day;
    f->flt.ARRIVAL_DATE.month = f->flt.DATE.month;
    f->flt.ARRIVAL_DATE.year = f->flt.DATE.year;

    if (minutes >= 60) {
        hours++;
        f->flt.ARRIVAL_HOUR.minutes = minutes - 60;
    }
    else
        f->flt.ARRIVAL_HOUR.minutes = minutes;
    if(hours >= 24) {
        f->flt.ARRIVAL_HOUR.hours = hours - 24;
        f->flt.ARRIVAL_DATE.day++;
    }
    else
        f->flt.ARRIVAL_HOUR.hours = hours;
    if (f->flt.ARRIVAL_DATE.day > days_of_month[f->flt.ARRIVAL_DATE.month-1]) {
        f->flt.ARRIVAL_DATE.day = 1;
        if(f->flt.ARRIVAL_DATE.month++ == 12) {
            f->flt.ARRIVAL_DATE.month = 1;
            f->flt.ARRIVAL_DATE.year++;
        }
    }
}

/* lista de todos os voos pela ordem de criação */
void list_all_flights() {
    FLTS f_aux;
    for(f_aux = flights; f_aux != NULL; f_aux = f_aux->next){
        printf("%s %s %s ", f_aux->flt.ID_F,
            f_aux->flt.ID_A_DEP,
            f_aux->flt.ID_A_ARR);

        printf("%02d-%02d-%04d ", f_aux->flt.DATE.day,
            f_aux->flt.DATE.month, f_aux->flt.DATE.year);

        printf("%02d:%02d\n", f_aux->flt.HOUR.hours,
                f_aux->flt.HOUR.minutes);
    }
}

/* verificar as condições(caso em commando 'v') */
int v_aux(FLTS f) {
    int i = FALSE;
    if(invalid_ID_FLIGHT(f->flt.ID_F)) {
        puts("invalid flight code");
    }
    else if(find_same_flt(f->flt.ID_F, f->flt.DATE) == TRUE) {
        puts("flight already exists");
    }
    else if(find_airport_ID(f->flt.ID_A_DEP) == N_EXIST) {
        printf("%s: no such airport ID\n", f->flt.ID_A_DEP);
    }
    else if(find_airport_ID(f->flt.ID_A_ARR) == N_EXIST) {
        printf("%s: no such airport ID\n", f->flt.ID_A_ARR);
    }
    else if(number_of_flights == MAX_FLIGHTS) {
        puts("too many flights");
    }
    else if(invalid_date(f->flt.DATE)) {
        puts("invalid date");
    }
    else if(invalid_duration(f->flt.DURATION)) {
        puts("invalid duration");
    }
    else if(invalid_capacity(f->flt.CAPACITY)) {
        puts("invalid capacity");
    }
    else
        i = TRUE;
    return i;
}

/* adicionar um novo voo a lista de voos */
void create_flight(FLTS f) {
    int i;
    arrival_date(f);
    if (flights == NULL) {
        flights = f;
        last_flt = f;
    }
    else {
        last_flt->next = f;
        last_flt = f;
    }

    for(i = 0; i < number_of_airports; i++){
        if(!strcmp(f->flt.ID_A_DEP, airports[i].ID_A)) {
            airports[i].FLIGHTS++;
            break;
        }
    }
    number_of_flights++;
}

/* comando 'v' */
void case_v() {
    FLTS f = (FLTS) malloc(sizeof(struct FLT));

    read_next_word(f->flt.ID_F);
    if(strlen(f->flt.ID_F) == 0) {
        list_all_flights();
        free(f);
    }
    else {
        read_next_word(f->flt.ID_A_DEP);
        read_next_word(f->flt.ID_A_ARR);
        f->flt.DATE = read_date();
        f->flt.HOUR = read_hour();
        f->flt.DURATION = read_hour();
        f->flt.CAPACITY = read_capacity();
        f->flt.cap_r = 0;
        read_to_the_end();
        if(v_aux(f) == TRUE) {
            f->next = NULL;
            create_flight(f);
        }
        else
            free(f);
    }
}

/* procurar voos com partida de um dado aeroporto*/
int find_flights_dep(char id_dep[], FLIGHT ** f) {
    int i = 0;
    FLTS f_aux;

    for(f_aux = flights; f_aux != NULL; f_aux = f_aux->next) {
        if(strcmp(f_aux->flt.ID_A_DEP, id_dep) == 0) {
            *f = (FLIGHT *) realloc(*f, sizeof(FLIGHT) * (i + 1));
            (*f)[i++] = f_aux->flt;
        }
    }
    return i;
}

/* procurar voos com chegada de um dado aeroporto */
int find_flights_arr(char id_arr[], FLIGHT ** f) {
    int i = 0;
    FLTS f_aux;

    for(f_aux = flights; f_aux != NULL; f_aux = f_aux->next) {
        if(strcmp(f_aux->flt.ID_A_ARR, id_arr) == 0) {
            *f = (FLIGHT *) realloc(*f, sizeof(FLIGHT) * (i + 1));
            (*f)[i++] = f_aux->flt;
        }
    }
    return i;
}

/* comparar dois datas com horas */
int earlier_date(date d1, hour h1, date d2, hour h2) {
    if(d1.year < d2.year || (d1.year == d2.year && d1.month < d2.month) ||
        (d1.year == d2.year && d1.month == d2.month && d1.day < d2.day)) {
            return TRUE;
        }
    else if(d1.year == d2.year && d1.month == d2.month && d1.day == d2.day) {
        if(h1.hours < h2.hours || (h1.hours == h2.hours &&
            h1.minutes < h2.minutes)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* MergeSort para voos com partida a um dado aeroporto */
void merge_f_dep(FLIGHT * f, int left, int m, int right, FLIGHT * f_aux) {
    int i, j, k;
    for (i = m+1; i > left; i--)
        f_aux[i-1] = f[i-1];
    for (j = m; j < right; j++)
        f_aux[right+m-j] = f[j+1];
    for (k = left; k <= right; k++)
        if (earlier_date(f_aux[j].DATE,f_aux[j].HOUR,
                        f_aux[i].DATE, f_aux[i].HOUR) == TRUE || i == m+1)
            f[k] = f_aux[j--];
        else
            f[k] = f_aux[i++];
}

void mergesort_f_dep(FLIGHT * f, int left, int right, FLIGHT * f_aux) {
    int m = (right+left)/2;
    if (right <= left)
        return;
    mergesort_f_dep(f, left, m, f_aux);
    mergesort_f_dep(f, m+1, right, f_aux);
    merge_f_dep(f, left, m, right, f_aux);
}

/* representar voos com partida de um dado aeroporto */
void show_flight_dep(FLIGHT * f, int count) {
    int i;
    for(i = 0; i < count; i++) {
    printf("%s %s ", f[i].ID_F, f[i].ID_A_ARR);
    printf("%02d-%02d-%04d ", f[i].DATE.day, f[i].DATE.month, f[i].DATE.year);
    printf("%02d:%02d\n", f[i].HOUR.hours, f[i].HOUR.minutes);
    }
}

/* ordenar voos com partida a um dado aeoporto e representá-los */
void p_aux(char id[]) {
    int l = 0, r;
    FLIGHT * f = (FLIGHT *) malloc(sizeof(FLIGHT));
    FLIGHT * f_aux;
    r = find_flights_dep(id, &f);
    f_aux = (FLIGHT *) malloc(sizeof(FLIGHT) * r);

    mergesort_f_dep(f, l, r - 1, f_aux);

    show_flight_dep(f, r);

    free(f);
    free(f_aux);
}

/*commando 'p'*/
void case_p(){
    char *id = (char*) malloc(sizeof(char) * DIM_ID_AIRPORT);

    read_next_word(id);

    if(find_airport_ID(id) == N_EXIST)
        printf("%s: no such airport ID\n", id);
    else
        p_aux(id);
    free(id);
}

/* MergeSort para voos com chegada a um dado aeroporto */
void merge_f_arr(FLIGHT * f, int left, int m, int right, FLIGHT * f_aux) {
    int i, j, k;
    for (i = m+1; i > left; i--)
        f_aux[i-1] = f[i-1];
    for (j = m; j < right; j++)
        f_aux[right+m-j] = f[j+1];
    for (k = left; k <= right; k++)
        if (earlier_date(f_aux[j].ARRIVAL_DATE, f_aux[j].ARRIVAL_HOUR,
                        f_aux[i].ARRIVAL_DATE, f_aux[i].ARRIVAL_HOUR) == TRUE
                        || i == m+1)
            f[k] = f_aux[j--];
        else
            f[k] = f_aux[i++];
}

void mergesort_f_arr(FLIGHT * f, int left, int right, FLIGHT * f_aux) {
    int m = (right+left)/2;
    if (right <= left)
        return;
    mergesort_f_arr(f, left, m, f_aux);
    mergesort_f_arr(f, m+1, right, f_aux);
    merge_f_arr(f, left, m, right, f_aux);
}

/* representar todos os voos com chegada a um dado aeroporto */
void show_flight_arr(FLIGHT * f, int count) {
    int i;
    for(i = 0; i < count; i++) {
    printf("%s %s ", f[i].ID_F, f[i].ID_A_DEP);
    printf("%02d-%02d-%04d ", f[i].ARRIVAL_DATE.day,
                            f[i].ARRIVAL_DATE.month,
                            f[i].ARRIVAL_DATE.year);
    printf("%02d:%02d\n", f[i].ARRIVAL_HOUR.hours,
                        f[i].ARRIVAL_HOUR.minutes);
    }
}

/* ordenar voos com chegada a um dado aeoporto e representá-los */
void c_aux(char id[]) {
    int l = 0, r;
    FLIGHT * f = (FLIGHT *) malloc(sizeof(FLIGHT));
    FLIGHT * f_aux;

    r = find_flights_arr(id, &f);
    f_aux = (FLIGHT *) malloc(sizeof(FLIGHT) * r);

    mergesort_f_arr(f, l, r - 1, f_aux);

    show_flight_arr(f, r);

    free(f);
    free(f_aux);
}

/*commando 'c'*/
void case_c() {
    char * id = (char*) malloc(sizeof(char) * DIM_ID_AIRPORT);

    read_next_word(id);

    if(find_airport_ID(id) == N_EXIST)
        printf("%s: no such airport ID\n", id);
    else
        c_aux(id);

    free(id);
}

/*commando 't'*/
void case_t() {
    date new_d;
    new_d = read_date();
    read_to_the_end();

    if(invalid_date(new_d))
        puts("invalid date");

    else {
        system_date.day = new_d.day;
        system_date.month = new_d.month;
        system_date.year = new_d.year;
        printf("%02d-%02d-%04d\n", new_d.day, new_d.month, new_d.year);
    }
}

/* hash table */
int hashU(char *v) {
    int h = 0, a = 127;
    for (; *v != '\0'; v++)
        h = (a*h + *v) % DIM_HASH_T;
    return h;
}

void Init() {
    int i;
    for (i = 0; i < DIM_HASH_T; i++) rsvs[i] = NULL;
}

/* verificar o código de reserva */
int invalid_code(char id_r[]) {
	int i;
	for (i = 0; id_r[i] != '\0'; i++)
		if (!((id_r[i] >= 'A' && id_r[i] <= 'Z') ||
            (id_r[i] >= '0' && id_r[i] <= '9')))
		    return TRUE;
	return FALSE;
}

/* procurar o voo */
int find_flight(char id_f[]) {
    FLTS F_aux;

    for(F_aux = flights; F_aux != NULL; F_aux = F_aux->next) {
        if(strcmp(F_aux->flt.ID_F, id_f) == 0) {
            return TRUE;
        }
    }
    return N_EXIST;
}

/* procurar a reserva */
int find_reservation(char id_r[]) {
    int i  = hashU(id_r);
    link r_aux;

    for(r_aux = rsvs[i]; r_aux != NULL; r_aux = r_aux->next)
        if(strcmp(r_aux->rsv.ID_R, id_r) == 0)
            return TRUE;

    return N_EXIST;
}

/* MergeSort para reservas */
void merge_r(pR lst[], int left, int m, int right, pR aux[]) {
    int i, j, k;
    for (i = m+1; i > left; i--)
        aux[i-1] = lst[i-1];
    for (j = m; j < right; j++)
        aux[right+m-j] = lst[j+1];
    for (k = left; k <= right; k++)
        if (strcmp(aux[j].ID_R, aux[i].ID_R) < 0 || i == m+1)
            lst[k] = aux[j--];
        else
            lst[k] = aux[i++];
}

void mergesort_r(pR lst[], int left, int right, pR aux[]) {
    int m = (right+left)/2;
        if (right <= left)
            return;
    mergesort_r(lst, left, m, aux);
    mergesort_r(lst, m+1, right, aux);
    merge_r(lst, left, m, right, aux);
}

/* representar reservas de um dado voo */
void show_reservations(pR * r, int count) {
    int i;

    for (i = 0; i < count; i++)
        printf("%s %d\n", r[i].ID_R, r[i].CAP);
}

/* ordenar reservas de um dado voo e representá-los */
void list_reservation(char id_f[], date d) {
    int l = 0, r = 0;
    int i;
    pR * lst = (pR *) malloc(0);
    pR * aux = (pR *) malloc(0);
    link r_aux;
    for(i = 0; i < DIM_HASH_T; i++) {
        for(r_aux = rsvs[i]; r_aux != NULL; r_aux = r_aux->next){
            if(strcmp(r_aux->rsv.ID_F, id_f) == 0 &&
                same_date(r_aux->rsv.DATE, d)) {
                lst = (pR *) realloc(lst, sizeof(pR) * (r + 1));
                lst[r++] = r_aux->rsv;
            }
        }
    }

    aux = (pR *) realloc(aux, sizeof(pR) * r);

    mergesort_r(lst, l, r - 1, aux);

    show_reservations(lst, r);

    free(aux);
    free(lst);
}

/* alterar número de reservas de um dado voo */
int update_cap(char id_f[], int num_rsv, date d) {
    FLTS F_aux;

    for(F_aux = flights; F_aux != NULL; F_aux = F_aux->next) {
        if(strcmp(F_aux->flt.ID_F, id_f) == 0 && 
            same_date(F_aux->flt.DATE, d)) {
            if(F_aux->flt.CAPACITY >= F_aux->flt.cap_r + num_rsv) {
                F_aux->flt.cap_r = F_aux->flt.cap_r + num_rsv;
                return TRUE;
            }
            else
                break;
        }
    }
    return FALSE;
}

/* verificar as condições(caso em commando 'r') */
int r_aux(link r) {
    int i = FALSE;
    if(strlen(r->rsv.ID_R) < 10 || invalid_code(r->rsv.ID_R)) {
        puts("invalid reservation code");
    }
    else if(find_same_flt(r->rsv.ID_F, r->rsv.DATE) == N_EXIST) {
        printf("%s: flight does not exist\n", r->rsv.ID_F);
    }
    else if(find_reservation(r->rsv.ID_R) == TRUE) {
        printf("%s: flight reservation already used\n", r->rsv.ID_R);
    }
    else if(!update_cap(r->rsv.ID_F, r->rsv.CAP, r->rsv.DATE)) {
        puts("too many reservations");
    }
    else if(invalid_date(r->rsv.DATE)) {
        puts("invalid date");
    }
    else if(r->rsv.CAP <= 0) {
        puts("invalid passenger number");
    }
    else
        i = TRUE;
    return i;
}

/* adicionar uma nova reserva a lista de reservas */
link creat_reservation(link rsv, int i) {

    if(rsvs[i] == NULL)
        rsvs[i] = rsv;

    else
        rsv->next = rsvs[i];
    
    return rsv;
}

/* comando'r' */
void case_r() {
    int i;
    link rsv = (link) malloc(sizeof(struct node));
    rsv->next = NULL;
    read_next_word(rsv->rsv.ID_F);
    rsv->rsv.DATE = read_date();

    if(read_pointer_word(&rsv->rsv.ID_R)) {
        if(find_flight(rsv->rsv.ID_F) == N_EXIST)
            printf("%s: flight does not exist\n", rsv->rsv.ID_F);
        else if(invalid_date(rsv->rsv.DATE))
            puts("invalid date");
        else
            list_reservation(rsv->rsv.ID_F, rsv->rsv.DATE);
        free(rsv->rsv.ID_R);
        free(rsv);
    }
    else {
        rsv->rsv.CAP = read_capacity();
        read_to_the_end();

        if (r_aux(rsv) == TRUE) {
            i = hashU(rsv->rsv.ID_R);
            rsvs[i] = creat_reservation(rsv, i);
        }
        else {
            free(rsv->rsv.ID_R);
            free(rsv);
        }
    }
}

/* altera o número de reservas de um dado voo depois de eliminar uma reserva */
void update_cap_after_e(char id_r[]) {
    int i = hashU(id_r);
    link rsv = (link) malloc(sizeof(struct node));
    link r_aux;
    FLTS F_aux;

    for(r_aux = rsvs[i]; r_aux != NULL; r_aux = r_aux->next)
        if(strcmp(r_aux->rsv.ID_R, id_r) == 0) {
            rsv->rsv = r_aux->rsv;
            break;
        }

    for(F_aux = flights; F_aux != NULL; F_aux = F_aux->next) {
        if(strcmp(F_aux->flt.ID_F, rsv->rsv.ID_F) == 0 && 
            same_date(F_aux->flt.DATE, rsv->rsv.DATE)) {
            F_aux->flt.cap_r = F_aux->flt.cap_r - rsv->rsv.CAP;
            break;
        }
    }
    free(rsv);
}

/* eliminar a reserva */
void delete_r(char id_r[]) {
    int i = hashU(id_r);
    link aux_r, pre_r;

    update_cap_after_e(id_r);
    for(aux_r = rsvs[i], pre_r = NULL; aux_r != NULL; 
            pre_r = aux_r, aux_r = aux_r->next) {
        if(strcmp(aux_r->rsv.ID_R, id_r) == 0) {
            if(aux_r == rsvs[i]) 
                rsvs[i] = aux_r->next;
            else 
                pre_r->next = aux_r->next;
            free(aux_r->rsv.ID_R);
            free(aux_r);
            break;
        }
    }
}

/* eliminar todas as reservas de um dado voo */
void delete_all_rsv(char id_f[]) {
    int i;
    link aux_r, pre_r;

    for(i = 0; i < DIM_HASH_T; i++) {
        for(aux_r = rsvs[i], pre_r = NULL; aux_r != NULL; 
                pre_r = aux_r, aux_r = aux_r->next)
            if(strcmp(aux_r->rsv.ID_F, id_f) == 0) {
                if(aux_r == rsvs[i])
                    rsvs[i] = aux_r->next;
                else
                    pre_r->next = aux_r->next;
            free(aux_r->rsv.ID_R);
            free(aux_r);
        }
    }
}


/* eliminar o voo */
void delete_flt(char id[]) {
    FLTS aux_f, pre_f;

    delete_all_rsv(id);

    for (aux_f = flights, pre_f = NULL; aux_f != NULL;) {
        if(strcmp(aux_f->flt.ID_F, id) == 0) {
            if (aux_f == last_flt) /* update last element */
                last_flt = pre_f;
            if (aux_f == flights) { /* delete head */
                pre_f = aux_f;
                flights = aux_f->next;
                aux_f = aux_f->next;
                free(pre_f);
                pre_f = NULL;
            }
            else { /* delete */
                pre_f->next = aux_f->next;
                free(aux_f);
                aux_f = pre_f->next;
            }
        }
        else {
            pre_f = aux_f;
            aux_f = aux_f->next;
        }
    }
    number_of_flights--;
}


/* verificar as condições(caso em commando 'e') */
int e_aux(char id[]) {
    if (strlen(id) >= 3 && strlen(id) <= 6) {
        if(find_flight(id) == TRUE) {
            delete_flt(id);
            return TRUE;
        }
        else
            return FALSE;
    }
    else if (strlen(id) >= 10) {
        if(find_reservation(id) == TRUE) {
            delete_r(id);
            return TRUE;
        }
        else
            return FALSE;
    }
    else
        return FALSE;
}

/* comando 'e' */
void case_e() {
    char * id;
    read_pointer_word(&id);
    if (e_aux(id) == FALSE)
        puts("not found");

    free(id);
}

/* comando 'q'(libertar a memória) */
void case_q() {
    int i;
    FLTS f_aux;
    link r_aux;

    for (f_aux = flights; f_aux != NULL;) {
        flights = flights->next;
        free(f_aux);
        f_aux = flights;
    }

    for(i = 0; i < DIM_HASH_T; i++){
        for (r_aux = rsvs[i]; r_aux != NULL;) {
            rsvs[i] = rsvs[i]->next;
            free(r_aux->rsv.ID_R);
            free(r_aux);
            r_aux = rsvs[i];
        }
    }
}


int main(){
    int c;

    while((c = getchar()) != EOF){
        switch (c) {
            case 'q': case_q();
                return 0;
            case 'a': case_a();
                break;
            case 'l': case_l();
                break;
            case 'v': case_v();
                break;
            case 'p': case_p();
                break;
            case 'c': case_c();
                break;
            case 't': case_t();
                break;
            case 'r': case_r();
                break;
            case 'e': case_e();
                break;
            default: printf("Invalid comand: %c\n", c);
        }
    }
    return 0;
}
