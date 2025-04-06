/*
Group Members:
Name                ID
---------------------------
Dimetri Lee         2404379
Twayne Campbell     2405399
Sanjay Stewart      2408878
Shanessa Wallace    2404874
*/


/*
 * National Water Commission (NWC) Utility Platform
 * University of Technology, Jamaica
 * School of Computing and Information Technology (SCIT)
 * Programming II - Group Project
 * Date: April 6, 2025
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <ctype.h>
 #include <stdbool.h>
 
 #define MAX_CUSTOMERS 100
 #define MAX_PREMISES 200
 #define MAX_NAME_LENGTH 50
 #define MAX_EMAIL_LENGTH 100
 #define MAX_PASSWORD_LENGTH 50
 #define MAX_BILLING_CYCLES 12
 #define FILE_CUSTOMERS "customers.dat"
 #define FILE_PREMISES "premises.dat"
 #define FILE_BILLS "bills.dat"
 #define FILE_PAYMENTS "payments.dat"
 #define FILE_USERS "users.dat"
 #define FILE_PAYMENT_CARDS "payment_cards.dat"
 #define FILE_LOGS "system_logs.dat"
 
 // Enumeration for user types
 typedef enum {
     AGENT = 1,
     CUSTOMER = 2
 } UserType;
 
 // Enumeration for income classes
 typedef enum {
     LOW = 1,           // Up to 125 L daily
     LOW_MEDIUM = 2,    // Up to 175 L daily
     MEDIUM = 3,        // Up to 220 L daily
     MEDIUM_HIGH = 4,   // Up to 250 L daily
     HIGH = 5           // Up to 300 L daily
 } IncomeClass;
 
 // Enumeration for meter sizes
 typedef enum {
     METER_15MM = 1,    // 15mm: $1,155.92 service charge
     METER_30MM = 2,    // 30mm: $6,217.03 service charge
     METER_150MM = 3    // 150mm: $39,354.59 service charge
 } MeterSize;
 
 // Structure for user accounts
 typedef struct {
     int id;
     char email[MAX_EMAIL_LENGTH];
     char password[MAX_PASSWORD_LENGTH];
     UserType type;
     bool is_active;
 } User;
 
 // Structure for customers
 typedef struct {
     char customer_number[8];  // 7-digit plus null terminator
     char first_name[MAX_NAME_LENGTH];
     char last_name[MAX_NAME_LENGTH];
     int user_id;
     IncomeClass income_class;
     bool is_active;
     bool has_payment_card;
 } Customer;
 
 // Structure for premises
 typedef struct {
     char premises_number[8];  // 7-digit plus null terminator
     char customer_number[8];
     MeterSize meter_size;
     int initial_reading;
     int previous_reading;
     int current_reading;
     bool is_active;
 } Premises;
 
 // Structure for bills
 typedef struct {
     char bill_id[20];
     char customer_number[8];
     char premises_number[8];
     int month_number;
     int year;
     int previous_reading;
     int current_reading;
     int consumption;
     double water_charge;
     double sewerage_charge;
     double service_charge;
     double pam;
     double x_factor;
     double k_factor;
     double total_current_charges;
     double early_payment_amount;
     double overdue_amount;
     double total_amount_due;
     double amount_paid;
     bool is_early_payment_eligible;
     bool is_paid;
     char bill_date[11];
     char due_date[11];
 } Bill;
 
 // Structure for payments
 typedef struct {
     char payment_id[20];
     char bill_id[20];
     char customer_number[8];
     char premises_number[8];
     double amount;
     char payment_date[11];
 } Payment;
 
 // Structure for payment cards
 typedef struct {
     char customer_number[8];
     char card_identifier[20];
     bool is_active;
 } PaymentCard;
 
 // Structure for system logs
 typedef struct {
     char log_id[20];
     char customer_number[8];
     int payments_count;
     double last_payment_amount;
     int meters_surrendered;
     char log_date[11];
 } SystemLog;
 
 // Global variables
 Customer customers[MAX_CUSTOMERS];
 Premises premises[MAX_PREMISES];
 User current_user;
 Customer current_customer;
 int customer_count = 0;
 int premises_count = 0;
 
 // Function prototypes
 void initializeSystem();
 void mainMenu();
 void registerAccount();
 bool signIn();
 void agentInterface();
 void customerInterface();
 void addCustomer();
 void editCustomer();
 void viewCustomer();
 void deleteCustomer();
 void generateBill();
 void viewReports();
 void registerPaymentCard();
 void viewBill();
 void payBill();
 void surrenderMeter();
 void loadData();
 void saveData();
 void generateID(char *id, const char *prefix);
 void getCurrentDate(char *date);
 int generateRandomNumber(int min, int max);
 float calculateWaterCharge(int consumption);
 float calculateSewerageCharge(int consumption);
 float calculateServiceCharge(MeterSize meter_size);
 bool isCustomerNumberExists(const char *customer_number);
 bool isPremisesNumberExists(const char *premises_number);
 bool isEmailExists(const char *email);
 void maskPassword(char *password);
 int getDailyUsageLimit(IncomeClass income_class);
 void logActivity(const char *customer_number, double payment_amount, bool surrender_meter);
 void displayCustomerDetails(const char *customer_number);
 void clearScreen();
 void pauseScreen();
 
 int main() {
     srand(time(NULL));
     initializeSystem();
     mainMenu();
     saveData();
     return 0;
 }
 
 // Initialize the system by loading data
 void initializeSystem() {
     loadData();
     printf("\nWelcome to the National Water Commission (NWC) Utility Platform\n");
 }
 
 // Main menu
 void mainMenu() {
     int choice;
     bool running = true;
     
     while (running) {
         clearScreen();
         printf("\n=== NWC Utility Platform ===\n");
         printf("1. Register Account\n");
         printf("2. Sign In\n");
         printf("3. Exit\n");
         printf("Please enter your choice: ");
         scanf("%d", &choice);
         getchar(); // Consume newline
         
         switch (choice) {
             case 1:
                 registerAccount();
                 break;
             case 2:
                 if (signIn()) {
                     if (current_user.type == AGENT) {
                         agentInterface();
                     } else {
                         customerInterface();
                     }
                 }
                 break;
             case 3:
                 running = false;
                 printf("Thank you for using the NWC Utility Platform\n");
                 break;
             default:
                 printf("Invalid choice. Please try again.\n");
                 pauseScreen();
         }
     }
 }
 
 // Register new account
 void registerAccount() {
     clearScreen();
     User new_user;
     Customer new_customer;
     char email[MAX_EMAIL_LENGTH];
     char password[MAX_PASSWORD_LENGTH];
     char first_name[MAX_NAME_LENGTH];
     char last_name[MAX_NAME_LENGTH];
     
     printf("\n=== Register New Account ===\n");
     
     printf("Enter email: ");
     fgets(email, MAX_EMAIL_LENGTH, stdin);
     email[strcspn(email, "\n")] = '\0';
     
     // Check if email already exists
     if (isEmailExists(email)) {
         printf("Email already exists. Please use a different email.\n");
         pauseScreen();
         return;
     }
     
     printf("Enter password: ");
     maskPassword(password);
     
     printf("Enter first name: ");
     fgets(first_name, MAX_NAME_LENGTH, stdin);
     first_name[strcspn(first_name, "\n")] = '\0';
     
     printf("Enter last name: ");
     fgets(last_name, MAX_NAME_LENGTH, stdin);
     last_name[strcspn(last_name, "\n")] = '\0';
     
     // Create new user
     new_user.id = rand() % 10000 + 1000;
     strcpy(new_user.email, email);
     strcpy(new_user.password, password);
     new_user.type = CUSTOMER;
     new_user.is_active = true;
     
     // Save user to file
     FILE *file = fopen(FILE_USERS, "ab");
     if (file != NULL) {
         fwrite(&new_user, sizeof(User), 1, file);
         fclose(file);
         
         // Create new customer
         sprintf(new_customer.customer_number, "%07d", rand() % 9000000 + 1000000);
         strcpy(new_customer.first_name, first_name);
         strcpy(new_customer.last_name, last_name);
         new_customer.user_id = new_user.id;
         new_customer.income_class = (IncomeClass)(rand() % 5 + 1); // Random income class
         new_customer.is_active = true;
         new_customer.has_payment_card = false;
         
         // Save customer to array and file
         customers[customer_count] = new_customer;
         customer_count++;
         
         file = fopen(FILE_CUSTOMERS, "ab");
         if (file != NULL) {
             fwrite(&new_customer, sizeof(Customer), 1, file);
             fclose(file);
             printf("\nAccount successfully registered!\n");
             printf("Your customer number is: %s\n", new_customer.customer_number);
         } else {
             printf("Error: Could not save customer data.\n");
         }
     } else {
         printf("Error: Could not save user data.\n");
     }
     
     pauseScreen();
 }
 
 // Sign in to the system
 bool signIn() {
     clearScreen();
     char email[MAX_EMAIL_LENGTH];
     char password[MAX_PASSWORD_LENGTH];
     bool authenticated = false;
     
     printf("\n=== Sign In ===\n");
     printf("Enter email: ");
     fgets(email, MAX_EMAIL_LENGTH, stdin);
     email[strcspn(email, "\n")] = '\0';
     
     printf("Enter password: ");
     maskPassword(password);
     
     // Check credentials
     FILE *file = fopen(FILE_USERS, "rb");
     if (file != NULL) {
         User user;
         while (fread(&user, sizeof(User), 1, file) == 1) {
             if (strcmp(user.email, email) == 0 && strcmp(user.password, password) == 0 && user.is_active) {
                 authenticated = true;
                 current_user = user;
                 
                 // If user is a customer, find the customer record
                 if (user.type == CUSTOMER) {
                     FILE *customer_file = fopen(FILE_CUSTOMERS, "rb");
                     if (customer_file != NULL) {
                         Customer customer;
                         while (fread(&customer, sizeof(Customer), 1, customer_file) == 1) {
                             if (customer.user_id == user.id && customer.is_active) {
                                 current_customer = customer;
                                 break;
                             }
                         }
                         fclose(customer_file);
                     }
                 }
                 
                 break;
             }
         }
         fclose(file);
     }
     
     if (authenticated) {
         printf("\nSign in successful!\n");
         if (current_user.type == AGENT) {
             printf("Welcome, Agent!\n");
         } else {
             printf("Welcome, %s %s!\n", current_customer.first_name, current_customer.last_name);
         }
         pauseScreen();
         return true;
     } else {
         printf("\nInvalid email or password. Please try again.\n");
         pauseScreen();
         return false;
     }
 }
 
 // Agent interface
 void agentInterface() {
     int choice;
     bool running = true;
     
     while (running) {
         clearScreen();
         printf("\n=== Agent Interface ===\n");
         printf("1. Add Customer\n");
         printf("2. Edit Customer\n");
         printf("3. View Customer\n");
         printf("4. Delete/Archive Customer\n");
         printf("5. Generate Bill\n");
         printf("6. View Reports\n");
         printf("7. Logout\n");
         printf("Please enter your choice: ");
         scanf("%d", &choice);
         getchar(); // Consume newline
         
         switch (choice) {
             case 1:
                 addCustomer();
                 break;
             case 2:
                 editCustomer();
                 break;
             case 3:
                 viewCustomer();
                 break;
             case 4:
                 deleteCustomer();
                 break;
             case 5:
                 generateBill();
                 break;
             case 6:
                 viewReports();
                 break;
             case 7:
                 running = false;
                 printf("Logged out successfully.\n");
                 pauseScreen();
                 break;
             default:
                 printf("Invalid choice. Please try again.\n");
                 pauseScreen();
         }
     }
 }
 
 // Customer interface
 void customerInterface() {
     int choice;
     bool running = true;
     
     while (running) {
         clearScreen();
         printf("\n=== Customer Interface ===\n");
         printf("1. Register Payment Card\n");
         printf("2. View Bill\n");
         printf("3. Pay Bill\n");
         printf("4. Surrender Meter\n");
         printf("5. Logout\n");
         printf("Please enter your choice: ");
         scanf("%d", &choice);
         getchar(); // Consume newline
         
         switch (choice) {
             case 1:
                 registerPaymentCard();
                 break;
             case 2:
                 viewBill();
                 break;
             case 3:
                 payBill();
                 break;
             case 4:
                 surrenderMeter();
                 break;
             case 5:
                 running = false;
                 printf("Logged out successfully.\n");
                 pauseScreen();
                 break;
             default:
                 printf("Invalid choice. Please try again.\n");
                 pauseScreen();
         }
     }
 }
 
 // Add a new customer (Agent function)
 void addCustomer() {
     clearScreen();
     Customer new_customer;
     Premises new_premises;
     char customer_number[8];
     char premises_number[8];
     int meter_size_choice;
     int first_reading;
     char first_name[MAX_NAME_LENGTH];
     char last_name[MAX_NAME_LENGTH];
     int income_class_choice;
     
     printf("\n=== Add Customer ===\n");
     
     // Customer Number
     printf("Enter Customer Number (7 digits): ");
     fgets(customer_number, 8, stdin);
     customer_number[strcspn(customer_number, "\n")] = '\0';
     
     // Validate customer number
     if (strlen(customer_number) != 7) {
         printf("Error: Customer number must be 7 digits.\n");
         pauseScreen();
         return;
     }
     
     for (int i = 0; i < 7; i++) {
         if (!isdigit(customer_number[i])) {
             printf("Error: Customer number must contain only digits.\n");
             pauseScreen();
             return;
         }
     }
     
     if (isCustomerNumberExists(customer_number)) {
         printf("Error: Customer number already exists.\n");
         pauseScreen();
         return;
     }
     
     // Premises Number
     printf("Enter Premises Number (7 digits): ");
     fgets(premises_number, 8, stdin);
     premises_number[strcspn(premises_number, "\n")] = '\0';
     
     // Validate premises number
     if (strlen(premises_number) != 7) {
         printf("Error: Premises number must be 7 digits.\n");
         pauseScreen();
         return;
     }
     
     for (int i = 0; i < 7; i++) {
         if (!isdigit(premises_number[i])) {
             printf("Error: Premises number must contain only digits.\n");
             pauseScreen();
             return;
         }
     }
     
     if (isPremisesNumberExists(premises_number)) {
         printf("Error: Premises number already exists.\n");
         pauseScreen();
         return;
     }
     
     // Service Name
     printf("Enter First Name: ");
     fgets(first_name, MAX_NAME_LENGTH, stdin);
     first_name[strcspn(first_name, "\n")] = '\0';
     
     printf("Enter Last Name: ");
     fgets(last_name, MAX_NAME_LENGTH, stdin);
     last_name[strcspn(last_name, "\n")] = '\0';
     
     // Meter Size
     printf("Select Meter Size:\n");
     printf("1. 15mm\n");
     printf("2. 30mm\n");
     printf("3. 150mm\n");
     printf("Choice: ");
     scanf("%d", &meter_size_choice);
     getchar(); // Consume newline
     
     if (meter_size_choice < 1 || meter_size_choice > 3) {
         printf("Error: Invalid meter size selection.\n");
         pauseScreen();
         return;
     }
     
     // First Reading
     printf("Enter First Reading (initial meter reading): ");
     scanf("%d", &first_reading);
     getchar(); // Consume newline
     
     // Income Class
     printf("Select Income Class:\n");
     printf("1. Low (daily usage up to 125 L)\n");
     printf("2. Low/Medium (daily usage up to 175 L)\n");
     printf("3. Medium (daily usage up to 220 L)\n");
     printf("4. Medium/High (daily usage up to 250 L)\n");
     printf("5. High (daily usage up to 300 L)\n");
     printf("Choice: ");
     scanf("%d", &income_class_choice);
     getchar(); // Consume newline
     
     if (income_class_choice < 1 || income_class_choice > 5) {
         printf("Error: Invalid income class selection.\n");
         pauseScreen();
         return;
     }
     
     // Create new customer
     strcpy(new_customer.customer_number, customer_number);
     strcpy(new_customer.first_name, first_name);
     strcpy(new_customer.last_name, last_name);
     new_customer.user_id = 0; // No user account associated yet
     new_customer.income_class = (IncomeClass)income_class_choice;
     new_customer.is_active = true;
     new_customer.has_payment_card = false;
     
     // Create new premises
     strcpy(new_premises.premises_number, premises_number);
     strcpy(new_premises.customer_number, customer_number);
     new_premises.meter_size = (MeterSize)meter_size_choice;
     new_premises.initial_reading = first_reading;
     new_premises.previous_reading = first_reading;
     new_premises.current_reading = first_reading;
     new_premises.is_active = true;
     
     // Save customer to array and file
     customers[customer_count] = new_customer;
     customer_count++;
     
     FILE *file = fopen(FILE_CUSTOMERS, "ab");
     if (file != NULL) {
         fwrite(&new_customer, sizeof(Customer), 1, file);
         fclose(file);
     } else {
         printf("Error: Could not save customer data.\n");
         pauseScreen();
         return;
     }
     
     // Save premises to array and file
     premises[premises_count] = new_premises;
     premises_count++;
     
     file = fopen(FILE_PREMISES, "ab");
     if (file != NULL) {
         fwrite(&new_premises, sizeof(Premises), 1, file);
         fclose(file);
         printf("\nCustomer and premises added successfully!\n");
     } else {
         printf("Error: Could not save premises data.\n");
     }
     
     pauseScreen();
 }
 
 // Edit an existing customer (Agent function)
 void editCustomer() {
     clearScreen();
     char customer_number[8];
     int choice;
     bool found = false;
     int index = -1;
     
     printf("\n=== Edit Customer ===\n");
     printf("Enter Customer Number to edit: ");
     fgets(customer_number, 8, stdin);
     customer_number[strcspn(customer_number, "\n")] = '\0';
     
     // Find customer in array
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].customer_number, customer_number) == 0 && customers[i].is_active) {
             found = true;
             index = i;
             break;
         }
     }
     
     if (!found) {
         printf("Customer not found or is archived.\n");
         pauseScreen();
         return;
     }
     
     // Display customer details
     printf("\nCustomer Details:\n");
     printf("Customer Number: %s\n", customers[index].customer_number);
     printf("Name: %s %s\n", customers[index].first_name, customers[index].last_name);
     printf("Income Class: %d\n", customers[index].income_class);
     
     printf("\nWhat would you like to edit?\n");
     printf("1. First Name\n");
     printf("2. Last Name\n");
     printf("3. Income Class\n");
     printf("4. All Fields\n");
     printf("5. Cancel\n");
     printf("Choice: ");
     scanf("%d", &choice);
     getchar(); // Consume newline
     
     switch (choice) {
         case 1: {
             char new_first_name[MAX_NAME_LENGTH];
             printf("Enter new First Name: ");
             fgets(new_first_name, MAX_NAME_LENGTH, stdin);
             new_first_name[strcspn(new_first_name, "\n")] = '\0';
             strcpy(customers[index].first_name, new_first_name);
             break;
         }
         case 2: {
             char new_last_name[MAX_NAME_LENGTH];
             printf("Enter new Last Name: ");
             fgets(new_last_name, MAX_NAME_LENGTH, stdin);
             new_last_name[strcspn(new_last_name, "\n")] = '\0';
             strcpy(customers[index].last_name, new_last_name);
             break;
         }
         case 3: {
             int new_income_class;
             printf("Select new Income Class:\n");
             printf("1. Low (daily usage up to 125 L)\n");
             printf("2. Low/Medium (daily usage up to 175 L)\n");
             printf("3. Medium (daily usage up to 220 L)\n");
             printf("4. Medium/High (daily usage up to 250 L)\n");
             printf("5. High (daily usage up to 300 L)\n");
             printf("Choice: ");
             scanf("%d", &new_income_class);
             getchar(); // Consume newline
             
             if (new_income_class < 1 || new_income_class > 5) {
                 printf("Invalid income class selection. No changes made.\n");
             } else {
                 customers[index].income_class = (IncomeClass)new_income_class;
             }
             break;
         }
         case 4: {
             char new_first_name[MAX_NAME_LENGTH];
             char new_last_name[MAX_NAME_LENGTH];
             int new_income_class;
             
             printf("Enter new First Name: ");
             fgets(new_first_name, MAX_NAME_LENGTH, stdin);
             new_first_name[strcspn(new_first_name, "\n")] = '\0';
             
             printf("Enter new Last Name: ");
             fgets(new_last_name, MAX_NAME_LENGTH, stdin);
             new_last_name[strcspn(new_last_name, "\n")] = '\0';
             
             printf("Select new Income Class:\n");
             printf("1. Low (daily usage up to 125 L)\n");
             printf("2. Low/Medium (daily usage up to 175 L)\n");
             printf("3. Medium (daily usage up to 220 L)\n");
             printf("4. Medium/High (daily usage up to 250 L)\n");
             printf("5. High (daily usage up to 300 L)\n");
             printf("Choice: ");
             scanf("%d", &new_income_class);
             getchar(); // Consume newline
             
             if (new_income_class < 1 || new_income_class > 5) {
                 printf("Invalid income class selection. No changes made to income class.\n");
             } else {
                 strcpy(customers[index].first_name, new_first_name);
                 strcpy(customers[index].last_name, new_last_name);
                 customers[index].income_class = (IncomeClass)new_income_class;
             }
             break;
         }
         case 5:
             printf("Edit canceled.\n");
             pauseScreen();
             return;
         default:
             printf("Invalid choice. No changes made.\n");
             pauseScreen();
             return;
     }
     
     // Update customer in file
     FILE *file = fopen(FILE_CUSTOMERS, "wb");
     if (file != NULL) {
         for (int i = 0; i < customer_count; i++) {
             fwrite(&customers[i], sizeof(Customer), 1, file);
         }
         fclose(file);
         printf("Customer updated successfully!\n");
     } else {
         printf("Error: Could not update customer data.\n");
     }
     
     pauseScreen();
 }
 
 // View customer details (Agent function)
 void viewCustomer() {
     clearScreen();
     char customer_number[8];
     
     printf("\n=== View Customer ===\n");
     printf("Enter Customer Number: ");
     fgets(customer_number, 8, stdin);
     customer_number[strcspn(customer_number, "\n")] = '\0';
     
     displayCustomerDetails(customer_number);
     pauseScreen();
 }
 
 // Delete/Archive a customer (Agent function)
 void deleteCustomer() {
     clearScreen();
     char customer_number[8];
     bool found = false;
     int index = -1;
     
     printf("\n=== Delete/Archive Customer ===\n");
     printf("Enter Customer Number to archive: ");
     fgets(customer_number, 8, stdin);
     customer_number[strcspn(customer_number, "\n")] = '\0';
     
     // Find customer in array
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].customer_number, customer_number) == 0 && customers[i].is_active) {
             found = true;
             index = i;
             break;
         }
     }
     
     if (!found) {
         printf("Customer not found or already archived.\n");
         pauseScreen();
         return;
     }
     
     // Archive customer (set is_active to false)
     customers[index].is_active = false;
     
     // Update customer in file
     FILE *file = fopen(FILE_CUSTOMERS, "wb");
     if (file != NULL) {
         for (int i = 0; i < customer_count; i++) {
             fwrite(&customers[i], sizeof(Customer), 1, file);
         }
         fclose(file);
         
         // Archive associated premises
         for (int i = 0; i < premises_count; i++) {
             if (strcmp(premises[i].customer_number, customer_number) == 0 && premises[i].is_active) {
                 premises[i].is_active = false;
             }
         }
         
         // Update premises in file
         file = fopen(FILE_PREMISES, "wb");
         if (file != NULL) {
             for (int i = 0; i < premises_count; i++) {
                 fwrite(&premises[i], sizeof(Premises), 1, file);
             }
             fclose(file);
             printf("Customer archived successfully!\n");
         } else {
             printf("Error: Could not update premises data.\n");
         }
     } else {
         printf("Error: Could not update customer data.\n");
     }
     
     pauseScreen();
 }
 
 // Generate bill for a customer (Agent function)
 void generateBill() {
     clearScreen();
     char customer_number[8];
     char premises_number[8];
     bool customer_found = false;
     bool premises_found = false;
     int customer_index = -1;
     int premises_index = -1;
     int unpaid_bills_count = 0;
     
     printf("\n=== Generate Bill ===\n");
     printf("Enter Customer Number: ");
     fgets(customer_number, 8, stdin);
     customer_number[strcspn(customer_number, "\n")] = '\0';
     
     // Find customer in array
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].customer_number, customer_number) == 0 && customers[i].is_active) {
             customer_found = true;
             customer_index = i;
             break;
         }
     }
     
     if (!customer_found) {
         printf("Customer not found or is archived.\n");
         pauseScreen();
         return;
     }
     
     printf("Enter Premises Number: ");
     fgets(premises_number, 8, stdin);
     premises_number[strcspn(premises_number, "\n")] = '\0';
     
     // Find premises in array
     for (int i = 0; i < premises_count; i++) {
         if (strcmp(premises[i].premises_number, premises_number) == 0 && 
             strcmp(premises[i].customer_number, customer_number) == 0 && 
             premises[i].is_active) {
             premises_found = true;
             premises_index = i;
             break;
         }
     }
     
     if (!premises_found) {
         printf("Premises not found, not associated with this customer, or is inactive.\n");
         pauseScreen();
         return;
     }
     
     // Check for two consecutive unpaid bills
     FILE *file = fopen(FILE_BILLS, "rb");
     if (file != NULL) {
         Bill bill;
         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
             if (strcmp(bill.customer_number, customer_number) == 0 && 
                 strcmp(bill.premises_number, premises_number) == 0 && 
                 !bill.is_paid) {
                 unpaid_bills_count++;
             }
         }
         fclose(file);
     }
     
     if (unpaid_bills_count >= 2) {
         printf("Cannot generate bill: Customer has two or more unpaid bills.\n");
         pauseScreen();
         return;
     }
     
     // Generate bill
     Bill new_bill;
     int total_consumption = 0;
     int daily_usage_limit = getDailyUsageLimit(customers[customer_index].income_class);
     
     // Generate 30 days of consumption
     for (int i = 0; i < 30; i++) {
         total_consumption += generateRandomNumber(0, daily_usage_limit);
     }
     
     // Update premises readings
     premises[premises_index].previous_reading = premises[premises_index].current_reading;
     premises[premises_index].current_reading = premises[premises_index].previous_reading + total_consumption;
     
     // Generate bill ID
     generateID(new_bill.bill_id, "BILL");
     
     // Set bill details
     strcpy(new_bill.customer_number, customer_number);
     strcpy(new_bill.premises_number, premises_number);
     
     // Get current date for bill date
     getCurrentDate(new_bill.bill_date);
     
     // Calculate due date (30 days from bill date)
     // For simplicity, we'll just add "30 days" to the bill date
     strcpy(new_bill.due_date, new_bill.bill_date);
     new_bill.due_date[0] = new_bill.due_date[0] + 1; // Simple increment to represent 30 days later
     
     // Set month number (1-12)
     FILE *bill_file = fopen(FILE_BILLS, "rb");
     if (bill_file != NULL) {
         Bill last_bill;
         int last_month = 0;
         
         while (fread(&last_bill, sizeof(Bill), 1, bill_file) == 1) {
             if (strcmp(last_bill.customer_number, customer_number) == 0 && 
                 strcmp(last_bill.premises_number, premises_number) == 0) {
                 if (last_bill.month_number > last_month) {
                     last_month = last_bill.month_number;
                 }
             }
         }
         fclose(bill_file);
         
         if (last_month == 12) {
             new_bill.month_number = 1;
         } else {
             new_bill.month_number = last_month + 1;
         }
     } else {
         new_bill.month_number = 1; // First bill
     }
     
     new_bill.year = 2025; // Current year
     new_bill.previous_reading = premises[premises_index].previous_reading;
     new_bill.current_reading = premises[premises_index].current_reading;
     new_bill.consumption = total_consumption;
     
     // Calculate charges
     new_bill.water_charge = calculateWaterCharge(total_consumption);
     new_bill.sewerage_charge = calculateSewerageCharge(total_consumption);
     new_bill.service_charge = calculateServiceCharge(premises[premises_index].meter_size);
     
     // PAM (Price Adjustment Mechanism): 1.21% of (Water + Sewerage + Service)
     new_bill.pam = 0.0121 * (new_bill.water_charge + new_bill.sewerage_charge + new_bill.service_charge);
     
     // X-Factor: -5% of (Water + Sewerage + Service)
     new_bill.x_factor = -0.05 * (new_bill.water_charge + new_bill.sewerage_charge + new_bill.service_charge);
     
     // K-Factor: 20% of (Water + Sewerage + Service + PAM) - X-Factor
     new_bill.k_factor = 0.2 * (new_bill.water_charge + new_bill.sewerage_charge + new_bill.service_charge + new_bill.pam) - new_bill.x_factor;
     
     // Total Current Charges
     new_bill.total_current_charges = new_bill.water_charge + new_bill.sewerage_charge + new_bill.service_charge - new_bill.x_factor + new_bill.k_factor;
     
     // Determine early payment eligibility (random)
     new_bill.is_early_payment_eligible = (generateRandomNumber(0, 1) == 1);
     
     if (new_bill.is_early_payment_eligible) {
         // Early payment discount (random between $50 and $250)
         new_bill.early_payment_amount = generateRandomNumber(50, 250);
     } else {
         new_bill.early_payment_amount = 0.0;
     }
     
     // Check for overdue amount
     new_bill.overdue_amount = 0.0;
     bill_file = fopen(FILE_BILLS, "rb");
     if (bill_file != NULL) {
         Bill last_bill;
         
         while (fread(&last_bill, sizeof(Bill), 1, bill_file) == 1) {
             if (strcmp(last_bill.customer_number, customer_number) == 0 && 
                 strcmp(last_bill.premises_number, premises_number) == 0 && 
                 !last_bill.is_paid) {
                 new_bill.overdue_amount += (last_bill.total_amount_due - last_bill.amount_paid);
             }
         }
         fclose(bill_file);
     }
     
     // Total Amount Due
     new_bill.total_amount_due = new_bill.total_current_charges - new_bill.early_payment_amount + new_bill.overdue_amount;
     
     new_bill.amount_paid = 0.0;
     new_bill.is_paid = false;
     
     // Save bill to file
     file = fopen(FILE_BILLS, "ab");
     if (file != NULL) {
         fwrite(&new_bill, sizeof(Bill), 1, file);
         fclose(file);
         
         // Update premises in file
         file = fopen(FILE_PREMISES, "wb");
         if (file != NULL) {
             for (int i = 0; i < premises_count; i++) {
                 fwrite(&premises[i], sizeof(Premises), 1, file);
             }
             fclose(file);
             
             printf("\nBill generated successfully!\n");
             printf("Bill ID: %s\n", new_bill.bill_id);
             printf("Customer: %s %s\n", customers[customer_index].first_name, customers[customer_index].last_name);
             printf("Consumption: %d litres\n", total_consumption);
             printf("Total Amount Due: $%.2f\n", new_bill.total_amount_due);
             
             if (new_bill.is_early_payment_eligible) {
                 printf("Early Payment Discount: $%.2f (if paid before due date)\n", new_bill.early_payment_amount);
             }
         } else {
             printf("Error: Could not update premises data.\n");
         }
     } else {
         printf("Error: Could not save bill data.\n");
     }
     
     pauseScreen();
 }
 
 // View reports (Agent function)
 void viewReports() {
     clearScreen();
     int choice;
     
     printf("\n=== View Reports ===\n");
     printf("1. Paid Customers\n");
     printf("2. Owing Customers\n");
     printf("3. Deleted/Archived Customers\n");
     printf("4. Back\n");
     printf("Please enter your choice: ");
     scanf("%d", &choice);
     getchar(); // Consume newline
     
     switch (choice) {
         case 1: {
             clearScreen();
             printf("\n=== Paid Customers Report ===\n");
             printf("%-10s %-10s %-20s %-10s %-10s\n", "Customer", "Premises", "Name", "Month", "Amount");
             printf("--------------------------------------------------------------\n");
             
             FILE *file = fopen(FILE_BILLS, "rb");
             if (file != NULL) {
                 Bill bill;
                 while (fread(&bill, sizeof(Bill), 1, file) == 1) {
                     if (bill.is_paid) {
                         // Find customer name
                         char full_name[MAX_NAME_LENGTH * 2 + 1] = "";
                         for (int i = 0; i < customer_count; i++) {
                             if (strcmp(customers[i].customer_number, bill.customer_number) == 0) {
                                 sprintf(full_name, "%s %s", customers[i].first_name, customers[i].last_name);
                                 break;
                             }
                         }
                         
                         printf("%-10s %-10s %-20s %-10d $%-9.2f\n", 
                                bill.customer_number, 
                                bill.premises_number, 
                                full_name, 
                                bill.month_number, 
                                bill.amount_paid);
                     }
                 }
                 fclose(file);
             } else {
                 printf("No paid bills found.\n");
             }
             break;
         }
         case 2: {
             clearScreen();
             printf("\n=== Owing Customers Report ===\n");
             printf("%-10s %-10s %-20s %-10s %-10s\n", "Customer", "Premises", "Name", "Month", "Amount");
             printf("--------------------------------------------------------------\n");
             
             FILE *file = fopen(FILE_BILLS, "rb");
             if (file != NULL) {
                 Bill bill;
                 while (fread(&bill, sizeof(Bill), 1, file) == 1) {
                     if (!bill.is_paid) {
                         // Find customer name
                         char full_name[MAX_NAME_LENGTH * 2 + 1] = "";
                         for (int i = 0; i < customer_count; i++) {
                             if (strcmp(customers[i].customer_number, bill.customer_number) == 0) {
                                 sprintf(full_name, "%s %s", customers[i].first_name, customers[i].last_name);
                                 break;
                             }
                         }
                         
                         double amount_owing = bill.total_amount_due - bill.amount_paid;
                         
                         printf("%-10s %-10s %-20s %-10d $%-9.2f\n", 
                                bill.customer_number, 
                                bill.premises_number, 
                                full_name, 
                                bill.month_number, 
                                amount_owing);
                     }
                 }
                 fclose(file);
             } else {
                 printf("No owing bills found.\n");
             }
             break;
         }
         case 3: {
             clearScreen();
             printf("\n=== Deleted/Archived Customers Report ===\n");
             printf("%-10s %-10s %-20s %-15s %-15s\n", "Customer", "Premises", "Name", "Balance", "Archive Date");
             printf("-----------------------------------------------------------------------\n");
             
             for (int i = 0; i < customer_count; i++) {
                 if (!customers[i].is_active) {
                     char premises_list[100] = "";
                     double outstanding_balance = 0.0;
                     
                     // Find associated premises
                     for (int j = 0; j < premises_count; j++) {
                         if (strcmp(premises[j].customer_number, customers[i].customer_number) == 0) {
                             strcat(premises_list, premises[j].premises_number);
                             strcat(premises_list, " ");
                         }
                     }
                     
                     // Calculate outstanding balance
                     FILE *file = fopen(FILE_BILLS, "rb");
                     if (file != NULL) {
                         Bill bill;
                         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
                             if (strcmp(bill.customer_number, customers[i].customer_number) == 0 && !bill.is_paid) {
                                 outstanding_balance += (bill.total_amount_due - bill.amount_paid);
                             }
                         }
                         fclose(file);
                     }
                     
                     printf("%-10s %-10s %-20s $%-14.2f %s\n", 
                            customers[i].customer_number, 
                            premises_list, 
                            customers[i].first_name, 
                            outstanding_balance, 
                            "N/A"); // Archive date not tracked in this implementation
                 }
             }
             break;
         }
         case 4:
             return;
         default:
             printf("Invalid choice. Please try again.\n");
     }
     
     pauseScreen();
 }
 
 // Register a payment card (Customer function)
 void registerPaymentCard() {
     clearScreen();
     PaymentCard card;
     char card_identifier[20];
     
     printf("\n=== Register Payment Card ===\n");
     
     // Check if customer already has a registered card
     if (current_customer.has_payment_card) {
         printf("You already have a registered payment card.\n");
         pauseScreen();
         return;
     }
     
     printf("Enter Card Identifier (e.g., last 4 digits): ");
     fgets(card_identifier, 20, stdin);
     card_identifier[strcspn(card_identifier, "\n")] = '\0';
     
     // Create payment card
     strcpy(card.customer_number, current_customer.customer_number);
     strcpy(card.card_identifier, card_identifier);
     card.is_active = true;
     
     // Save payment card to file
     FILE *file = fopen(FILE_PAYMENT_CARDS, "ab");
     if (file != NULL) {
         fwrite(&card, sizeof(PaymentCard), 1, file);
         fclose(file);
         
         // Update customer's has_payment_card flag
         for (int i = 0; i < customer_count; i++) {
             if (strcmp(customers[i].customer_number, current_customer.customer_number) == 0) {
                 customers[i].has_payment_card = true;
                 current_customer.has_payment_card = true;
                 break;
             }
         }
         
         // Update customer file
         file = fopen(FILE_CUSTOMERS, "wb");
         if (file != NULL) {
             for (int i = 0; i < customer_count; i++) {
                 fwrite(&customers[i], sizeof(Customer), 1, file);
             }
             fclose(file);
             printf("Payment card registered successfully!\n");
         } else {
             printf("Error: Could not update customer data.\n");
         }
     } else {
         printf("Error: Could not save payment card data.\n");
     }
     
     pauseScreen();
 }
 
 // View bill (Customer function)
 void viewBill() {
     clearScreen();
     bool bill_found = false;
     Bill latest_bill;
     
     printf("\n=== View Bill ===\n");
     
     // Find the most recent bill for the customer
     FILE *file = fopen(FILE_BILLS, "rb");
     if (file != NULL) {
         Bill bill;
         time_t latest_time = 0;
         
         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
             if (strcmp(bill.customer_number, current_customer.customer_number) == 0) {
                 bill_found = true;
                 
                 // Simple logic to find the most recent bill (based on month number)
                 if (bill.month_number > latest_bill.month_number || latest_time == 0) {
                     latest_bill = bill;
                     latest_time = 1; // Just to mark that we found a bill
                 }
             }
         }
         fclose(file);
     }
     
     if (!bill_found) {
         printf("No bills found for your account.\n");
         pauseScreen();
         return;
     }
     
     // Find premises details
     char meter_size_str[10] = "";
     for (int i = 0; i < premises_count; i++) {
         if (strcmp(premises[i].premises_number, latest_bill.premises_number) == 0) {
             switch (premises[i].meter_size) {
                 case METER_15MM:
                     strcpy(meter_size_str, "15mm");
                     break;
                 case METER_30MM:
                     strcpy(meter_size_str, "30mm");
                     break;
                 case METER_150MM:
                     strcpy(meter_size_str, "150mm");
                     break;
             }
             break;
         }
     }
     
     // Display bill details
     printf("\n======= NATIONAL WATER COMMISSION =======\n");
     printf("Bill ID: %s\n", latest_bill.bill_id);
     printf("Customer: %s %s\n", current_customer.first_name, current_customer.last_name);
     printf("Customer Number: %s\n", latest_bill.customer_number);
     printf("Premises Number: %s\n", latest_bill.premises_number);
     printf("Meter Size: %s\n", meter_size_str);
     printf("Bill Date: %s\n", latest_bill.bill_date);
     printf("Due Date: %s\n", latest_bill.due_date);
     printf("\n--- Meter Readings ---\n");
     printf("Previous Reading: %d\n", latest_bill.previous_reading);
     printf("Current Reading: %d\n", latest_bill.current_reading);
     printf("Consumption: %d litres\n", latest_bill.consumption);
     printf("\n--- Charges ---\n");
     printf("Water Charge: $%.2f\n", latest_bill.water_charge);
     printf("Sewerage Charge: $%.2f\n", latest_bill.sewerage_charge);
     printf("Service Charge: $%.2f\n", latest_bill.service_charge);
     printf("PAM: $%.2f\n", latest_bill.pam);
     printf("X-Factor: $%.2f\n", latest_bill.x_factor);
     printf("K-Factor: $%.2f\n", latest_bill.k_factor);
     printf("Total Current Charges: $%.2f\n", latest_bill.total_current_charges);
     
     if (latest_bill.is_early_payment_eligible) {
         printf("Early Payment Discount: $%.2f\n", latest_bill.early_payment_amount);
     }
     
     if (latest_bill.overdue_amount > 0) {
         printf("Overdue Amount: $%.2f\n", latest_bill.overdue_amount);
     }
     
     printf("\nTotal Amount Due: $%.2f\n", latest_bill.total_amount_due);
     
     if (latest_bill.amount_paid > 0) {
         printf("Amount Paid: $%.2f\n", latest_bill.amount_paid);
         printf("Balance: $%.2f\n", latest_bill.total_amount_due - latest_bill.amount_paid);
     }
     
     printf("\nPayment Status: %s\n", latest_bill.is_paid ? "PAID" : "UNPAID");
     printf("========================================\n");
     
     pauseScreen();
 }
 
 // Pay bill (Customer function)
 void payBill() {
     clearScreen();
     bool bill_found = false;
     Bill latest_bill;
     double payment_amount;
     
     printf("\n=== Pay Bill ===\n");
     
     // Check if customer has registered a payment card
     if (!current_customer.has_payment_card) {
         printf("You must register a payment card before making payments.\n");
         printf("Please select 'Register Payment Card' from the menu first.\n");
         pauseScreen();
         return;
     }
     
     // Find the most recent unpaid bill for the customer
     FILE *file = fopen(FILE_BILLS, "rb");
     if (file != NULL) {
         Bill bill;
         time_t latest_time = 0;
         
         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
             if (strcmp(bill.customer_number, current_customer.customer_number) == 0 && !bill.is_paid) {
                 bill_found = true;
                 
                 // Simple logic to find the most recent bill (based on month number)
                 if (bill.month_number > latest_bill.month_number || latest_time == 0) {
                     latest_bill = bill;
                     latest_time = 1; // Just to mark that we found a bill
                 }
             }
         }
         fclose(file);
     }
     
     if (!bill_found) {
         printf("No unpaid bills found for your account.\n");
         pauseScreen();
         return;
     }
     
     // Display bill summary
     printf("Latest Unpaid Bill Summary:\n");
     printf("Bill ID: %s\n", latest_bill.bill_id);
     printf("Total Amount Due: $%.2f\n", latest_bill.total_amount_due);
     printf("Amount Already Paid: $%.2f\n", latest_bill.amount_paid);
     printf("Remaining Balance: $%.2f\n", latest_bill.total_amount_due - latest_bill.amount_paid);
     
     // Get payment amount
     printf("\nEnter payment amount: $");
     scanf("%lf", &payment_amount);
     getchar(); // Consume newline
     
     if (payment_amount <= 0) {
         printf("Invalid payment amount. Must be greater than zero.\n");
         pauseScreen();
         return;
     }
     
     // Create payment
     Payment payment;
     generateID(payment.payment_id, "PMT");
     strcpy(payment.bill_id, latest_bill.bill_id);
     strcpy(payment.customer_number, current_customer.customer_number);
     strcpy(payment.premises_number, latest_bill.premises_number);
     payment.amount = payment_amount;
     getCurrentDate(payment.payment_date);
     
     // Update bill
     latest_bill.amount_paid += payment_amount;
     
     // Check if bill is fully paid
     if (latest_bill.amount_paid >= latest_bill.total_amount_due) {
         latest_bill.is_paid = true;
         
         // Handle overpayment (credit for next bill)
         double overpayment = latest_bill.amount_paid - latest_bill.total_amount_due;
         if (overpayment > 0) {
             printf("Overpayment of $%.2f will be credited to your next bill.\n", overpayment);
             // The overpayment will be handled when generating the next bill
         }
     }
     
     // Save payment to file
     file = fopen(FILE_PAYMENTS, "ab");
     if (file != NULL) {
         fwrite(&payment, sizeof(Payment), 1, file);
         fclose(file);
         
         // Update bill in file
         FILE *bill_file = fopen(FILE_BILLS, "rb");
         FILE *temp_file = fopen("temp_bills.dat", "wb");
         
         if (bill_file != NULL && temp_file != NULL) {
             Bill bill;
             while (fread(&bill, sizeof(Bill), 1, bill_file) == 1) {
                 if (strcmp(bill.bill_id, latest_bill.bill_id) == 0) {
                     fwrite(&latest_bill, sizeof(Bill), 1, temp_file);
                 } else {
                     fwrite(&bill, sizeof(Bill), 1, temp_file);
                 }
             }
             fclose(bill_file);
             fclose(temp_file);
             
             remove(FILE_BILLS);
             rename("temp_bills.dat", FILE_BILLS);
             
             // Log the payment
             logActivity(current_customer.customer_number, payment_amount, false);
             
             // Display receipt
             clearScreen();
             printf("\n========= PAYMENT RECEIPT =========\n");
             printf("Receipt ID: %s\n", payment.payment_id);
             printf("Date: %s\n", payment.payment_date);
             printf("Customer: %s %s\n", current_customer.first_name, current_customer.last_name);
             printf("Customer Number: %s\n", payment.customer_number);
             printf("Premises Number: %s\n", payment.premises_number);
             printf("Bill ID: %s\n", payment.bill_id);
             printf("Payment Amount: $%.2f\n", payment.amount);
             printf("Remaining Balance: $%.2f\n", latest_bill.total_amount_due - latest_bill.amount_paid);
             printf("Status: %s\n", latest_bill.is_paid ? "PAID IN FULL" : "PARTIALLY PAID");
             printf("==================================\n");
             
             printf("\nPayment processed successfully!\n");
         } else {
             printf("Error: Could not update bill data.\n");
         }
     } else {
         printf("Error: Could not save payment data.\n");
     }
     
     pauseScreen();
 }
 
 // Surrender meter (Customer function)
 void surrenderMeter() {
     clearScreen();
     char premises_number[8];
     bool premises_found = false;
     int premises_index = -1;
     
     printf("\n=== Surrender Meter ===\n");
     printf("Enter Premises Number to surrender: ");
     fgets(premises_number, 8, stdin);
     premises_number[strcspn(premises_number, "\n")] = '\0';
     
     // Find premises in array
     for (int i = 0; i < premises_count; i++) {
         if (strcmp(premises[i].premises_number, premises_number) == 0 && 
             strcmp(premises[i].customer_number, current_customer.customer_number) == 0 && 
             premises[i].is_active) {
             premises_found = true;
             premises_index = i;
             break;
         }
     }
     
     if (!premises_found) {
         printf("Premises not found, not associated with your account, or already inactive.\n");
         pauseScreen();
         return;
     }
     
     // Check for unpaid bills
     bool has_unpaid_bills = false;
     FILE *file = fopen(FILE_BILLS, "rb");
     if (file != NULL) {
         Bill bill;
         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
             if (strcmp(bill.customer_number, current_customer.customer_number) == 0 && 
                 strcmp(bill.premises_number, premises_number) == 0 && 
                 !bill.is_paid) {
                 has_unpaid_bills = true;
                 break;
             }
         }
         fclose(file);
     }
     
     if (has_unpaid_bills) {
         printf("Cannot surrender meter: You have unpaid bills for this premises.\n");
         printf("Please pay all outstanding bills before surrendering the meter.\n");
         pauseScreen();
         return;
     }
     
     // Deactivate premises
     premises[premises_index].is_active = false;
     
     // Update premises in file
     file = fopen(FILE_PREMISES, "wb");
     if (file != NULL) {
         for (int i = 0; i < premises_count; i++) {
             fwrite(&premises[i], sizeof(Premises), 1, file);
         }
         fclose(file);
         
         // Log the surrender
         logActivity(current_customer.customer_number, 0.0, true);
         
         printf("Meter surrendered successfully!\n");
     } else {
         printf("Error: Could not update premises data.\n");
     }
     
     pauseScreen();
 }
 
 // Load data from files
 void loadData() {
     // Load customers
     FILE *file = fopen(FILE_CUSTOMERS, "rb");
     if (file != NULL) {
         customer_count = 0;
         while (fread(&customers[customer_count], sizeof(Customer), 1, file) == 1) {
             customer_count++;
             if (customer_count >= MAX_CUSTOMERS) {
                 break;
             }
         }
         fclose(file);
     }
     
     // Load premises
     file = fopen(FILE_PREMISES, "rb");
     if (file != NULL) {
         premises_count = 0;
         while (fread(&premises[premises_count], sizeof(Premises), 1, file) == 1) {
             premises_count++;
             if (premises_count >= MAX_PREMISES) {
                 break;
             }
         }
         fclose(file);
     }
     
     // Create default agent account if it doesn't exist
     bool agent_exists = false;
     file = fopen(FILE_USERS, "rb");
     if (file != NULL) {
         User user;
         while (fread(&user, sizeof(User), 1, file) == 1) {
             if (user.type == AGENT) {
                 agent_exists = true;
                 break;
             }
         }
         fclose(file);
     }
     
     if (!agent_exists) {
         User agent;
         agent.id = 1;
         strcpy(agent.email, "agent@nwc.com");
         strcpy(agent.password, "password");
         agent.type = AGENT;
         agent.is_active = true;
         
         file = fopen(FILE_USERS, "ab");
         if (file != NULL) {
             fwrite(&agent, sizeof(User), 1, file);
             fclose(file);
         }
     }
 }
 
 // Save data to files
 void saveData() {
     // Save customers
     FILE *file = fopen(FILE_CUSTOMERS, "wb");
     if (file != NULL) {
         for (int i = 0; i < customer_count; i++) {
             fwrite(&customers[i], sizeof(Customer), 1, file);
         }
         fclose(file);
     }
     
     // Save premises
     file = fopen(FILE_PREMISES, "wb");
     if (file != NULL) {
         for (int i = 0; i < premises_count; i++) {
             fwrite(&premises[i], sizeof(Premises), 1, file);
         }
         fclose(file);
     }
 }
 
 // Generate a unique ID with prefix (e.g., BILL-1234)
 void generateID(char *id, const char *prefix) {
     int random_num = rand() % 10000;
     char timestamp[10];
     time_t t = time(NULL);
     struct tm *tm_info = localtime(&t);
     
     strftime(timestamp, 10, "%H%M%S", tm_info);
     sprintf(id, "%s-%d-%s", prefix, random_num, timestamp);
 }
 
 // Get current date in YYYY-MM-DD format
 void getCurrentDate(char *date) {
     time_t t = time(NULL);
     struct tm *tm_info = localtime(&t);
     
     strftime(date, 11, "%Y-%m-%d", tm_info);
 }
 
 // Generate a random number between min and max (inclusive)
 int generateRandomNumber(int min, int max) {
     return min + rand() % (max - min + 1);
 }
 
 // Calculate water charge based on consumption
 float calculateWaterCharge(int consumption) {
     float charge = 0.0;
     
     if (consumption <= 14000) {
         charge = consumption * 149.55 / 1000; // Convert to cubic meters rate
     } else if (consumption <= 27000) {
         charge = 14000 * 149.55 / 1000;
         charge += (consumption - 14000) * 266.15 / 1000;
     } else if (consumption <= 41000) {
         charge = 14000 * 149.55 / 1000;
         charge += 13000 * 266.15 / 1000;
         charge += (consumption - 27000) * 290.10 / 1000;
     } else {
         charge = 14000 * 149.55 / 1000;
         charge += 13000 * 266.15 / 1000;
         charge += 14000 * 290.10 / 1000;
         charge += (consumption - 41000) * 494.87 / 1000;
     }
     
     return charge;
 }
 
 // Calculate sewerage charge based on consumption
 float calculateSewerageCharge(int consumption) {
     float charge = 0.0;
     
     if (consumption <= 14000) {
         charge = consumption * 172.72 / 1000; // Convert to cubic meters rate
     } else if (consumption <= 27000) {
         charge = 14000 * 172.72 / 1000;
         charge += (consumption - 14000) * 307.42 / 1000;
     } else if (consumption <= 41000) {
         charge = 14000 * 172.72 / 1000;
         charge += 13000 * 307.42 / 1000;
         charge += (consumption - 27000) * 335.06 / 1000;
     } else {
         charge = 14000 * 172.72 / 1000;
         charge += 13000 * 307.42 / 1000;
         charge += 14000 * 335.06 / 1000;
         charge += (consumption - 41000) * 571.56 / 1000;
     }
     
     return charge;
 }
 
 // Calculate service charge based on meter size
 float calculateServiceCharge(MeterSize meter_size) {
     switch (meter_size) {
         case METER_15MM:
             return 1155.92;
         case METER_30MM:
             return 6217.03;
         case METER_150MM:
             return 39354.59;
         default:
             return 0.0;
     }
 }
 
 // Check if customer number already exists
 bool isCustomerNumberExists(const char *customer_number) {
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].customer_number, customer_number) == 0) {
             return true;
         }
     }
     return false;
 }
 
 // Check if premises number already exists (for active premises)
 bool isPremisesNumberExists(const char *premises_number) {
     for (int i = 0; i < premises_count; i++) {
         if (strcmp(premises[i].premises_number, premises_number) == 0 && premises[i].is_active) {
             return true;
         }
     }
     return false;
 }
 
 // Check if email already exists
 bool isEmailExists(const char *email) {
     FILE *file = fopen(FILE_USERS, "rb");
     if (file != NULL) {
         User user;
         while (fread(&user, sizeof(User), 1, file) == 1) {
             if (strcmp(user.email, email) == 0) {
                 fclose(file);
                 return true;
             }
         }
         fclose(file);
     }
     return false;
 }
 
 // Mask password input
 void maskPassword(char *password) {
     char ch;
     int i = 0;
     
     while (1) {
         ch = getchar();
         
         if (ch == '\n' || ch == '\r') {
             password[i] = '\0';
             break;
         } else if (ch == '\b' && i > 0) { // Backspace
             i--;
             printf("\b \b");
         } else if (ch != '\b') {
             password[i++] = ch;
             printf("*");
         }
     }
     printf("\n");
 }
 
 // Get daily usage limit based on income class
 int getDailyUsageLimit(IncomeClass income_class) {
     switch (income_class) {
         case LOW:
             return 125;
         case LOW_MEDIUM:
             return 175;
         case MEDIUM:
             return 220;
         case MEDIUM_HIGH:
             return 250;
         case HIGH:
             return 300;
         default:
             return 150; // Default value
     }
 }
 
 // Log system activity
 void logActivity(const char *customer_number, double payment_amount, bool surrender_meter) {
     SystemLog log;
     generateID(log.log_id, "LOG");
     strcpy(log.customer_number, customer_number);
     getCurrentDate(log.log_date);
     
     // Read existing log for this customer if any
     FILE *file = fopen(FILE_LOGS, "rb");
     if (file != NULL) {
         SystemLog existing_log;
         while (fread(&existing_log, sizeof(SystemLog), 1, file) == 1) {
             if (strcmp(existing_log.customer_number, customer_number) == 0) {
                 log.payments_count = existing_log.payments_count;
                 log.last_payment_amount = existing_log.last_payment_amount;
                 log.meters_surrendered = existing_log.meters_surrendered;
                 break;
             }
         }
         fclose(file);
     }
     
     // Update log data
     if (payment_amount > 0) {
         log.payments_count++;
         log.last_payment_amount = payment_amount;
     }
     
     if (surrender_meter) {
         log.meters_surrendered++;
     }
     
     // Save to file (append)
     file = fopen(FILE_LOGS, "ab");
     if (file != NULL) {
         fwrite(&log, sizeof(SystemLog), 1, file);
         fclose(file);
     }
 }
 
 // Display customer details
 void displayCustomerDetails(const char *customer_number) {
     bool customer_found = false;
     int customer_index = -1;
     
     // Find customer in array
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].customer_number, customer_number) == 0) {
             customer_found = true;
             customer_index = i;
             break;
         }
     }
     
     if (!customer_found) {
         printf("Customer not found.\n");
         return;
     }
     
     // Display customer details
     printf("\nCustomer Details:\n");
     printf("Customer Number: %s\n", customers[customer_index].customer_number);
     printf("Name: %s %s\n", customers[customer_index].first_name, customers[customer_index].last_name);
     printf("Status: %s\n", customers[customer_index].is_active ? "Active" : "Archived");
     printf("Income Class: ");
     
     switch (customers[customer_index].income_class) {
         case LOW:
             printf("Low (up to 125 L/day)\n");
             break;
         case LOW_MEDIUM:
             printf("Low/Medium (up to 175 L/day)\n");
             break;
         case MEDIUM:
             printf("Medium (up to 220 L/day)\n");
             break;
         case MEDIUM_HIGH:
             printf("Medium/High (up to 250 L/day)\n");
             break;
         case HIGH:
             printf("High (up to 300 L/day)\n");
             break;
     }
     
     // Display associated premises
     printf("\nAssociated Premises:\n");
     bool has_premises = false;
     
     for (int i = 0; i < premises_count; i++) {
         if (strcmp(premises[i].customer_number, customer_number) == 0) {
             has_premises = true;
             printf("Premises Number: %s\n", premises[i].premises_number);
             printf("Status: %s\n", premises[i].is_active ? "Active" : "Inactive");
             printf("Meter Size: ");
             
             switch (premises[i].meter_size) {
                 case METER_15MM:
                     printf("15mm\n");
                     break;
                 case METER_30MM:
                     printf("30mm\n");
                     break;
                 case METER_150MM:
                     printf("150mm\n");
                     break;
             }
             
             printf("Initial Reading: %d\n", premises[i].initial_reading);
             printf("Previous Reading: %d\n", premises[i].previous_reading);
             printf("Current Reading: %d\n", premises[i].current_reading);
             printf("---------------------------\n");
         }
     }
     
     if (!has_premises) {
         printf("No premises associated with this customer.\n");
     }
     
     // Display billing history
     printf("\nBilling History:\n");
     bool has_bills = false;
     
     FILE *file = fopen(FILE_BILLS, "rb");
     if (file != NULL) {
         Bill bill;
         while (fread(&bill, sizeof(Bill), 1, file) == 1) {
             if (strcmp(bill.customer_number, customer_number) == 0) {
                 has_bills = true;
                 printf("Bill ID: %s\n", bill.bill_id);
                 printf("Premises Number: %s\n", bill.premises_number);
                 printf("Month: %d\n", bill.month_number);
                 printf("Consumption: %d litres\n", bill.consumption);
                 printf("Total Amount Due: $%.2f\n", bill.total_amount_due);
                 printf("Amount Paid: $%.2f\n", bill.amount_paid);
                 printf("Status: %s\n", bill.is_paid ? "PAID" : "UNPAID");
                 printf("---------------------------\n");
             }
         }
         fclose(file);
     }
     
     if (!has_bills) {
         printf("No bills found for this customer.\n");
     }
 }
 
 // Clear the screen
 void clearScreen() {
     #ifdef _WIN32
         system("cls");
     #else
         system("clear");
     #endif
 }
 
 // Pause the screen
 void pauseScreen() {
     printf("\nPress Enter to continue...");
     getchar();
 }