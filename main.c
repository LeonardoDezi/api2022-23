#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define DEBUG false



/********************
     SINGLE VALUED
    REPEATABLE LIST
*********************/

typedef struct list_node {
    unsigned int value;
    struct list_node * next;
} list;


void delete_list(list * l);

void list_push_back(list ** l, unsigned int value);
list * list_push_front(list ** l, unsigned int value);
list * list_remove(list ** l, unsigned int value);
bool list_search(list ** l, unsigned int value);

void print_list(list * l);



/**** MEMORY ****/

void delete_list(list * l) {
    list * it = l;
    list * tmp;
    while (it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
    }
}



/**** OPERATIONS ****/

void list_push_back(list ** l, unsigned int value) {
    list * new = (list *) malloc(sizeof(list));
    new->value = value;
    new->next = NULL;

    if (*l == NULL) {
        *l = new;
    }
    else {
        list * it = *l;
        while (it->next != NULL) {
            it = it->next;
        }
        it->next = new;
    }
}

list * list_push_front(list ** l, unsigned int value) {
    list * new = (list *) malloc(sizeof(list));
    new->value = value;
    new->next = *l;
    return new;
}

list * list_remove(list ** l, unsigned int value) {
    if (*l != NULL) {
        if ((*l)->value == value) {
            *l = (*l)->next;
            return *l;
        }
        else {
            list * it = *l;
            while (it->next != NULL) {
                if (it->next->value == value) {
                    list * tmp = it->next;
                    it->next = it->next->next;
                    free(tmp);
                    break;
                }
                it = it->next;
            }
        }
    }

    return *l;
}

bool list_search(list ** l, unsigned int value) {
    list * it = *l;

    while (it != NULL) {
        if (it->value == value)
            return true;
        it = it->next;
    }

    return false;
}



/**** UTILITIES ****/

void print_list(list * l) {
    list * it = l;
    while (it != NULL) {
        printf("%u", it->value);
        if (it->next)
            printf(" ");
        else
            printf("\n");
        it = it->next;
    }
}





/********************
    DOUBLE VALUED
     UNIQUE LIST
*********************/

typedef struct list2_node {
    unsigned int key;
    unsigned int value;
    struct list2_node * next;
} list2;


void delete_list2(list2 * l);

bool list2_push_back(list2 ** l, unsigned int key, unsigned int value);
list2 * list2_remove(list2 ** l, unsigned int key);

void print_list2(list2 * l);



/**** MEMORY ****/

void delete_list2(list2 * l) {
    list2 * it = l;
    list2 * tmp;
    while (it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
    }
}



/**** OPERATIONS ****/

bool list2_push_back(list2 ** l, unsigned int key, unsigned int value) {
    if (*l == NULL) {
        *l = (list2 *) malloc(sizeof(list2));
        (*l)->key = key;
        (*l)->value = value;
        (*l)->next = NULL;
        return true;
    }
    else {
        list2 * it = *l;
        if (it->key == key)
            return false;
        while (it->next != NULL) {
            if (it->next->key == key)
                return false;
            it = it->next;
        }
        list2 * new = (list2 *) malloc(sizeof(list2));
        new->key = key;
        new->value = value;
        new->next = NULL;
        it->next = new;
        return true;
    }
}

list2 * list2_remove(list2 ** l, unsigned int key) {
    list2 * ret = NULL;

    if (*l != NULL) {
        if ((*l)->key == key) {
            ret = *l;
            *l = (*l)->next;
            ret->next = NULL;
        }
        else {
            list2 * it = *l;
            while (it->next != NULL) {
                if (it->next->key == key) {
                    ret = it->next;
                    it->next = it->next->next;
                    break;
                }
                it = it->next;
            }
        }
    }

    return ret;
}



/**** UTILITIES ****/

void print_list2(list2 * l) {
    list2 * it = l;
    while (it != NULL) {
        printf("(%u, %u) ", it->key, it->value);
        it = it->next;
    }
    printf("\n");
}

// #include "heap.c"



/****************
    HASH TABLE
*****************/

#define HASHMAP_INITIAL_SIZE 8
#define HASHMAP_RESIZE_BITS 2
#define HASHMAP_LOAD_FACTOR 75

typedef struct {
    list2 ** items;
    unsigned int size;
    unsigned int count;
    unsigned int grow_at;
    unsigned int shrink_at;
} hash_table;


void alloc_ht(hash_table * ht, unsigned int size);
hash_table * create_ht(unsigned int n_elem);

unsigned long hash_function(hash_table * ht, unsigned int value) {
    return value % ht->size;
}

void rehash(hash_table * ht, unsigned int new_size);
bool ht_insert(hash_table * ht, unsigned int key, unsigned int value);
int ht_remove(hash_table * ht, unsigned int key);
int ht_search(hash_table * ht, unsigned int key);
void ht_update(hash_table * ht, unsigned int key, unsigned int value);

void print_ht(hash_table * ht);



/**** MEMORY ****/

void alloc_ht(hash_table * ht, unsigned int size) {
    ht->items = calloc(size, sizeof(list2 *));
    ht->size = size;
    ht->count = 0;
    ht->grow_at = (unsigned int) (size * (HASHMAP_LOAD_FACTOR / 100.));
    ht->shrink_at = (unsigned int) ((float) ht->grow_at / (float) ((1 << HASHMAP_RESIZE_BITS) + 1));
}

hash_table * create_ht(unsigned int n_elem) {
    hash_table * ht = malloc(sizeof(hash_table));
    unsigned int size = HASHMAP_INITIAL_SIZE;
    unsigned int min_size = (unsigned int) (n_elem / (HASHMAP_LOAD_FACTOR / 100.));

    while (min_size > size)
        size <<= HASHMAP_RESIZE_BITS;

    alloc_ht(ht, size);

    return ht;
}

void delete_ht(hash_table * ht) {
    free(ht->items);
}



/**** OPERATIONS ****/

void rehash(hash_table * ht, unsigned int new_size) {
    list2 ** old_items = ht->items;
    unsigned int old_size = ht->size;
    alloc_ht(ht, new_size);

    for (unsigned int k = 0; k < old_size; ++k) {
        list2 * old_item = old_items[k];
        while (old_item) {
            ht_insert(ht, old_item->key, old_item->value);
            old_item = old_item->next;
        }
        delete_list2(old_items[k]);
    }

    free(old_items);
}

bool ht_insert(hash_table * ht, unsigned int key, unsigned int value) {
    unsigned int hash = hash_function(ht, key);
    if (!list2_push_back((ht->items) + hash, key, value)) {
        return false;
    }
    else {
        ht->count++;

        if (ht->count > ht->grow_at)
            rehash(ht, ht->size << HASHMAP_RESIZE_BITS);

        return true;
    }
}

int ht_remove(hash_table *ht, unsigned int key) {
    unsigned int hash = hash_function(ht, key);
    list2 * item = list2_remove((ht->items) + hash, key);
    int ret = -1;

    if (item) {
        ht->count--;
        ret = item->value;
        free(item);

        if(ht->count < ht->shrink_at)
            rehash(ht, ht->size >> HASHMAP_RESIZE_BITS);
    }

    return ret;
}

int ht_search(hash_table * ht, unsigned int key) {
    list2 * item = ht->items[hash_function(ht, key)];
    while (item) {
        if (item->key == key)
            return item->value;
        item = item->next;
    }
    return -1;
}

void ht_update(hash_table * ht, unsigned int key, unsigned int value) {
    list2 * item = ht->items[hash_function(ht, key)];
    while (item) {
        if (item->key == key) {
            item->value = value;
            break;
        }
        item = item->next;
    }
}



/**** UTILITIES ****/

void print_ht(hash_table * ht) {
    printf("\n---- Hash Table ---\n");
    for (unsigned int k = 0; k < ht->size; ++k) {
        printf("@%u: ", k);
        list2 * item = (ht->items)[k];
        print_list2(item);
    }
    printf("-------------------\n");
}




typedef struct {
    hash_table * stations_map;
    unsigned int n_stations;
    unsigned int max_stations;
    unsigned int * stations_inverse_map;
    list ** cars;
} highway;



highway * create_highway() {
    highway * hw = malloc(sizeof(highway));
    hw->stations_map = create_ht(8);
    hw->n_stations = 0;
    hw->max_stations = 8;
    hw->stations_inverse_map = calloc(hw->max_stations, sizeof(unsigned int));
    hw->cars = calloc(hw->max_stations, sizeof(list *));
    return hw;
}

void delete_highway(highway * hw) {
    delete_ht(hw->stations_map);
    free(hw->stations_inverse_map);
    free(hw->cars);
}

void print_highway(highway * hw) {
    print_ht(hw->stations_map);

    for (unsigned int k = 0; k < hw->n_stations; ++k) {
        printf("%u: ", k);
        print_list(hw->cars[k]);
    }
}

void highway_add_station(highway * hw, unsigned int station, char * cars) {
    if (ht_insert(hw->stations_map, station, hw->n_stations)) {
        char * token = strtok(cars, "\n\r ");
        unsigned int num_cars = atoi(token);

        hw->stations_inverse_map[hw->n_stations] = station;
        for (unsigned int k = 0; k < num_cars; ++k) {
            token = strtok(NULL, "\n\r ");
            unsigned int car = atoi(token);
            hw->cars[hw->n_stations] = list_push_front(&(hw->cars[hw->n_stations]), car);
        }

        hw->n_stations++;

        if (hw->n_stations == hw->max_stations) {
            hw->max_stations *= 2;
            unsigned int * old_stations_inverse_map = hw->stations_inverse_map;
            list ** old_cars = hw->cars;
            hw->stations_inverse_map = calloc(hw->max_stations, sizeof(unsigned int));
            hw->cars = calloc(hw->max_stations, sizeof(list *));
            for (unsigned int k = 0; k < hw->n_stations; ++k) {
                hw->stations_inverse_map[k] = old_stations_inverse_map[k];
                hw->cars[k] = old_cars[k];
            }
            free(old_stations_inverse_map);
            free(old_cars);
        }

        printf("aggiunta\n");
    }
    else {
        printf("non aggiunta\n");
    }
}

void highway_remove_station(highway * hw, unsigned int station) {
    int s = ht_remove(hw->stations_map, station);
    if (s == -1){
        printf("non demolita\n");
    }
    else {
        free(hw->cars[s]);
        hw->n_stations--;
        hw->stations_inverse_map[s] = hw->stations_inverse_map[hw->n_stations];
        hw->cars[s] = hw->cars[hw->n_stations];
        hw->stations_inverse_map[hw->n_stations] = 0;
        hw->cars[hw->n_stations] = NULL;
        ht_update(hw->stations_map, hw->stations_inverse_map[s], s);
        // TODO shrink old_stations_inverse_map
        // TODO shrink cars
        printf("demolita\n");
    }
}

void highway_add_car(highway * hw, unsigned int station, unsigned int car) {
    unsigned int station_id = ht_search(hw->stations_map, station);
    if (station_id == -1) {
        printf("non aggiunta\n");
    }
    else {
        hw->cars[station_id] = list_push_front(&(hw->cars[station_id]), car);
        printf("aggiunta\n");
    }
}

void highway_remove_car(highway * hw, unsigned int station, unsigned int car) {
    unsigned int station_id = ht_search(hw->stations_map, station);
    if (station_id == -1) {
        printf("non rottamata\n");
    }
    else {
        if (list_search(&(hw->cars[station_id]), car)) {
            hw->cars[station_id] = list_remove(&(hw->cars[station_id]), car);
            printf("rottamata\n");
        }
        else
            printf("non rottamata\n");
    }
}



void print_vector(unsigned int * vec, unsigned int size) {
    for (unsigned int k = 0; k < size; ++k) {
        printf("%u ", vec[k]);
    }
    printf("\n");
}


void highway_plan_trip(highway * hw, unsigned int start, unsigned int end) {
    bool left_to_right = (end > start) ? true : false;

    // Creo lista ordinata di stazioni comprese tra start e end
    list2 * stations = NULL;
    list2 * stations_it = NULL;
    for (unsigned int k = 0; k < hw->stations_map->size; ++k) {
        list2 * it = hw->stations_map->items[k];
        while (it) {
            if ((left_to_right && (it->key >= start && it->key <= end)) ||
                (!left_to_right && (it->key >= end && it->key <= start))) {
                list2 * new = (list2 *) malloc(sizeof(list2));
                new->key = it->key;
                new->value = it->value;
                new->next = NULL;

                if (!stations) {
                    stations = new;
                }
                else {
                    if ((left_to_right && (it->key < stations->key)) ||
                        (!left_to_right && (it->key > stations->key))) {
                        new->next = stations;
                        stations = new;
                    }
                    else {
                        stations_it = stations;
                        while (stations_it->next) {
                            if ((left_to_right && (it->key < stations_it->next->key)) ||
                                (!left_to_right && (it->key > stations_it->next->key))) {
                                new->next = stations_it->next;
                                stations_it->next = new;
                                break;
                            }
                            stations_it = stations_it->next;
                        }
                        if (!stations_it->next) {
                            stations_it->next = new;
                        }
                    }
                }
            }

            it = it->next;
        }
    }

    // calcolo macchina con massima autonomia per ciascuna stazione
    unsigned int * max_cars = calloc(hw->n_stations, sizeof(unsigned int));
    for (unsigned int k = 0; k < hw->n_stations; ++k) {
        max_cars[k] = 0;
        list * cars_it = hw->cars[k];
        while (cars_it) {
            if (cars_it->value > max_cars[k])
                max_cars[k] = cars_it->value;
            cars_it = cars_it->next;
        }
    }

    // creo vettore con distanza e nodo (stazione) precedente (per dijkstra)
    unsigned int * distances = calloc(hw->n_stations, sizeof(unsigned int));
    unsigned int * previouses = calloc(hw->n_stations, sizeof(unsigned int));
    for (unsigned int k = 0; k < hw->n_stations; ++k) {
        distances[k] = UINT_MAX;
        previouses[k] = UINT_MAX;
    }
    distances[stations->value] = 0;

    list2 * cur_station = stations;
    unsigned int cur_max_car = max_cars[stations->value];
    unsigned int cur_distance;

    // dijkstra
    while (cur_station->key != end) {
        unsigned int cur_station_id = cur_station->value;
        unsigned int next_station_id;
        cur_distance = distances[cur_station_id];
        cur_max_car = max_cars[cur_station_id];

        if (cur_distance == UINT_MAX)
            break;

        list2 * next_station = cur_station->next;

        if (left_to_right) {
            while (next_station) {
                if (next_station->key > cur_station->key + cur_max_car)
                    break;

                next_station_id = next_station->value;

                if (distances[next_station_id] > cur_distance + 1) {
                    distances[next_station_id] = cur_distance + 1;
                    previouses[next_station_id] = cur_station->value;
                }

                next_station = next_station->next;
            }
        }
        else {
            while (next_station) {
                if (next_station->key + cur_max_car < cur_station->key)
                    break;

                next_station_id = next_station->value;

                if (distances[next_station_id] >= cur_distance + 1) {
                    distances[next_station_id] = cur_distance + 1;
                    previouses[next_station_id] = cur_station->value;
                }

                next_station = next_station->next;
            }
        }

        cur_station = cur_station->next;
    }


    if (previouses[cur_station->value] == UINT_MAX) {
        printf("nessun percorso\n");
    }
    else {
        list * path = NULL;
        path = list_push_front(&path, end);
        unsigned int cur_station_key = end;
        unsigned int cur_station_value = cur_station->value;
        while (cur_station_key != start) {
            cur_station_value = previouses[cur_station_value];
            cur_station_key = hw->stations_inverse_map[cur_station_value];
            path = list_push_front(&path, cur_station_key);
        }

        print_list(path);
        delete_list(path);
    }

    free(max_cars);
    free(distances);
    free(previouses);
    delete_list2(stations);
}

int main() {
    char * line = NULL;
    size_t len = 0;
    char * token;
    unsigned int k = 1;

    highway * hw = create_highway();

    while ((getline(&line, &len, stdin)) != -1) {
        token = strtok(line, "\n\r ");
        if (DEBUG) printf("%u: ", k);

        if (strcmp(token, "aggiungi-stazione") == 0) {
            token = strtok(NULL, "\n\r ");
            unsigned int station = atoi(token);
            highway_add_station(hw, station, strtok(NULL, ""));
        }
        else if (strcmp(token, "demolisci-stazione") == 0) {
            token = strtok(NULL, "\n\r ");
            unsigned int station = atoi(token);
            highway_remove_station(hw, station);
        }
        else if (strcmp(token, "aggiungi-auto") == 0) {
            token = strtok(NULL, "\n\r ");
            unsigned int station = atoi(token);
            token = strtok(NULL, "\n\r ");
            unsigned int car = atoi(token);
            highway_add_car(hw, station, car);
        }
        else if (strcmp(token, "rottama-auto") == 0) {
            token = strtok(NULL, "\n\r ");
            unsigned int station = atoi(token);
            token = strtok(NULL, "\n\r ");
            unsigned int car = atoi(token);
            highway_remove_car(hw, station, car);
        }
        else if (strcmp(token, "pianifica-percorso") == 0) {
            token = strtok(NULL, "\n\r ");
            unsigned int start = atoi(token);
            token = strtok(NULL, "\n\r ");
            unsigned int end = atoi(token);
            highway_plan_trip(hw, start, end);
        }

        ++k;
    }

    delete_highway(hw);

    return 0;
}