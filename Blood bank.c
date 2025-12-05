#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FOUR_MONTHS_SECONDS (4 * 30 * 24 * 60 * 60.0)

struct Person {
    char name[50];
    char bloodGroup[5];
    int age;
    float weight;
    char division[20];
    char district[20];
    char contactNumber[15];
    char address[100];
    char lastDonationDateStr[12];
    struct tm lastDonationDate;
    struct Person *next;
};

/* ---------- Utility functions ---------- */

void toUpperString(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = toupper((unsigned char)s[i]);
}

int isValidString(const char *s) {
    if (s[0] == '\0') return 0;
    for (int i = 0; s[i]; i++) {
        if (!isalpha((unsigned char)s[i]) && !isspace((unsigned char)s[i]))
            return 0;
    }
    return 1;
}

int isValidNumericString(const char *s) {
    if (s[0] == '\0') return 0;
    for (int i = 0; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

int isValidBloodGroup(char *bg) {
    const char *valid[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-"};
    char temp[5];
    strncpy(temp, bg, sizeof(temp));
    temp[sizeof(temp)-1] = '\0';
    temp[strcspn(temp, "\n")] = '\0';

    for (int i = 0; temp[i]; i++) {
        if (temp[i] != '+' && temp[i] != '-')
            temp[i] = toupper((unsigned char)temp[i]);
    }
    for (int i = 0; i < 8; i++) {
        if (strcmp(temp, valid[i]) == 0) {
            strcpy(bg, temp);  // normalize (e.g. a+ -> A+)
            return 1;
        }
    }
    return 0;
}

/* ---------- Bangladesh division → district mapping ---------- */

const char *Dhaka[] = {
 "DHAKA","GAZIPUR","NARAYANGANJ","NARSINGDI","MANIKGANJ",
 "MUNSHIGANJ","KISHOREGANJ","TANGAIL","RAJBARI","FARIDPUR",
 "MADARIPUR","SHARIATPUR","GOPALGANJ"
};
const char *Chattogram[] = {
 "CHATTOGRAM","COX'S BAZAR","CUMILLA","FENI","BRAHMANBARIA","CHANDPUR",
 "LAKSHMIPUR","NOAKHALI","KHAGRACHHARI","RANGAMATI","BANDARBAN"
};
const char *Rajshahi[] = {
 "RAJSHAHI","CHAPAI NABABGANJ","NAOGAON","NATORE","PABNA",
 "SIRAJGANJ","JOYPURHAT","BOGURA"
};
const char *Khulna[] = {
 "KHULNA","JASHORE","SATKHIRA","JHENAIDAH","KUSHTIA","MAGURA",
 "NARAIL","MEHERPUR","CHUADANGA","BAGERHAT"
};
const char *Rangpur[] = {
 "RANGPUR","DINAJPUR","LALMONIRHAT","NILPHAMARI","GAIBANDHA",
 "KURIGRAM","PANCHAGARH","THAKURGAON"
};
const char *Sylhet[] = {
 "SYLHET","HABIGANJ","MAULVIBAZAR","SUNAMGANJ"
};
const char *Mymensingh[] = {
 "MYMENSINGH","JAMALPUR","SHERPUR","NETRAKONA"
};
const char *Barishal[] = {
 "BARISHAL","BHOLA","JHALOKATI","PATUAKHALI","PIROJPUR","BARGUNA"
};

int isValidDivision(char *div) {
    const char *valid[] = {
        "DHAKA","CHATTOGRAM","RAJSHAHI","KHULNA",
        "RANGPUR","SYLHET","MYMENSINGH","BARISHAL"
    };

    div[strcspn(div, "\n")] = '\0';
    toUpperString(div);

    for (int i = 0; i < 8; i++) {
        if (strcmp(div, valid[i]) == 0)
            return 1;
    }
    return 0;
}

int districtInDivision(char *dist, const char *div) {
    dist[strcspn(dist, "\n")] = '\0';
    toUpperString(dist);

    const char **list = NULL;
    int count = 0;

    if (strcmp(div, "DHAKA") == 0) { list = Dhaka; count = 13; }
    else if (strcmp(div, "CHATTOGRAM") == 0) { list = Chattogram; count = 11; }
    else if (strcmp(div, "RAJSHAHI") == 0) { list = Rajshahi; count = 8; }
    else if (strcmp(div, "KHULNA") == 0) { list = Khulna; count = 10; }
    else if (strcmp(div, "RANGPUR") == 0) { list = Rangpur; count = 8; }
    else if (strcmp(div, "SYLHET") == 0) { list = Sylhet; count = 4; }
    else if (strcmp(div, "MYMENSINGH") == 0) { list = Mymensingh; count = 4; }
    else if (strcmp(div, "BARISHAL") == 0) { list = Barishal; count = 6; }

    for (int i = 0; i < count; i++) {
        if (strcmp(dist, list[i]) == 0)
            return 1;
    }
    return 0;
}

/* ---------- BMI ---------- */

float calculateBMI(float w, float h) {
    if (h <= 0) return 0.0f;
    return w / (h * h);
}

/* ---------- Linked list ops ---------- */

struct Person* createPerson(char name[], char bg[], int age, float weight,
                            char div[], char dist[],
                            char phone[], char addr[]) {
    struct Person *p = malloc(sizeof(struct Person));
    if (!p) { perror("malloc"); exit(1); }

    strncpy(p->name, name, sizeof(p->name)); p->name[sizeof(p->name)-1] = '\0';
    strncpy(p->bloodGroup, bg, sizeof(p->bloodGroup)); p->bloodGroup[sizeof(p->bloodGroup)-1] = '\0';
    p->age = age;
    p->weight = weight;
    strncpy(p->division, div, sizeof(p->division)); p->division[sizeof(p->division)-1] = '\0';
    strncpy(p->district, dist, sizeof(p->district)); p->district[sizeof(p->district)-1] = '\0';
    strncpy(p->contactNumber, phone, sizeof(p->contactNumber)); p->contactNumber[sizeof(p->contactNumber)-1] = '\0';
    strncpy(p->address, addr, sizeof(p->address)); p->address[sizeof(p->address)-1] = '\0';

    strcpy(p->lastDonationDateStr, "N/A");
    memset(&p->lastDonationDate, 0, sizeof(struct tm));
    p->next = NULL;
    return p;
}

void insertPerson(struct Person **head, struct Person *p) {
    if (*head == NULL) { *head = p; return; }
    struct Person *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = p;
}

/* ---------- File save/load (pipe-separated) ---------- */

void saveToFile(struct Person *head) {
    FILE *file = fopen("blood_donors.txt", "w");
    if (!file) { perror("fopen"); return; }

    struct Person *p = head;
    while (p) {
        long t = 0;
        if (strcmp(p->lastDonationDateStr, "N/A") != 0)
            t = mktime(&p->lastDonationDate);

        fprintf(file, "%s|%s|%d|%.2f|%s|%s|%s|%s|%s|%ld\n",
                p->name,
                p->bloodGroup,
                p->age,
                p->weight,
                p->division,
                p->district,
                p->contactNumber,
                p->address,
                p->lastDonationDateStr,
                t);
        p = p->next;
    }

    fclose(file);
}

void loadFromFile(struct Person **head) {
    FILE *file = fopen("blood_donors.txt", "r");
    if (!file) return;

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char *token;
        char name[50], bg[5], div[20], dist[20], phone[15], addr[100], lastStr[12];
        int age; float weight; long t;

        token = strtok(line, "|"); if (!token) continue; strncpy(name, token, sizeof(name)); name[sizeof(name)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; strncpy(bg, token, sizeof(bg)); bg[sizeof(bg)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; age = atoi(token);
        token = strtok(NULL, "|"); if (!token) continue; weight = (float)atof(token);
        token = strtok(NULL, "|"); if (!token) continue; strncpy(div, token, sizeof(div)); div[sizeof(div)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; strncpy(dist, token, sizeof(dist)); dist[sizeof(dist)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; strncpy(phone, token, sizeof(phone)); phone[sizeof(phone)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; strncpy(addr, token, sizeof(addr)); addr[sizeof(addr)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; strncpy(lastStr, token, sizeof(lastStr)); lastStr[sizeof(lastStr)-1] = '\0';
        token = strtok(NULL, "|"); if (!token) continue; t = atol(token);

        struct Person *p = createPerson(name, bg, age, weight, div, dist, phone, addr);

        if (t > 0 && strcmp(lastStr, "N/A") != 0) {
            time_t tt = (time_t)t;
            p->lastDonationDate = *localtime(&tt);
            strncpy(p->lastDonationDateStr, lastStr, sizeof(p->lastDonationDateStr));
            p->lastDonationDateStr[sizeof(p->lastDonationDateStr)-1] = '\0';
        }

        insertPerson(head, p);
    }

    fclose(file);
}

/* ---------- Printing & donation ---------- */

void printDonor(struct Person *p) {
    printf("\n\tName: %s\n", p->name);
    printf("\tBlood Group: %s\n", p->bloodGroup);
    printf("\tAge: %d\n", p->age);
    printf("\tWeight: %.2f\n", p->weight);
    printf("\tDivision: %s\n", p->division);
    printf("\tDistrict: %s\n", p->district);
    printf("\tContact Number: %s\n", p->contactNumber);
    printf("\tAddress: %s\n", p->address);
    printf("\tLast Donation: %s\n\n", p->lastDonationDateStr);
}

void donateBlood(struct Person *p) {
    time_t now; time(&now);
    struct tm *tmNow = localtime(&now);
    p->lastDonationDate = *tmNow;
    strftime(p->lastDonationDateStr, sizeof(p->lastDonationDateStr), "%d/%m/%Y", tmNow);
    printf("\n\tThank you for donating blood, %s!\n", p->name);
}

/* 4-month rule: if last donation < 4 months ago, hide in search */
int eligibleToShow(struct Person *p) {
    if (strcmp(p->lastDonationDateStr, "N/A") == 0) return 1;

    time_t now; time(&now);
    time_t last = mktime(&p->lastDonationDate);
    double diff = difftime(now, last);
    return diff >= FOUR_MONTHS_SECONDS;
}

/* ---------- Search ---------- */

void findDonor(struct Person *head, char bg[], char div[], char dist[]) {
    struct Person *p = head;
    int found = 0;
    while (p) {
        if (strcmp(p->bloodGroup, bg) == 0 &&
            strcmp(p->division, div) == 0 &&
            strcmp(p->district, dist) == 0 &&
            eligibleToShow(p)) {
            printDonor(p);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("\n\tNo eligible donor found.\n");
}

void searchAndDonate(struct Person *head, char bg[], char div[], char dist[]) {
    struct Person *p = head;
    int found = 0;
    while (p) {
        if (strcmp(p->bloodGroup, bg) == 0 &&
            strcmp(p->division, div) == 0 &&
            strcmp(p->district, dist) == 0 &&
            eligibleToShow(p)) {

            printDonor(p);
            char buf[10];
            printf("\tDonate now? (y/n): ");
            if (fgets(buf, sizeof(buf), stdin)) {
                if (buf[0] == 'y' || buf[0] == 'Y') {
                    donateBlood(p);
                    saveToFile(head);
                }
            }
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("\n\tNo eligible donor found.\n");
}

/* ---------- Safe numeric input ---------- */

int readInt(const char *msg) {
    char buf[32];
    while (1) {
        printf("%s", msg);
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        buf[strcspn(buf, "\n")] = '\0';
        if (isValidNumericString(buf)) return atoi(buf);
        printf("Invalid number, try again.\n");
    }
}

float readFloat(const char *msg) {
    char buf[32];
    while (1) {
        printf("%s", msg);
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) return (float)atof(buf);
        printf("Invalid number, try again.\n");
    }
}

void freeList(struct Person *head) {
    while (head) {
        struct Person *tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* ---------- MAIN ---------- */

int main(void) {
    struct Person *head = NULL;
    loadFromFile(&head);

    // Style 1 banner
    printf("\n\n");
    printf("=================================================================\n");
    printf("||                                                             ||\n");
    printf("||                     🔴  BLOOD DONATION BANK  🔴              ||\n");
    printf("||                                                             ||\n");
    printf("=================================================================\n");
    printf("||          SAVE LIFE • DONATE BLOOD • BE A REAL HERO         ||\n");
    printf("=================================================================\n\n");

    while (1) {
        printf("\n1. Save Information\n");
        printf("2. Search Donor\n");
        printf("3. Donate Blood\n");
        printf("4. Exit\n");

        int choice = readInt("\nEnter your choice: ");

        if (choice == 1) {
            char name[50], bg[5], div[20], dist[20], phone[15], addr[100];
            int age; float weight;

            printf("\nEnter Name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';
            if (!isValidString(name)) { printf("Invalid name.\n"); continue; }

            while (1) {
                printf("Enter Blood Group: ");
                fgets(bg, sizeof(bg), stdin);
                bg[strcspn(bg, "\n")] = '\0';
                if (isValidBloodGroup(bg)) break;
                printf("Invalid blood group.\n");
            }

            age = readInt("Enter Age: ");
            if (age < 18 || age > 45) { printf("Age must be 18–45.\n"); continue; }

            float h = readFloat("Enter Height (meters): ");
            float wtemp = readFloat("Enter Weight for BMI: ");
            float bmi = calculateBMI(wtemp, h);
            if (bmi < 18.5f) { printf("Underweight. Registration cancelled.\n"); continue; }

            weight = readFloat("Enter Actual Weight to Save: ");
            if (weight < 55.0f) { printf("Weight must be at least 55kg.\n"); continue; }

            while (1) {
                printf("Enter Division: ");
                fgets(div, sizeof(div), stdin);
                if (isValidDivision(div)) break;
                printf("Invalid division.\n");
            }
            toUpperString(div);

            while (1) {
                printf("Enter District: ");
                fgets(dist, sizeof(dist), stdin);
                if (districtInDivision(dist, div)) break;
                printf("Invalid district for this division.\n");
            }
            dist[strcspn(dist, "\n")] = '\0';
            toUpperString(dist);

            printf("Enter Contact Number: ");
            fgets(phone, sizeof(phone), stdin);
            phone[strcspn(phone, "\n")] = '\0';
            if (!isValidNumericString(phone)) { printf("Invalid phone.\n"); continue; }

            printf("Enter Address: ");
            fgets(addr, sizeof(addr), stdin);
            addr[strcspn(addr, "\n")] = '\0';

            struct Person *p = createPerson(name, bg, age, weight, div, dist, phone, addr);
            insertPerson(&head, p);
            saveToFile(head);
            printf("\nRegistration successful.\n");
        }
        else if (choice == 2) {
            char bg[5], div[20], dist[20];

            while (1) {
                printf("\nEnter Blood Group: ");
                fgets(bg, sizeof(bg), stdin);
                bg[strcspn(bg, "\n")] = '\0';
                if (isValidBloodGroup(bg)) break;
                printf("Invalid blood group.\n");
            }
            while (1) {
                printf("Enter Division: ");
                fgets(div, sizeof(div), stdin);
                if (isValidDivision(div)) break;
                printf("Invalid division.\n");
            }
            toUpperString(div);
            while (1) {
                printf("Enter District: ");
                fgets(dist, sizeof(dist), stdin);
                if (districtInDivision(dist, div)) break;
                printf("Invalid district for this division.\n");
            }
            dist[strcspn(dist, "\n")] = '\0';
            toUpperString(dist);

            findDonor(head, bg, div, dist);
        }
        else if (choice == 3) {
            char bg[5], div[20], dist[20];

            while (1) {
                printf("\nEnter Blood Group: ");
                fgets(bg, sizeof(bg), stdin);
                bg[strcspn(bg, "\n")] = '\0';
                if (isValidBloodGroup(bg)) break;
                printf("Invalid blood group.\n");
            }
            while (1) {
                printf("Enter Division: ");
                fgets(div, sizeof(div), stdin);
                if (isValidDivision(div)) break;
                printf("Invalid division.\n");
            }
            toUpperString(div);
            while (1) {
                printf("Enter District: ");
                fgets(dist, sizeof(dist), stdin);
                if (districtInDivision(dist, div)) break;
                printf("Invalid district for this division.\n");
            }
            dist[strcspn(dist, "\n")] = '\0';
            toUpperString(dist);

            searchAndDonate(head, bg, div, dist);
        }
        else if (choice == 4) {
            saveToFile(head);
            freeList(head);
            printf("Exiting...\n");
            break;
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
