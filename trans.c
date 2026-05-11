#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

unsigned int enterChoice(void);

void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
void depositMoney(FILE *fPtr);
void withdrawMoney(FILE *fPtr);
void logTransaction(const char *message);

int main()
{
    FILE *cfPtr;

    // Create file if not exists
    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be created.\n");
            return 1;
        }

        struct clientData blank = {0, "", "", 0.0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }

        rewind(cfPtr);
    }

    unsigned int choice;

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            displayAll(cfPtr);
            break;

        case 6:
            searchAccount(cfPtr);
            break;

        case 7:
            depositMoney(cfPtr);
            break;

        case 8:
            withdrawMoney(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);

    printf("Program closed successfully.\n");

    return 0;
}

// MENU
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n========== BANK MANAGEMENT SYSTEM ==========\n");
    printf("1 - Export accounts to text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Display all accounts\n");
    printf("6 - Search account\n");
    printf("7 - Deposit money\n");
    printf("8 - Withdraw money\n");
    printf("9 - Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);

    return choice;
}

// EXPORT TO TEXT FILE
void textFile(FILE *readPtr)
{
    FILE *writePtr;

    struct clientData client = {0, "", "", 0.0};

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Cannot create text file.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "%-6s %-15s %-10s %-10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6d %-15s %-10s %.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("accounts.txt created successfully.\n");
}

// ADD ACCOUNT
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance:\n");

    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = accountNum;

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");

    logTransaction("New account created");
}

// UPDATE ACCOUNT
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Current Balance: %.2f\n", client.balance);

    printf("Enter transaction amount (+/-): ");
    scanf("%lf", &transaction);

    client.balance += transaction;

    fseek(fPtr,
          -(long)sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated Balance: %.2f\n", client.balance);

    logTransaction("Account updated");
}

// DELETE ACCOUNT
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blankClient,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Account deleted successfully.\n");

    logTransaction("Account deleted");
}

// DISPLAY ALL ACCOUNTS
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n========== ACCOUNT LIST ==========\n");

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("A/C: %d | %s %s | Balance: %.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

// SEARCH ACCOUNT
void searchAccount(FILE *fPtr)
{
    unsigned int account;

    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found\n");
        printf("Account No : %d\n", client.acctNum);
        printf("Name       : %s %s\n",
               client.firstName,
               client.lastName);
        printf("Balance    : %.2f\n",
               client.balance);
    }
}

// DEPOSIT MONEY
void depositMoney(FILE *fPtr)
{
    unsigned int account;
    double amount;

    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    printf("Enter amount to deposit: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr,
          -(long)sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Deposit successful.\n");
    printf("New Balance: %.2f\n", client.balance);

    logTransaction("Money deposited");
}

// WITHDRAW MONEY
void withdrawMoney(FILE *fPtr)
{
    unsigned int account;
    double amount;

    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    printf("Enter amount to withdraw: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    if (client.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    client.balance -= amount;

    fseek(fPtr,
          -(long)sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Withdrawal successful.\n");
    printf("Remaining Balance: %.2f\n", client.balance);

    logTransaction("Money withdrawn");
}

// LOG TRANSACTION
void logTransaction(const char *message)
{
    FILE *logFile = fopen("transactions.txt", "a");

    if (logFile != NULL)
    {
        fprintf(logFile, "%s\n", message);
        fclose(logFile);
    }
}