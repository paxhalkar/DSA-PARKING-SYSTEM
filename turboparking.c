#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

#define MAX_SLOTS    20
#define MAX_VEHICLES 200

struct Vehicle
{
    int   id;
    char  number[20];
    int   type;      /* 1 = 2 Wheeler, 2 = 4 Wheeler */
    int   service;   /* 1 Normal, 2 Ambulance, 3 Defence, 4 Government */
    int   slot;      /* 1..MAX_SLOTS */
    float fee;
    int   active;
};

struct Vehicle vehicles[MAX_VEHICLES];
int   slots[MAX_SLOTS];
int   count = 0;          /* records currently stored          */
int   nextId = 1;         /* never repeats, even after cleanup */
int   totalRecords = 0;   /* all vehicles ever registered      */
int   totalTwo = 0;       /* all 2 wheelers ever registered    */
int   totalFour = 0;      /* all 4 wheelers ever registered    */
float totalRev = 0.0;     /* all revenue ever collected        */

/* ---------- Safe input helpers ---------- */

int readInt(int *value)
{
    char buf[20];
    int i, len;

    if(fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    len = strlen(buf);
    if(len > 0 && buf[len - 1] == '\n')
    {
        buf[len - 1] = '\0';
        len--;
    }
    else
    {
        /* line too long: discard the rest */
        while(getchar() != '\n')
            ;
        return 0;
    }

    if(len == 0)
        return 0;

    for(i = 0; i < len; i++)
    {
        if(!isdigit(buf[i]))
            return 0;
    }

    sscanf(buf, "%d", value);
    return 1;
}

int readText(char *text, int size)
{
    char buf[40];
    int i, len, j = 0;

    if(fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    len = strlen(buf);
    if(len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    else
    {
        while(getchar() != '\n')
            ;
        return 0;
    }

    /* remove spaces, convert to uppercase */
    for(i = 0; i < len && j < size - 1; i++)
    {
        if(buf[i] != ' ')
            text[j++] = toupper(buf[i]);
    }
    text[j] = '\0';

    return (j > 0);
}

/* ---------- Function 1: Search Vehicle (inside parking) ---------- */

int searchVehicle(char number[])
{
    int i;

    for(i = 0; i < count; i++)
    {
        if(strcmp(vehicles[i].number, number) == 0 &&
           vehicles[i].active == 1)
            return i;
    }

    return -1;
}

/* ---------- Function 2: Find Available Slot ---------- */

int findSlot()
{
    int i;

    for(i = 0; i < MAX_SLOTS; i++)
    {
        if(slots[i] == 0)
            return i;
    }

    return -1;
}

/* ---------- Function 3: View Available Slots ---------- */

void viewSlots()
{
    int i, available = 0;

    clrscr();

    printf("====================================");
    printf("\n       PARKING SLOT STATUS");
    printf("\n====================================\n");

    for(i = 0; i < MAX_SLOTS; i++)
    {
        if(slots[i] == 0)
        {
            printf("Slot %2d - Available\n", i + 1);
            available++;
        }
        else
            printf("Slot %2d - Occupied\n", i + 1);
    }

    printf("\nAvailable Slots: %d", available);
    printf("\nOccupied Slots : %d", MAX_SLOTS - available);
    printf("\n\nPress any key to continue...");

    getch();
}

/* ---------- Function 4: Select Parking Slot ---------- */

int selectSlot()
{
    int slot;

    if(findSlot() == -1)
    {
        printf("\nNo parking slot available!");
        return -1;
    }

    while(1)
    {
        printf("\nEnter preferred slot (1-%d): ", MAX_SLOTS);

        if(!readInt(&slot))
        {
            printf("Invalid input! Enter a number.\n");
            continue;
        }

        if(slot < 1 || slot > MAX_SLOTS)
            printf("Invalid slot number!\n");
        else if(slots[slot - 1] == 1)
        {
            printf("That slot is not available.\n");
            printf("Please select another slot.\n");
        }
        else
            return slot - 1;
    }
}

/* ---------- Function 5: Scan Vehicle Type ---------- */

int scanVehicleType()
{
    int type;

    printf("\n1. 2 Wheeler");
    printf("\n2. 4 Wheeler");
    printf("\nEnter vehicle type: ");

    if(!readInt(&type))
        return 0;

    if(type == 1 || type == 2)
        return type;

    return 0;
}

/* ---------- Function 6: Calculate Parking Fee ---------- */

float calculateFee(int type, int service)
{
    if(service == 2 || service == 3 || service == 4)
        return 0.0;

    if(type == 1)
        return 20.0;

    if(type == 2)
        return 40.0;

    return 0.0;
}

/* ---------- Remove exited records to free space ---------- */

void cleanupRecords()
{
    int i, j = 0;

    for(i = 0; i < count; i++)
    {
        if(vehicles[i].active == 1)
        {
            if(i != j)
                vehicles[j] = vehicles[i];
            j++;
        }
    }

    count = j;
}

/* ---------- Function 7: Register Vehicle ---------- */

void registerVehicle()
{
    char number[20];
    int type, service, slot, i;

    clrscr();

    printf("====================================");
    printf("\n       REGISTER VEHICLE");
    printf("\n====================================");

    if(count >= MAX_VEHICLES)
        cleanupRecords();

    if(count >= MAX_VEHICLES)
    {
        printf("\nVehicle record limit reached!");
        getch();
        return;
    }

    if(findSlot() == -1)
    {
        printf("\nNo parking slot available!");
        getch();
        return;
    }

    printf("\nEnter Vehicle Number: ");
    if(!readText(number, sizeof(number)))
    {
        printf("\nInvalid vehicle number!");
        getch();
        return;
    }

    if(searchVehicle(number) != -1)
    {
        printf("\nVehicle is already inside parking!");
        getch();
        return;
    }

    type = scanVehicleType();

    if(type == 0)
    {
        printf("\nInvalid vehicle type!");
        getch();
        return;
    }

    printf("\nService Type");
    printf("\n1. Normal");
    printf("\n2. Ambulance");
    printf("\n3. Defence");
    printf("\n4. Government");
    printf("\nEnter service type: ");

    if(!readInt(&service) || service < 1 || service > 4)
    {
        printf("\nInvalid service type!");
        getch();
        return;
    }

    printf("\n\nAvailable and occupied slots:\n");

    for(i = 0; i < MAX_SLOTS; i++)
    {
        if(slots[i] == 0)
            printf("Slot %2d - Available\n", i + 1);
        else
            printf("Slot %2d - Occupied\n", i + 1);
    }

    slot = selectSlot();

    if(slot == -1)
    {
        getch();
        return;
    }

    i = count;

    vehicles[i].id = nextId++;
    strcpy(vehicles[i].number, number);
    vehicles[i].type = type;
    vehicles[i].service = service;
    vehicles[i].slot = slot + 1;
    vehicles[i].fee = calculateFee(type, service);
    vehicles[i].active = 1;

    slots[slot] = 1;
    count++;

    totalRecords++;
    if(type == 1)
        totalTwo++;
    else
        totalFour++;
    totalRev += vehicles[i].fee;

    printf("\n====================================");
    printf("\n       VEHICLE REGISTERED");
    printf("\n====================================");

    printf("\nVehicle ID     : %d", vehicles[i].id);
    printf("\nVehicle Number : %s", vehicles[i].number);

    if(type == 1)
        printf("\nVehicle Type   : 2 Wheeler");
    else
        printf("\nVehicle Type   : 4 Wheeler");

    printf("\nSlot Number    : %d", vehicles[i].slot);
    printf("\nParking Fee    : Rs. %.2f", vehicles[i].fee);
    printf("\n\nPress any key to continue...");

    getch();
}

/* ---------- Function 8: Display Vehicle Details ---------- */

void displayVehicles()
{
    int i;

    clrscr();

    printf("====================================");
    printf("\n        VEHICLE DETAILS");
    printf("\n====================================");

    if(count == 0)
    {
        printf("\nNo vehicles registered!");
        getch();
        return;
    }

    for(i = 0; i < count; i++)
    {
        printf("\n\nVehicle ID     : %d", vehicles[i].id);
        printf("\nVehicle Number : %s", vehicles[i].number);

        if(vehicles[i].type == 1)
            printf("\nVehicle Type   : 2 Wheeler");
        else
            printf("\nVehicle Type   : 4 Wheeler");

        printf("\nSlot Number    : %d", vehicles[i].slot);
        printf("\nParking Fee    : Rs. %.2f", vehicles[i].fee);

        if(vehicles[i].active == 1)
            printf("\nStatus         : Inside Parking");
        else
            printf("\nStatus         : Exited");

        /* pause every 2 records so output does not scroll away */
        if((i + 1) % 2 == 0 && i + 1 < count)
        {
            printf("\n\n-- Press any key for next page --");
            getch();
            clrscr();
        }
    }

    printf("\n\nPress any key to continue...");
    getch();
}

/* ---------- Function 9: Vehicle Exit ---------- */

void vehicleExit()
{
    char number[20];
    int index;

    clrscr();

    printf("====================================");
    printf("\n          VEHICLE EXIT");
    printf("\n====================================");

    printf("\nEnter Vehicle Number: ");
    if(!readText(number, sizeof(number)))
    {
        printf("\nInvalid vehicle number!");
        getch();
        return;
    }

    index = searchVehicle(number);

    if(index == -1)
    {
        printf("\nVehicle not found or already exited!");
        getch();
        return;
    }

    slots[vehicles[index].slot - 1] = 0;
    vehicles[index].active = 0;

    printf("\nVehicle Exit Successful!");
    printf("\nVehicle Number : %s", vehicles[index].number);
    printf("\nReleased Slot  : %d", vehicles[index].slot);
    printf("\nParking Fee    : Rs. %.2f", vehicles[index].fee);
    printf("\n\nPress any key to continue...");

    getch();
}

/* ---------- Function 10: Total Revenue ---------- */

float totalRevenue()
{
    return totalRev;
}

/* ---------- Function 11: Count 2 Wheelers ---------- */

int countTwoWheelers()
{
    return totalTwo;
}

/* ---------- Function 12: Count 4 Wheelers ---------- */

int countFourWheelers()
{
    return totalFour;
}

/* ---------- Function 13: Owner Report ---------- */

void ownerReport()
{
    int i, occupied = 0;

    clrscr();

    for(i = 0; i < MAX_SLOTS; i++)
    {
        if(slots[i] == 1)
            occupied++;
    }

    printf("====================================");
    printf("\n           OWNER REPORT");
    printf("\n====================================");

    printf("\nTotal Vehicle Records : %d", totalRecords);
    printf("\nVehicles Inside       : %d", occupied);
    printf("\nAvailable Slots       : %d", MAX_SLOTS - occupied);
    printf("\n2 Wheelers            : %d", countTwoWheelers());
    printf("\n4 Wheelers            : %d", countFourWheelers());
    printf("\nTotal Revenue         : Rs. %.2f", totalRevenue());
    printf("\n\nPress any key to continue...");

    getch();
}

/* ---------- Function 14: Search and Display Vehicle ---------- */

void searchAndDisplayVehicle()
{
    char number[20];
    int i, found = 0;

    clrscr();

    printf("====================================");
    printf("\n          SEARCH VEHICLE");
    printf("\n====================================");

    printf("\nEnter Vehicle Number to Search: ");
    if(!readText(number, sizeof(number)))
    {
        printf("\nInvalid vehicle number!");
        getch();
        return;
    }

    /* search from newest to oldest so the latest visit is shown */
    for(i = count - 1; i >= 0; i--)
    {
        if(strcmp(vehicles[i].number, number) == 0)
        {
            found = 1;

            printf("\n\nVehicle ID     : %d", vehicles[i].id);
            printf("\nVehicle Number : %s", vehicles[i].number);

            if(vehicles[i].type == 1)
                printf("\nVehicle Type   : 2 Wheeler");
            else
                printf("\nVehicle Type   : 4 Wheeler");

            printf("\nSlot Number    : %d", vehicles[i].slot);
            printf("\nParking Fee    : Rs. %.2f", vehicles[i].fee);

            if(vehicles[i].active == 1)
                printf("\nStatus         : Inside Parking");
            else
                printf("\nStatus         : Exited");

            break;
        }
    }

    if(found == 0)
        printf("\nVehicle record not found!");

    printf("\n\nPress any key to continue...");
    getch();
}

/* ---------- Main Function ---------- */

void main()
{
    int choice, i;

    clrscr();

    for(i = 0; i < MAX_SLOTS; i++)
        slots[i] = 0;

    do
    {
        clrscr();

        printf("========================================");
        printf("\n    MALL PARKING MANAGEMENT SYSTEM");
        printf("\n========================================");

        printf("\n\n1. View Available Slots");
        printf("\n2. Register Vehicle");
        printf("\n3. Vehicle Exit");
        printf("\n4. View Vehicle Details");
        printf("\n5. Owner Report");
        printf("\n6. Search Vehicle");
        printf("\n7. Exit");

        printf("\n\nPlease Enter Your Choice From Above: ");

        if(!readInt(&choice))
            choice = 0;

        switch(choice)
        {
            case 1:
                viewSlots();
                break;

            case 2:
                registerVehicle();
                break;

            case 3:
                vehicleExit();
                break;

            case 4:
                displayVehicles();
                break;

            case 5:
                ownerReport();
                break;

            case 6:
                searchAndDisplayVehicle();
                break;

            case 7:
                printf("\nThank you for using the system!");
                break;

            default:
                printf("\nInvalid choice! Press any key...");
                getch();
        }

    } while(choice != 7);

    getch();
}
