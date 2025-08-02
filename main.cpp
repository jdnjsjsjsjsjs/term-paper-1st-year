#include <iostream>
#include "migration.h"
#include "menu.h"

using namespace std;

int main(int argc, char* argv[])
{
    menu menu;
    menu.run(argv[1]);
}
