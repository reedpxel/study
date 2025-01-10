void f() {}

double g(int) { return 0.; }

// int[] h() // C++ functions can not returns arrays, only pointers
// {
//   int* p = new int[10]{};
//   return *p;
// }

void (*h(int[]))() { return nullptr; } // function h takes an array of ints and
// return a pointer to function that takes no arguments and returns void

void (*(*i(void (*(&)[])())))() { return nullptr; } // function i takes a 
// reference to an array of pointers to functions that take no arguments and
// return void and returns a pointer to a function that returns a pointer to a
// function that takes no arguments and returns void

int main()
{
    const int cnum = 5;
    int num = 6;

    const int* p1 = &cnum; // a pointer to constant
    int const* p1_ = &num; // a pointer to constant too
    const int* p2 = &num;
    int* const p3 = &num; // a constant pointer
    const int* const p4 = &cnum; // a constant pointer to constant

    const int** p5 = &p1; // a pointer to a pointer to constant
    *p5 = nullptr;
    int* const* p6 = &p3; // a pointer to a constant pointer
    const int** const p7 = &p1; // a constant pointer to a pointer to a constant
    int* const* const p8 = &p3; // a constant pointer to a constant pointer

    int a[10]{};
    int (*p9)[10] = &a; // a pointer to an array of ten elements
    const int ca[10]{};
    const int (*p10)[10] = &ca; // a pointer to a constant array
    int (*const p11)[10] = &a; // a constant pointer to an array
    const int (*const p12)[10] = &a; // a constant pointer to a constant array
    
    int (&ra1)[10] = a; // a reference to an array
    const int (&ra2)[10] = ra1; // a reference to a constant array

    double (*pg)(int) = g; // a pointer to function g
    void (*pf)() = f; // a pointer to function f
    void (*(*ph)(int[]))() = h; // a pointer to a function that takes array of
// ints and return a pointer to a function without arguments that returns void
    void (*arr[])(){}; // an array of pointers to functions without arguments
// that return void
    void (*(*arr2[])(int[]))(){}; // an array of pointers to functions that take
// an array of ints and return a pointer to functions that takes no arguments
// and return void
    void (*(*(*p13)(void (*(&)[])())))() = i; // a pointer to a function that 
// takes a reference to an array of pointers to functions that take no arguments
// and return void and returns a pointer to a function that returns a pointer 
// to a function that takes no arguments and returns void
}
