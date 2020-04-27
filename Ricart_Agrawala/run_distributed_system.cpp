#include<iostream>
#include"timeutil.hpp"

int main(int argc, char *args[]){

    std::cout<<"Turning Server 1\n";
    system("gnome-terminal -e './main.out 5005 5006 1'");
    delay(1,0);
    std::cout<<"Turning Server 2\n";
    system("gnome-terminal -e './main.out 5006 5007 20'");
    delay(1,0);
    std::cout<<"Turning Server 3\n";
    system("gnome-terminal -e './main.out 5007 5005 10'");
}