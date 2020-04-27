#include"criticalsection.hpp"


void critical_section(){
    FILE *file;
    file = fopen("criticalfile.txt","w");
    uint64_t s = seconds();
    fprintf(file,"Entered at %2ld\n",s);
    delay(1,0);
    s = seconds();
    fprintf(file,"Exited at %2ld\n",s);
    fclose(file);
}