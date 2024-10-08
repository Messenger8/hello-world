#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a movie
struct movie {
    char *title;
    int year;
    char *languages[5];
    double rating;
    struct movie *next;
};

// Function to create a new movie node
struct movie *create_movie(char *line) {
    struct movie *new_movie = (struct movie *)malloc(sizeof(struct movie));
    char *saveptr;
    char *token;

    // Parse title
    token = strtok_r(line, ",", &saveptr);
    new_movie->title = strdup(token);

    // Parse year
    token = strtok_r(NULL, ",", &saveptr);
    new_movie->year = atoi(token);

    // Parse languages
    token = strtok_r(NULL, ",", &saveptr);
    char *lang_saveptr;
    char *lang_token = strtok_r(token + 1, ";", &lang_saveptr);  // Skip initial '['
    int lang_index = 0;
    while (lang_token != NULL && lang_index < 5) {
        new_movie->languages[lang_index++] = strdup(lang_token);
        lang_token = strtok_r(NULL, ";", &lang_saveptr);
    }
    new_movie->languages[lang_index - 1][strcspn(new_movie->languages[lang_index - 1], "]")] = '\0';  // Remove trailing ']'

    // Parse rating
    token = strtok_r(NULL, ",", &saveptr);
    new_movie->rating = strtod(token, NULL);

    new_movie->next = NULL;
    return new_movie;
}

// Function to add a movie to the linked list
void add_movie_to_list(struct movie **head, struct movie *new_movie) {
    if (*head == NULL) {
        *head = new_movie;
    } else {
        struct movie *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_movie;
    }
}

// Function to process the file and create the linked list
struct movie *process_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return NULL;
    }

    struct movie *head = NULL;
    char buffer[1024];
    fgets(buffer, 1024, file);  // Skip header line

    int movie_count = 0;
    while (fgets(buffer, 1024, file)) {
        struct movie *new_movie = create_movie(buffer);
        add_movie_to_list(&head, new_movie);
        movie_count++;
    }

    fclose(file);
    printf("Processed file %s and parsed data for %d movies\n", filename, movie_count);
    return head;
}

// Function to show movies by year
void show_movies_by_year(struct movie *head, int year) {
    struct movie *temp = head;
    int found = 0;
    while (temp != NULL) {
        if (temp->year == year) {
            printf("%s\n", temp->title);
            found = 1;
        }
        temp = temp->next;
    }
    if (!found) {
        printf("No data about movies released in the year %d\n", year);
    }
}

// Function to show highest rated movie for each year
void show_highest_rated_per_year(struct movie *head) {
    struct movie *temp = head;
    struct movie *highest_rated[2022 - 1900] = {NULL};  // Array to track highest-rated movie for each year

    while (temp != NULL) {
        int index = temp->year - 1900;
        if (highest_rated[index] == NULL || temp->rating > highest_rated[index]->rating) {
            highest_rated[index] = temp;
        }
        temp = temp->next;
    }

    for (int i = 0; i < (2022 - 1900); i++) {
        if (highest_rated[i] != NULL) {
            printf("%d %.1f %s\n", 1900 + i, highest_rated[i]->rating, highest_rated[i]->title);
        }
    }
}

// Function to show movies by language
void show_movies_by_language(struct movie *head, char *language) {
    struct movie *temp = head;
    int found = 0;
    while (temp != NULL) {
        for (int i = 0; i < 5; i++) {
            if (temp->languages[i] != NULL && strcmp(temp->languages[i], language) == 0) {
                printf("%d %s\n", temp->year, temp->title);
                found = 1;
                break;
            }
        }
        temp = temp->next;
    }
    if (!found) {
        printf("No data about movies released in %s\n", language);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <csv_file>\n", argv[0]);
        return 1;
    }

    struct movie *head = process_file(argv[1]);
    if (head == NULL) {
        return 1;
    }

    int choice;
    do {
        printf("\n1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int year;
                printf("Enter the year for which you want to see movies: ");
                scanf("%d", &year);
                show_movies_by_year(head, year);
                break;
            }
            case 2:
                show_highest_rated_per_year(head);
                break;
            case 3: {
                char language[20];
                printf("Enter the language for which you want to see movies: ");
                scanf("%s", language);
                show_movies_by_language(head, language);
                break;
            }
            case 4:
                printf("Exiting program...\n");
                break;
            default:
                printf("You entered an incorrect choice. Try again.\n");
        }
    } while (choice != 4);

    // Free the memory allocated for movies
    struct movie *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->title);
        for (int i = 0; i < 5; i++) {
            if (temp->languages[i] != NULL) {
                free(temp->languages[i]);
            }
        }
        free(temp);
    }

    return 0;
}
