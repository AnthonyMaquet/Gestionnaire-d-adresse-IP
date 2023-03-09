#include <stdio.h>   // Inclut les déclarations pour les fonctions d'entrée/sortie standard
#include <stdlib.h>  // Inclut les déclarations pour les fonctions de gestion de la mémoire dynamique
#include <string.h>  // Inclut les déclarations pour les fonctions de manipulation de chaînes de caractères

// Définition d'une structure pour stocker une adresse IP sous forme de 4 octets
typedef struct {
    unsigned char octets[4];  // Tableau de 4 octets pour stocker l'adresse IP
} ip_address;

// Définition d'une structure (pour stocker) d'un catalogue d'adresses IP
typedef struct {
    size_t size;             // Taille du catalogue (nombre d'adresses)
    ip_address *addresses;   // Tableau d'adresses IP
} address_catalog;

// Fonction pour créer un catalogue d'adresses vide (nouveau catalogue d'adresses IP)
address_catalog *create_address_catalog() {
    address_catalog *catalog = malloc(sizeof(address_catalog));  // Allocation dynamique de la mémoire pour le catalogue
    catalog->size = 0;  // Initialisation de la taille du catalogue à 0
    catalog->addresses = NULL;  // Initialisation du tableau d'adresses à NULL
    return catalog;
}

// Fonction pour ajouter une adresse IP au catalogue
void add_address_to_catalog(address_catalog *catalog, ip_address address) {
    catalog->size++;  // Incrémente la taille du catalogue
    catalog->addresses = realloc(catalog->addresses, catalog->size * sizeof(ip_address));  // Réalloue de la mémoire pour le tableau d'adresses
    catalog->addresses[catalog->size - 1] = address;  // Ajoute l'adresse à la fin du tableau
}

// Fonction pour retirer une adresse IP du catalogue
void remove_address_from_catalog(address_catalog *catalog, ip_address address) {
    int found = 0; // indique si l'adresse a été trouvée dans le catalogue
    for (int i = 0; i < catalog->size; i++) { // Parcours le catalogue
        if (found) { // si l'adresse a déjà été trouvée, on décale les éléments suivants d'un index vers la gauche
            catalog->addresses[i-1] = catalog->addresses[i];
        } else if (memcmp(&(catalog->addresses[i]), &address, sizeof(ip_address)) == 0) { // on compare les adresses
            found = 1; // l'adresse a été trouvée
        }
    }
    if (found) { // Si l'adresse a été trouvée, on réalloue la mémoire et on réduit la taille du catalogue
        catalog->size--;
        catalog->addresses = realloc(catalog->addresses, catalog->size * sizeof(ip_address));
    }
}

// Fonction pour filtrer les adresses du catalogue en fonction d'un masque donné
address_catalog *filter_addresses_by_mask(address_catalog *catalog, ip_address mask) {
    address_catalog *filtered_catalog = create_address_catalog();  // Crée un nouveau catalogue filtré (vide)
    for (int i = 0; i < catalog->size; i++) { // Parcours du catalogue original
        ip_address address = catalog->addresses[i];  // Récupère l'adresse courante
        // Vérifie si l'adresse courante correspond au masque en comparant chaque octet
        if ((address.octets[0] & mask.octets[0]) == mask.octets[0] && //Vérification du masque sur chaque octet de l'adresse
            (address.octets[1] & mask.octets[1]) == mask.octets[1] &&
            (address.octets[2] & mask.octets[2]) == mask.octets[2] &&
            (address.octets[3] & mask.octets[3]) == mask.octets[3]) {
            add_address_to_catalog(filtered_catalog, address);  // Ajoute l'adresse filtrée au nouveau catalogue
        }
    }
    return filtered_catalog;  // Retourne le nouveau catalogue filtré
}

// Fonction pour sauvegarder le catalogue dans un fichier
void save_catalog_to_file(address_catalog *catalog, const char *filename) {
    // Ouvre le fichier spécifié par le paramètre filename en mode écriture binaire.
    FILE *fp = fopen(filename, "wb"); //Ouverture d'un fichier binaire en écriture
    // Si le fichier est ouvert avec succès...
    if (fp != NULL) {
        // Écrit la taille du catalogue dans le fichier.
        fwrite(&catalog->size, sizeof(size_t), 1, fp);
        // Écrit les adresses du catalogue dans le fichier.
        fwrite(catalog->addresses, sizeof(ip_address), catalog->size, fp);
        // Ferme le fichier.
        fclose(fp);
    }
}

// Fonction pour récupérer les IP depuis le catalogue
address_catalog *load_catalog_from_file(const char *filename) {
    // Crée un nouveau catalogue d'adresses.
    address_catalog *catalog = create_address_catalog();
    // Ouvre le fichier spécifié par le paramètre filename en mode lecture binaire.
    FILE *fp = fopen(filename, "rb"); // Ouverture d'un fichier binaire en lecture
    // Si le fichier est ouvert avec succès...
    if (fp != NULL) {
        // Lit la taille du catalogue à partir du fichier.
        fread(&catalog->size, sizeof(size_t), 1, fp);
        // Alloue de la mémoire pour les adresses du catalogue.
        catalog->addresses = malloc(catalog->size * sizeof(ip_address));
        // Lit les adresses du catalogue à partir du fichier.
        fread(catalog->addresses, sizeof(ip_address), catalog->size, fp);
        // Ferme le fichier.
        fclose(fp);
    }
    // Renvoie le catalogue.
    return catalog;
}

// Fonction pour avoir l'IP sous forme décimal
void print_address(ip_address address) {
    printf("Sous forme decimal : %d.%d.%d.%d", address.octets[0], address.octets[1], address.octets[2], address.octets[3]);
}

// Fonction pour avoir l'IP sous forme binaire
void print_binary_address(ip_address address) {
    printf("Sous forme binaire : ");
    for (int i = 0; i < 4; i++) {
        unsigned char octet = address.octets[i]; // Parcourt chaque octets de l'adresse
        int tab[8]; // On stock dans un tableau
        // On convertit en binaire
        for (int j = 0; j < 8; j++) {
            tab[j] = octet % 2;
            octet /= 2;
        }
        // On met les bits du tableau dans l'ordre inverse (de gauche à droite)
        for (int j = 7; j >= 0; j--) {
            printf("%d", tab[j]);
        }
        // On met un point entre chaque octet
        if (i != 3) {
            printf(".");
        }
    }
}

// Fonction pour l'IP sous forme hexadecimal
void print_hexadecimal_address(ip_address address) {
    printf("Sous forme hexadecimal : %x.%x.%x.%x", address.octets[0], address.octets[1], address.octets[2], address.octets[3]);
}

// Fonction pour savoir si l'addresse est privé, publique ou spécial
void is_private_or_public(ip_address address) {
    if (address.octets[0] == 10 || (address.octets[0] == 172 && address.octets[1] >= 16 && address.octets[1] <= 31) || (address.octets[0] == 192 && address.octets[1] == 168 && address.octets[2] >= 1)) {
        printf("L'addresse IP est prive");
    } else if (address.octets[0] == 127 && address.octets[1] == 0 && address.octets[2] == 0 && address.octets[3] == 1) {
        printf("L'addresse IP est speciale");
    } else {
        printf("L'addresse IP est publique");
    }
}

// Fonction pour savoir la classe des adresses
void is_class(ip_address address) {
    if ((address.octets[0] >= 0 &&  address.octets[0] <= 126)) {
        printf(" et fait partie de la classe A");
    } else if ((address.octets[0] >= 128 &&  address.octets[0] <= 191)) {
        printf(" et fait partie de la classe B");
    } else if ((address.octets[0] >= 192 &&  address.octets[0] <= 223)) {
        printf(" et fait partie de la classe C");
    } else if ((address.octets[0] >= 224 &&  address.octets[0] <= 239)) {
        printf(" et fait partie de la classe D");
    } else if ((address.octets[0] >= 240 &&  address.octets[0] <= 255)) {
        printf(" et fait partie de la classe E");
    } else {
        printf(" et ne fait partie d'aucune classe");
    }
}

// Fonction pour afficher les adresses IP
void print_address_catalog(address_catalog *catalog) {
    printf("\nVoici les adresses presentes dans votre catalogue :\n\n");
    for (int i = 0; i < catalog->size; i++) {
        is_private_or_public(catalog->addresses[i]); // Affiche si l'IP est publique / privée ou spéciale
        is_class(catalog->addresses[i]); // Affiche les classes de l'IP
        printf("\n");
        print_address(catalog->addresses[i]); // Affiche l'IP sous forme décimale
        printf("\n");
        print_binary_address(catalog->addresses[i]); // Affiche l'IP sous forme binaire
        printf("\n");
        print_hexadecimal_address(catalog->addresses[i]); // // Affiche l'IP sous forme hexadécimale
        printf("\n\n");
    }
}

// Menu
void menu() {
    address_catalog *catalog = load_catalog_from_file("adresses.bin");

    int choice = 0;
    while (choice != 5) {
        printf("\nChoisissez ce que vous voulez faire :\n");
        printf("1. Afficher toutes les adresses du catalogue\n");
        printf("2. Ajouter une nouvelle adresse au catalogue\n");
        printf("3. Retirer une adresse du catalogue\n");
        printf("4. Filrer les adresses par masque\n");
        printf("5. Quitter\n");
        printf("Que voulez-vous faire ? : ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                print_address_catalog(catalog); // On liste les IP du catalogue
                break;
            }
            case 2: {
                ip_address address;
                printf("Entrez l'adresse IP que vous souhaitez ajouter sous forme decimal (Exemple : x.x.x.x) : ");
                scanf("%hhu.%hhu.%hhu.%hhu", &address.octets[0], &address.octets[1], &address.octets[2], &address.octets[3]);
                add_address_to_catalog(catalog, address); // On ajoute l'IP au catalogue
                save_catalog_to_file(catalog, "adresses.bin"); // On sauvegarde l'IP dans le fichier
                printf("Votre adresse a bien etait ajoute au catalogue\n");
                break;
            }
            case 3: {
                ip_address address;
                printf("Entrez l'adresse IP que vous souhaitez supprimer sous forme decimal (Exemple : x.x.x.x) : ");
                scanf("%hhu.%hhu.%hhu.%hhu", &address.octets[0], &address.octets[1], &address.octets[2], &address.octets[3]);
                remove_address_from_catalog(catalog, address); // On retire l'IP du catalogue
                save_catalog_to_file(catalog, "adresses.bin"); // On sauvegarde le fichier
                printf("L'adresse a bien ete retire du catalogue\n");
                break;
            }
            case 4: {
                ip_address mask;
                printf("Entrez votre filtre sous forme decimal (Exemple : x.x.x.x): ");
                scanf("%hhu.%hhu.%hhu.%hhu", &mask.octets[0], &mask.octets[1], &mask.octets[2], &mask.octets[3]);
                address_catalog *filtered_catalog = filter_addresses_by_mask(catalog, mask); // Comparaison du catalogue d'origine avec celui du filtre
                print_address_catalog(filtered_catalog); // On liste les IP avec le filtre
                break;
            }
            case 5: {
                printf("\nA bientot !\n");
                break;
            }
            default:
                printf("\nChoix invalide, veuillez reassayer.\n");
                break;
        }
    }

// Libéreration de la mémoire
    free(catalog->addresses);
    free(catalog);
}

int main() {
    menu(); //Retourner le menu sinon cmd vide
    return 0;
}
