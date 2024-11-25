#define forn(i,n) for(int i=0;i<int(n);i++)
#define forsn(i,s,n) for(int i=int(s);i<int(n);i++)
#define dforn(i,n) for(int i=int(n)-1;i>=0;i--)

#define SIZE(c) int((c).size())
#define DBG(x) cerr << #x << " = " << (x) << endl

typedef long long tint;

const int MAX_OUTPUT = 50000000;
const int MAX_INPUT = 75000000;

char output[MAX_OUTPUT];
char input[MAX_INPUT];
char *outputPtr = output;
char *inputPtr = input;

#ifdef DESKTOP_WALLY

// syscall number in rax
//  rdi , rsi , rdx , r10 , r8 , and r9
// using syscall instruction destroys rcx, r11

void osWrite()
{
    tint cant = int(outputPtr - output);
    asm volatile (
                  "lea %[output], %%rbx\n\t"
                  "mov %[cant], %%rcx\n\t"
                  ".intel_syntax noprefix\n\t"
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
                  :  [cant]"m"(cant), [output]"m"(output) // inputs
                  :  "rax", "rbx", "rcx", "rdx", "rdi", "rsi", "r11"
    );
}

tint osRead()
{
    tint ret;
    asm (
                  "lea %[inpu] , %%rbx\n\t "
                  "mov %[MAXINPU], %%rcx\n\t"
                  ".intel_syntax noprefix\n\t"
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
                  :  "=a"(ret), [inpu]"=m"(*(char(*)[MAX_INPUT])input)// output
                  :  [MAXINPU]"i"(MAX_INPUT) // inputs
                  :  "rbx", "rcx", "rdx", "rdi", "rsi", "r11"
    );
    inputPtr += ret;
    return ret;
}

#else

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

void printLn()
{
    //*outputPtr++ = '\r';
    *outputPtr++ = '\n';
}

void printIntLn(tint x)
{
    printInt(x);
    printLn();
}

void printString(const char *s)
{
    while (*s != 0)
        *outputPtr++ = *s++;
}

void printStringLn(const char*s)
{
    printString(s);
    printLn();
}

bool isSpace(char c)
{
    return c == ' ' || c == '\n' 
           || *inputPtr == '\r' || c == '\t';
}

void skipSpaces()
{
    while (isSpace(*inputPtr))
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

volatile char *readString()
{
    skipSpaces();
    volatile char *ret = inputPtr;
    while (!isSpace(*inputPtr))
        inputPtr++;
    *inputPtr++ = 0;
    return ret;
}

int stringLength(const char *s)
{
    int ret = 0;
    while (*s)
    {
        s++;
        ret++;
    }
    return ret;
}

unsigned long long RAND_CURR = 238794233ULL;

template<typename T>
void quickSort(T*a, T*b)
{
    //if (b-a < 1)
    if (b-a < 10)
    {
        for (T *i = a; i != b; i++)
        for (T *j = i-1; !(j < a) && *(j+1) < *j ; j--)
            miswap(*j, *(j+1));
        return;
    }
    RAND_CURR *= 3384523;
    RAND_CURR %= 1000000007;
    miswap(a[0], a[(RAND_CURR)%(b-a)]);
    T *i = a+1;
    T *j = b;
    while (i < j)
    {
        while (!(a[0] < *i)  && i < b)
            i++;
        while (!(*(j-1) < a[0]) && j > a+1)
            j--;
        if (i < j)
        {
            miswap(*i, *(j-1));
            i++;
            j--;
        }
    }
    // j <= i
    // [j,b) hay >= pivot
    // [a,i) hay <= pivot
    miswap(a[0], *(j-1));
    quickSort(a,j-1);
    quickSort(i,b);
}

// Codigo del problema

const tint MAX_VAL = 4000000000000000000LL;

struct Entero
{
    tint x;
    Entero operator +(Entero o) const
    {
        tint ret = x + o.x;
        return {ret > MAX_VAL ? MAX_VAL : ret};
    }
    Entero operator *(Entero o) const
    {
        __int128 ret = __int128(x) * o.x;
        return {ret > MAX_VAL ? MAX_VAL : tint(ret)};
    }
    
    bool operator <(Entero o) const
    {
        return x < o.x;
    }
};

const int MAXN = 201000;
const int MAXR = 200;

Entero f[MAXN][MAXR];

int ft[MAXN];
// ft[mask] guarda la suma en  [apagaBit(mask), mask)

void add(int i, int delta)
{
    i++;
    for ( ; i < MAXN ; i += i&(-i) )
        ft[i] += delta;
}

int getPrefixSum(int i)
{
    int ret = 0;
    for ( ; i > 0 ; i -= i&(-i) )
        ret += ft[i];
    return ret;
}


int main()
{
    osRead();
    inputPtr = input;
    
    int N = int(readInt());
    tint K = readInt()-1;
    
    forn(r,MAXR)
        f[0][r] = {0};
    f[0][0] = {1};
    forsn(n,1,N+1)
    forn(r,MAXR)
    {
        f[n][r] = {0};
        int tope = r+1;
        if (n < tope)
            tope = n;
        forn(i,tope)
            f[n][r] = f[n][r] + f[n-1][r-i];
        if (f[n][r].x == MAX_VAL || f[n][r].x == 0)
            break;
    }
    
    int inversionCount = 0;
    while (K >= f[N][inversionCount].x)
        K -= f[N][inversionCount++].x;
    
    forn(i,N)
        add(i,1);
    //forn(i,N)
    //    ft[i] = 1;
    
    int originalN = N;
    for (; N > 0; N--)
    {
        int firstNumberIndex = 0;
        while (K >= f[N-1][inversionCount-firstNumberIndex].x)
            K -= f[N-1][inversionCount-firstNumberIndex++].x;
            
        // el primero que cumple esto
        // getPrefixSum(i+1) >= firstNumberIndex+1
        // a no cumple, b cumple
        int a = -1, b = originalN-1;
        while (b-a > 1)
        {
            int c = (a+b)/2;
            if (getPrefixSum(c+1) >= firstNumberIndex+1)
                b = c;
            else
                a = c;
        }
        int firstNumber = b;
        
        //int cant = 0, firstNumber = 0;
        //while (cant < firstNumberIndex || ft[firstNumber] == 0)
        //    cant += ft[firstNumber++];
        printInt(firstNumber+1);
        //ft[firstNumber]--;
        add(firstNumber,-1);
        inversionCount -= firstNumberIndex;
        if (N > 1)
            printString(" ");
    }
    printLn();
    
    
    osWrite();
    return 0;
}


