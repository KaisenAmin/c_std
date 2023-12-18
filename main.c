#include "bitset/bitset.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Bitset* myBitset = bitset_create(8);
    if (!myBitset) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    int pos;

    printf("Enter a position to set (0-7): ");
    scanf("%d", &pos);

    if (pos >= 0 && pos < 8) 
    {
        myBitset->set(myBitset, pos, true);

        printf("Bitset after setting position %d: ", pos);
        myBitset->print(myBitset);
    }
    else 
        printf("Invalid position!\n");
    
    myBitset->deallocate(myBitset);

    return 0;
}
