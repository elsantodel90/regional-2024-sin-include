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
        char *base = outputPtr;
        while (x > 0)
        {
            *outputPtr++ = char('0' + x % 10);
            x /= 10;
        }
        char *fin = outputPtr;
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

char *readString()
{
    skipSpaces();
    char *ret = inputPtr;
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

const int MAXN = 1024;
const int MAXM = 2*1024;
const int MAXT = 2*MAXN;

struct Arista
{
    int nodo;
    Arista *siguiente;
} aristas[MAXM];

Arista *nextArista = aristas;

Arista* g[MAXT];

void addAristaDirigida(int a,int b)
{
    nextArista->nodo = b;
    nextArista->siguiente = g[a];
    g[a] = nextArista;
    nextArista++;
}

void addArista(int a,int b)
{
    addAristaDirigida(a, b);
    addAristaDirigida(b, a);
}

char *mat[MAXN];
int cliqueIds[MAXN][2];
int cliqueCount[MAXN];

int compa[2][MAXT];

int maxMatching(int T)
{
    forn(lado,2)
    forn(i,T)
        compa[lado][i] = -1;
    bool cambio = true;
    int ret = 0;
    int pending[2*T];
    int parent[2][T];
    int pendingSize = 0;
    while (cambio)
    {
        forn(lado,2)
        forn(i,T)
            parent[lado][i] = -1;
        #define PUSH(lado, x, p) do { parent[lado][x] = p; pending[pendingSize++] = x + lado*T;  } while(false)
        forn(i,T)
        if (compa[0][i] < 0)
            PUSH(0, i, T);
        while (pendingSize > 0)
        {
            int x = pending[--pendingSize];
            if (x < T)
            {
                for (Arista *edge = g[x]; edge; edge = edge->siguiente)
                {
                    int y = edge->nodo;
                    if (y != compa[0][x] && parent[1][y] < 0)
                        PUSH(1, y, x);
                }
            }
            else
            {
                x -= T;
                int y = compa[1][x];
                if (y >= 0 && parent[0][y] < 0)
                        PUSH(0, y, x);
            }
        }
        cambio = false;
        forn(i,T)
        if (parent[1][i] >= 0 && compa[1][i] < 0)
        {
            cambio = true;
            ret++;
            int y = i;
            int x = parent[1][y];
            compa[1][y] = x;
            while (parent[0][x] != T)
            {
                compa[0][x] = y;
                y = parent[0][x];
                x = parent[1][y];
                compa[1][y] = x;
            }
            compa[0][x] = y;
            break;
        }
    }
    return ret;
}

//#include <iostream>
//using namespace std;

int main()
{
    osRead();
    inputPtr = input;
    
    int N = int(readInt());
    forn(i,N)
        mat[i] = readString();
    
    int T = 0;
        
    forn(i,N)
    forn(j,N)
    if (mat[i][j] == '1')
    {
        static int clique[MAXN];
        int tam = 0;
        clique[tam++] = i;
        clique[tam++] = j;
        forn(k,N)
        if (k != i && k != j && mat[i][k] != '0' && mat[j][k] != '0')
            clique[tam++] = k;
        forn(k1, tam)
        forn(k2, k1)
            mat[clique[k1]][clique[k2]] = mat[clique[k2]][clique[k1]] = '2';
        forn(k, tam)
            cliqueIds[clique[k]][cliqueCount[clique[k]]++] = T;
        T++;
    }
    forn(i,N)
    while (cliqueCount[i] < 2)
        cliqueIds[i][cliqueCount[i]++] = T++;
    
    forn(i,N)
        addArista(cliqueIds[i][0], cliqueIds[i][1]);
    
    printIntLn(maxMatching(T)/2);
    
    osWrite();
    return 0;
}


