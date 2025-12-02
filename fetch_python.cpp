#include <Python.h>

int main() {
    Py_Initialize();

    PyRun_SimpleString("print('Hello from Python!')");
    PyRun_SimpleString("import requests\n"
                       "r = requests.get('https://www.w3schools.com/html/html_basic.asp')\n"
                       "open('output.html','w').write(r.text)");

    Py_Finalize();
    return 0;
}