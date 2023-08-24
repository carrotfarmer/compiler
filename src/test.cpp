#include <fstream>
#include <iostream>

int main()
{
    system("as -o out.o out.s");
    system("ld -macosx_version_min 11.0.0 -o out out.o "
           "-lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e "
           "_start -arch arm64");
}
