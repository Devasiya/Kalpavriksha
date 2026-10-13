#include <stdio.h>

int main() {
    const float STANDARD_PRICE = 150.0;
    const float PREMIUM_PRICE = 250.0;
    const float RECLINER_PRICE = 400.0;

    char customerName[50];
    int tickets, choice;
    float ticketPrice, subtotal, discount, finalBill;
    char category[20];

    printf("Enter customer name: ");
    scanf(" %49[^\n]",customerName);

    printf("Enter number of tickets: ");
    scanf("%d", &tickets);

    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ");
    scanf("%d", &choice);

    if (tickets < 1 || tickets > 20) {
        printf("Error: Invalid ticket count.\n");
        return 0;
    }

    switch (choice) {
        case 1:
            ticketPrice = STANDARD_PRICE;
            sprintf(category, "Standard");
            break;

        case 2:
            ticketPrice = PREMIUM_PRICE;
            sprintf(category, "Premium");
            break;

        case 3:
            ticketPrice = RECLINER_PRICE;
            sprintf(category, "Recliner");
            break;

        default:
            printf("Error: Invalid ticket category.\n");
            return 0;
    }

    subtotal = ticketPrice * tickets;

    if (tickets >= 5) {
        discount = subtotal * 0.10;
    } else {
        discount = 0.0;
    }

    finalBill = subtotal - discount;

    printf("\nCustomer: %s\n", customerName);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}
