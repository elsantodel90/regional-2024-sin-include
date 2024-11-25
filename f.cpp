#define forn(i,n) for(int i=0;i<int(n);i++)
#define dforn(i,n) for(int i=int(n)-1;i>=0;i--)

#define SIZE(c) int((c).size())
#define DBG(x) cerr << #x << " = " << (x) << endl

typedef long long tint;

const int MAX_OUTPUT = 50000000;
const int MAX_INPUT = 75000000;


#ifdef DESKTOP_WALLY

// syscall number in rax
//  rdi , rsi , rdx , r10 , r8 , and r9

volatile char output[MAX_OUTPUT];
volatile char input[MAX_INPUT];
volatile char *outputPtr = output;
volatile char *inputPtr = input;

void osWrite()
{
    volatile tint cant = int(outputPtr - output);
    asm volatile (".intel_syntax noprefix\n\t"
                  ".start:\n\t"
                  "push rcx\n\t"
                  "push rbx\n\t"
                  "mov rax,1\n\t"
                  "mov rdi,1\n\t"
                  "mov rsi,rbx\n\t"
                  "mov rdx,rcx\n\t"
                  "syscall\n\t"
                  "pop rbx\n\t"
                  "pop rcx\n\t"
                  "test rax, rax\n\t"
                  "js .start\n\t"
                  "add rbx, rax\n\t"
                  "sub rcx, rax\n\t"
                  "jnz .start\n\t"
                  ".att_syntax\n\t"                  
                  :  // output
                  :  "b"(output) , "c"(cant) // inputs
                  :  "rdi", "rsi"
    );
}

tint osRead()
{
    volatile tint ret;
    asm volatile (".intel_syntax noprefix\n\t"
                  "push rbx\n\t"
                  ".start2:\n\t"
                  "push rcx\n\t"
                  "push rbx\n\t"
                  "mov rax,0\n\t"
                  "mov rdi,0\n\t"
                  "mov rsi,rbx\n\t"
                  "mov rdx,rcx\n\t"
                  "syscall\n\t"
                  "pop rbx\n\t"
                  "pop rcx\n\t"
                  "test rax, rax\n\t"
                  "js .start2\n\t"
                  "add rbx, rax\n\t"
                  "sub rcx, rax\n\t"
                  "test rax, rax\n\t"
                  "jnz .start2\n\t"
                  "mov rax, rbx\n\t"
                  "pop rbx\n\t"
                  "sub rax, rbx\n\t"
                  ".att_syntax\n\t"                  
                  :  "=a"(ret)// output
                  :  "b"(input) , "c"(MAX_INPUT) // inputs
                  :  "rdi", "rsi"
    );
    return ret;
}

#else

char output[MAX_OUTPUT];
char input[MAX_INPUT];
char *outputPtr = output;
char *inputPtr = input;


extern "C" bool WriteFile(
  void * hFile,
  char *lpBuffer,
  int nNumberOfBytesToWrite,
  int *lpNumberOfBytesWritten,
  void *lpOverlapped
);


extern "C" bool ReadFile(
  void * hFile,
  char *lpBuffer,
  int nNumberOfBytesToWrite,
  int *lpNumberOfBytesWritten,
  void *lpOverlapped
);

extern "C" void * GetStdHandle(unsigned int nStdHandle);

void osWrite()
{
    //int written = 0;
    //WriteFile(GetStdHandle(-11) , output, int(outputPtr - output), &written, nullptr);
    int pending = int(outputPtr - output);
    while (pending > 0)
    {
        int written = 0;
        if (WriteFile(GetStdHandle(-11) , outputPtr - pending, pending, &written, nullptr))
            pending -= written;
    }
}

tint osRead()
{
    //int read = 0;
    //ReadFile(GetStdHandle(-10) , input, MAX_INPUT, &read, nullptr);
    //return read;
    int read = 1;
    while (read != 0)
    {
        read = 0;
        while (!ReadFile(GetStdHandle(-10) , inputPtr, int(MAX_INPUT - (inputPtr - input)),
            &read, nullptr))
        {
            // nothing to do, retry
        }
        inputPtr += read;
    }
    return inputPtr - input;
}


#endif

// Codigo IO no tan low-level

template <typename T>
void miswap(T &a, T&b)
{
    T aux;
    aux = a;
    a = b;
    b = aux;
}

void printInt(tint x)
{
    if (x < 0)
    {
        *outputPtr++ = '-';
        x = -x;
    }
    if (x == 0)
        *outputPtr++ = '0';
    else
    {
        volatile char *base = outputPtr;
        while (x > 0)
        {
            *outputPtr++ = char('0' + x % 10);
            x /= 10;
        }
        volatile char *fin = outputPtr;
        while (base < fin)
        {
           fin--; 
           miswap(*base, *fin);
           base++;
        }
    }
}

void enter()
{
    //*outputPtr++ = '\r';
    *outputPtr++ = '\n';
}

void printIntLn(tint x)
{
    printInt(x);
    enter();
}

void printString(const char *s)
{
    while (*s != 0)
        *outputPtr++ = *s++;
}

void printStringLn(const char*s)
{
    printString(s);
    enter();
}

void skipSpaces()
{
    while (*inputPtr == ' ' || *inputPtr == '\n' 
           || *inputPtr == '\r' || *inputPtr == '\t')
        inputPtr++;
}

tint readInt()
{
    skipSpaces();
    int signo = 1;
    if (*inputPtr == '-')
    {
       signo = -1;
       inputPtr++;
    }
    tint ret = 0;
    while ('0' <= *inputPtr && *inputPtr <= '9')
    {
        ret = 10*ret + (*inputPtr-'0');
        inputPtr++;
    }
    return ret * signo;
}

// Codigo del problema

int main()
{
    osRead();
    inputPtr = input;
    
    tint K,N;
    K = readInt();
    N = readInt();
    
    if (K > (N+1)/2 || K < (N+2)/3)
        printStringLn("*");
    else
    {
        while (N > 0)
        {
            if (N == 1)
            {
                printString("X");
                N -= 1;
                K -= 1;
            }
            else if (N == 3*K)
            {
                printString("-X-");
                N -= 3;
                K -= 1;
            }
            else
            {
                printString("X-");
                N -= 2;
                K -= 1;
            }
        }
    }
    
    osWrite();
    return 0;
}
