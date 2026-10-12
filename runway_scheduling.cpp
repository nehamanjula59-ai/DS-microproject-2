#include <iostream>
#include <string>
using namespace std;

struct Flight {
    string id, type;
    int passengers, delay, urgency;
    float fuel, distance, priority;
    bool emergency;
};

struct Runway {
    int id;
    bool available;
};

struct Node {
    Flight f;
    Node *left, *right;
};

// Calculate flight priority
float getPriority(Flight f) {
    float p = f.urgency * 10;
    p += (100 - f.fuel) * 0.5;
    p += f.delay * 0.5;
    p += (100 - f.distance) * 0.1;

    if (f.emergency)
        p += 100;

    return p;
}

// Linear Search
int searchFlight(Flight a[], int n, string id) {
    for (int i = 0; i < n; i++)
        if (a[i].id == id)
            return i;

    return -1;
}

// Selection Sort - highest priority first
void sortFlights(Flight a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int max = i;

        for (int j = i + 1; j < n; j++)
            if (a[j].priority > a[max].priority)
                max = j;

        Flight temp = a[i];
        a[i] = a[max];
        a[max] = temp;
    }
}

// Create BST node
Node* createNode(Flight f) {
    Node* p = new Node;
    p->f = f;
    p->left = p->right = NULL;
    return p;
}

// Insert into BST
Node* insert(Node* root, Flight f) {
    if (root == NULL)
        return createNode(f);

    if (f.priority > root->f.priority)
        root->left = insert(root->left, f);
    else
        root->right = insert(root->right, f);

    return root;
}

// Display BST in priority order
void inorder(Node* root) {
    if (root == NULL)
        return;

    inorder(root->left);

    cout << root->f.id << "  Priority: "
         << root->f.priority << endl;

    inorder(root->right);
}

// Display flights
void display(Flight a[], int n) {
    cout << "\nID\tType\tFuel\tDelay\tUrgency\tPriority\n";

    for (int i = 0; i < n; i++)
        cout << a[i].id << "\t"
             << a[i].type << "\t"
             << a[i].fuel << "\t"
             << a[i].delay << "\t"
             << a[i].urgency << "\t"
             << a[i].priority << endl;
}

// Schedule flights
void schedule(Flight a[], int n, Runway r[], int rn) {

    sortFlights(a, n);

    cout << "\n===== RUNWAY SCHEDULE =====\n";

    int runway = 0;

    for (int i = 0; i < n; i++) {

        while (runway < rn && !r[runway].available)
            runway++;

        if (runway == rn) {
            cout << "No runway available for "
                 << a[i].id << endl;
            continue;
        }

        cout << "Flight " << a[i].id
             << " -> Runway " << r[runway].id;

        if (a[i].emergency)
            cout << " [EMERGENCY]";

        cout << endl;

        r[runway].available = false;
        runway++;
    }
}

// ================= ADDED: MAX-HEAP PRIORITY QUEUE (CO-3) =================
struct MaxHeap {
    Flight h[20];
    int size;
};

void heapifyUp(MaxHeap &m, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (m.h[i].priority <= m.h[parent].priority)
            break;
        Flight temp = m.h[i];
        m.h[i] = m.h[parent];
        m.h[parent] = temp;
        i = parent;
    }
}

void heapifyDown(MaxHeap &m, int i) {
    while (true) {
        int l = 2 * i + 1, r = 2 * i + 2, big = i;
        if (l < m.size && m.h[l].priority > m.h[big].priority) big = l;
        if (r < m.size && m.h[r].priority > m.h[big].priority) big = r;
        if (big == i)
            break;
        Flight temp = m.h[i];
        m.h[i] = m.h[big];
        m.h[big] = temp;
        i = big;
    }
}

void heapInsert(MaxHeap &m, Flight f) {
    m.h[m.size] = f;
    heapifyUp(m, m.size);
    m.size++;
}

Flight heapExtract(MaxHeap &m) {
    Flight top = m.h[0];
    m.size--;
    m.h[0] = m.h[m.size];
    heapifyDown(m, 0);
    return top;
}

void buildHeap(MaxHeap &m, Flight a[], int n) {
    m.size = 0;
    for (int i = 0; i < n; i++)
        heapInsert(m, a[i]);
}

void heapOrder(MaxHeap m) {
    int slot = 1;
    while (m.size > 0) {
        Flight f = heapExtract(m);
        cout << "Slot " << slot++ << ": " << f.id
             << "  Priority: " << f.priority;
        if (f.emergency)
            cout << " [EMERGENCY]";
        cout << endl;
    }
}

// ================= ADDED: GRAPH + DIJKSTRA (CO-4) =================
const int V = 6;
const int INF = 9999;
string place[V] = {"Gate1", "Gate2", "TaxiA", "TaxiB", "TaxiC", "Runway"};
int graph[V][V] = {
    {0, 0, 2, 0, 0, 0},
    {0, 0, 3, 2, 0, 0},
    {2, 3, 0, 1, 4, 0},
    {0, 2, 1, 0, 3, 6},
    {0, 0, 4, 3, 0, 2},
    {0, 0, 0, 6, 2, 0}
};
bool closed = false;   // TaxiC -> Runway closure

void toggleClosure() {
    closed = !closed;
    graph[4][5] = graph[5][4] = closed ? 0 : 2;
}

void dijkstra(int src, int dest) {
    int dist[V], prev[V];
    bool done[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
        done[i] = false;
    }
    dist[src] = 0;

    for (int k = 0; k < V; k++) {
        int u = -1;
        for (int i = 0; i < V; i++)
            if (!done[i] && (u == -1 || dist[i] < dist[u]))
                u = i;

        if (dist[u] == INF)
            break;
        done[u] = true;

        for (int v = 0; v < V; v++)
            if (graph[u][v] != 0 && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                prev[v] = u;
            }
    }

    if (dist[dest] == INF) {
        cout << "No route available.\n";
        return;
    }

    int path[V], len = 0;
    for (int at = dest; at != -1; at = prev[at])
        path[len++] = at;

    cout << "Route: ";
    for (int i = len - 1; i >= 0; i--) {
        cout << place[path[i]];
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nTaxi time: " << dist[dest] << " min\n";
}

int main() {

    Flight flights[10] = {
        {"AI101", "A320", 160, 20, 5, 45, 35, 0, false},
        {"6E202", "B737", 180, 40, 7, 25, 20, 0, false},
        {"EK303", "B777", 300, 10, 4, 60, 50, 0, false},
        {"SG404", "ATR72", 70, 55, 8, 15, 15, 0, false},
        {"UK505", "A350", 280, 5, 10, 10, 10, 0, true}
    };

    int n = 5;

    // Calculate priorities
    for (int i = 0; i < n; i++)
        flights[i].priority = getPriority(flights[i]);

    Runway runways[3] = {
        {1, true},
        {2, true},
        {3, true}
    };

    int choice;

    do {
        cout << "\n==============================\n";
        cout << " AIRPORT RUNWAY SCHEDULING\n";
        cout << "==============================\n";
        cout << "1. Display Flights\n";
        cout << "2. Search Flight\n";
        cout << "3. Sort by Priority\n";
        cout << "4. Schedule Runways\n";
        cout << "5. Display Priority BST\n";
        cout << "6. Max-Heap Priority Order\n";
        cout << "7. Add Emergency Flight (Heap)\n";
        cout << "8. Shortest Taxi Route (Dijkstra)\n";
        cout << "9. Toggle TaxiC-Runway Closure\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {

            display(flights, n);

        } else if (choice == 2) {

            string id;
            cout << "Enter Flight ID: ";
            cin >> id;

            int pos = searchFlight(flights, n, id);

            if (pos == -1)
                cout << "Flight not found.\n";
            else
                cout << "Flight " << flights[pos].id
                     << " found. Priority = "
                     << flights[pos].priority << endl;

        } else if (choice == 3) {

            sortFlights(flights, n);
            cout << "Flights sorted by priority.\n";
            display(flights, n);

        } else if (choice == 4) {

            schedule(flights, n, runways, 3);

        } else if (choice == 5) {

            Node* root = NULL;

            for (int i = 0; i < n; i++)
                root = insert(root, flights[i]);

            cout << "\nFlights in Priority BST:\n";
            inorder(root);

        } else if (choice == 6) {

            MaxHeap m;
            buildHeap(m, flights, n);
            cout << "\nMax-Heap root: " << m.h[0].id
                 << "  Priority: " << m.h[0].priority << endl;
            cout << "Heap extraction order:\n";
            heapOrder(m);

        } else if (choice == 7) {

            if (n >= 10) {
                cout << "Flight list is full.\n";
            } else {
                string id;
                cout << "Enter Emergency Flight ID: ";
                cin >> id;

                if (searchFlight(flights, n, id) != -1) {
                    cout << "Flight ID already exists.\n";
                } else {
                    MaxHeap m;
                    buildHeap(m, flights, n);
                    cout << "Root before: " << m.h[0].id
                         << "  Priority: " << m.h[0].priority << endl;

                    Flight e = {id, "B737", 120, 0, 10, 5, 5, 0, true};
                    e.priority = getPriority(e);
                    flights[n++] = e;

                    heapInsert(m, e);
                    cout << "Root after:  " << m.h[0].id
                         << "  Priority: " << m.h[0].priority << endl;
                    cout << "Heap extraction order:\n";
                    heapOrder(m);
                }
            }

        } else if (choice == 8) {

            int g;
            cout << "Start gate (1 or 2): ";
            cin >> g;

            if (g == 1 || g == 2) {
                cout << "Taxiway TaxiC-Runway: "
                     << (closed ? "CLOSED" : "OPEN") << endl;
                dijkstra(g - 1, 5);
            } else {
                cout << "Invalid gate.\n";
            }

        } else if (choice == 9) {

            toggleClosure();
            cout << "TaxiC-Runway is now "
                 << (closed ? "CLOSED" : "OPEN") << endl;

        } else if (choice == 0) {

            cout << "Program ended.\n";

        } else {

            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
