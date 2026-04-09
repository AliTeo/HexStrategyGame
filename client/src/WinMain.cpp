#ifdef _WIN32
#include <windows.h>

extern int __argc;
extern char** __argv;

int main(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main(__argc, __argv);
}
#endif
