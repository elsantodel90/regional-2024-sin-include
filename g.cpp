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
const T& min(const T&a, const T&b)
{
    if (a < b)
        return a;
    else
        return b;
}

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

#define assert(x)

const int INF = 2000000000;

const int MAX_N = 200000;
const int MAX_M = 2*MAX_N;

struct Arista
{
    int nodo, l;
    Arista *siguiente;
} aristas[MAX_M];

Arista *nextArista = aristas;

Arista* g[MAX_N];

void addAristaDirigida(int a,int b,int l)
{
    nextArista->l = l;
    nextArista->nodo = b;
    nextArista->siguiente = g[a];
    g[a] = nextArista;
    nextArista++;
}

void addArista(int a, int b, int l)
{
    addAristaDirigida(a,b,l);
    addAristaDirigida(b,a,l);
}

const int MAX_Q = 200000;

struct Query
{
    int index, runner, target;
    
    bool operator<(const Query &o) const
    {
        return target < o.target;
    }
    
} queries[MAX_Q];

int queryRangeStart[MAX_N];
int queryRangeEnd[MAX_N];

int queryRet[MAX_Q];

bool isLeaf(int node)
{
    assert(g[node]);
    return !g[node]->siguiente;
}

struct Winner
{
    int d, w;
    bool operator<(const Winner &o) const
    {
        if (d != o.d)
            return d < o.d;
        return w < o.w;
    }
    Winner extend(int node, int l) const
    {
        Winner ret = *this;
        if (isLeaf(node))
        {
            ret.d = 0;
            ret.w = node;
        }
        ret.d += l;
        return ret;
    }
};

Winner subtree[MAX_N];

const Winner EMPTY = {INF, -1};

//bool isLeaf(int node, int parent)
//{
//    return !g[node] || (g[node]->nodo == parent && !g[node]->siguiente);
//}

void subtreeDfs(int node, int parent)
{
    subtree[node] = EMPTY;
    for (Arista *edge = g[node]; edge; edge = edge->siguiente)
    if (edge->nodo != parent)
    {
        subtreeDfs(edge->nodo, node);
        Winner childWinner = subtree[edge->nodo].extend(edge->nodo, edge->l);
        subtree[node] = min(subtree[node], childWinner);
    }
}

Winner parentSubtree[MAX_N];
//Winner extendedParentSubtree[MAX_N];

const int ROOT = 0;

void parentDfs(int node, int parent, Winner parentWinner)
{
    assert(g[node]);
    //if (node == ROOT && !g[node]->siguiente)
    //{
    //    parentSubtree[g[node]->nodo] = {0, node};
    //    parentDfs(g[node]->nodo, node, g[node]->l);
    //    return;
    //}
    //extendedParentSubtree[node] = parentWinner;
    Winner best1 = EMPTY, best2 = EMPTY;
    int child1 = -1; 
    for (Arista *edge = g[node]; edge; edge = edge->siguiente)
    if (edge->nodo != parent)
    {
        int child = edge->nodo;
        Winner childWinner = subtree[child].extend(child, edge->l);
        if (childWinner < best1)
        {
            best2 = best1;
            best1 = childWinner;
            child1 = child;
        }
        else if (childWinner < best2)
            best2 = childWinner;
    }
    for (Arista *edge = g[node]; edge; edge = edge->siguiente)
    if (edge->nodo != parent)
    {
        int 
        
        
        child = edge->nodo;
        if (child1 == child)
            parentSubtree[child] = min(best2, parentWinner);
        else
            parentSubtree[child] = min(best1, parentWinner);
        parentDfs(child, node, parentSubtree[child].extend(node, edge->l));
    }
}

Winner whoWins[MAX_N];
int winCount[MAX_N];

void update(int node, Winner newValue)
{
    winCount[whoWins[node].w]--;
    whoWins[node] = newValue;
    winCount[whoWins[node].w]++;
}

void dfs(int node, int parent)
{
    forsn(index, queryRangeStart[node], queryRangeEnd[node])
    {
        Query &query = queries[index];
        assert(query.target == node);
        queryRet[query.index] = winCount[query.runner];
    }
    
    for (Arista *edge = g[node]; edge; edge = edge->siguiente)
    if (edge->nodo != parent)
    {
        int child = edge->nodo;
        // MOVER DE node A child
        Winner oldNodeValue = whoWins[node];
        Winner oldChildValue = whoWins[child];
        if (whoWins[node].w != whoWins[child].w)
        {
            Winner candidate = {whoWins[node].d + edge->l, whoWins[node].w};
            if (candidate < whoWins[child])
                update(child, candidate);
        }
        if (!isLeaf(node))
            update(node, parentSubtree[child]);
        dfs(child, node);
        update(node, oldNodeValue);
        update(child, oldChildValue);
    }
}

int main()
{
    osRead();
    inputPtr = input;
    
    int N = int(readInt());
    forn(edgeIndex,N-1)
    {
        int u = int(readInt());
        int v = int(readInt());
        int l = int(readInt());
        u--; v--;
        addArista(u,v,l);
    }

    subtreeDfs(ROOT, -1);
    parentSubtree[ROOT] = EMPTY;
    //extendedParentSubtree[ROOT] = EMPTY;
    parentDfs(ROOT, -1, EMPTY);
    
    forn(i,N)
    {
        //Winner nodeWinner = subtree[i]; //min(extendedParentSubtree[i], subtree[i]);
        Winner nodeWinner;
        if (isLeaf(i))
            nodeWinner = {0, i};
        else
            nodeWinner = subtree[i];
        //if (nodeWinner.w < 0)
        //{
        //    printInt(i);
        //    printLn();
        //    printInt(nodeWinner.d);
        //    printLn();
        //    printInt(nodeWinner.w);
        //    printLn();
        //    printLn();
        //    osWrite();
        //}
        assert(nodeWinner.w >= 0);
        whoWins[i] = nodeWinner;
        //printInt(whoWins[i]+1);
        //printLn();
        winCount[nodeWinner.w]++;
    }
    //printLn();

    int Q = int(readInt());
    forn(queryIndex,Q)
    {
        Query &query = queries[queryIndex];
        query.index = queryIndex;
        query.runner = int(readInt()) - 1;
        query.target = int(readInt()) - 1;
        //if (query.target == ROOT)
        //    queryRet[queryIndex] = winCount[query.runner];
    }
    quickSort(queries, queries+Q);
    int rangeStart = 0;
    forn(target, N)
    {
        int rangeEnd = rangeStart;
        while (rangeEnd < Q && queries[rangeEnd].target == target)
            rangeEnd++;
        queryRangeStart[target] = rangeStart;
        queryRangeEnd[target] = rangeEnd;
        rangeStart = rangeEnd;
    }
    assert(rangeStart == Q);
    
    dfs(ROOT, -1);
    
    forn(queryIndex, Q)
    {
        printInt(queryRet[queryIndex]);
        printLn();
    }
    
    osWrite();
    return 0;
}


