#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <windows.h>
#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
using namespace std;

const int MVNum = 100; // ��󶥵���
typedef int VerTexType; // ������������
typedef int Otherlnfo; // ��Ȩֵ����

// �߽��
struct ArcNode {
    int adjvex; // �ڽӵ��������е��±�
    Otherlnfo info; // ��Ȩֵ
    ArcNode* nextaec; // ָ����һ���ߵ�ָ��
};

// ������
struct VNode {
    VerTexType data; // ��������
    ArcNode* firstarc; // ָ���һ���ߵ�ָ��
};

typedef VNode AdjList[MVNum]; // �ڽӱ�����

// �ڽӱ�洢��ͼ
struct ALGraph {
    AdjList vertices; // �ڽӱ�����
    int venxnum, arcnum; // �������ͱ���
};

// �ߣ����� Kruskal �㷨������
struct Edge {
    int u, v, w;
    Edge(int u, int v, int w) : u(u), v(v), w(w) {}
};

// ���ݶ������ݲ��Ҷ����������е��±�
int LocateVex(ALGraph& G, VerTexType v) {
    for (int i = 0; i < G.venxnum; i++) {
        if (G.vertices[i].data == v) {
            return i;
        }
    }
    return -1;
}

// �߱ȽϺ���������Ȩֵ��С��������
bool cmp(Edge a, Edge b) {
    return a.w < b.w;
}

// ���鼯���Һ���������Ԫ�����ڼ��ϵĸ����
int find(vector<int>& parent, int x) {
    if (parent[x] != x) {
        parent[x] = find(parent, parent[x]);
    }
    return parent[x];
}

// ���鼯�ϲ��������ϲ���������
void unionn(vector<int>& parent, vector<int>& rank, int x, int y) {
    int rootx = find(parent, x);
    int rooty = find(parent, y);
    if (rootx != rooty) {
        if (rank[rootx] > rank[rooty]) {
            parent[rooty] = rootx;
        }
        else if (rank[rootx] < rank[rooty]) {
            parent[rootx] = rooty;
        }
        else {
            parent[rooty] = rootx;
            rank[rootx] += 1;
        }
    }
}

// ��������ͼ��������ȡ�ļ�����ͼ
void CreateUDG(ALGraph& G) {
    string filename;
    cout << "������Ҫ��ȡ���ļ�����";
    cin >> filename;
    FILE* fp;
    errno_t err = freopen_s(&fp, filename.c_str(), "r", stdin);
    if (err != 0) {
        cout << "�޷����ļ���" << endl;
        exit(1);
    }

    cin >> G.venxnum >> G.arcnum; // ���ļ��ж�ȡ�������ͱ���
    for (int i = 0; i < G.venxnum; i++) { // ����ÿ������
        cin >> G.vertices[i].data; // ���ļ��ж�ȡ��������
        G.vertices[i].firstarc = NULL;
    }
    for (int k = 0; k < G.arcnum; k++) { // ����ÿ����
        VerTexType v1, v2;
        Otherlnfo w;
        cin >> v1 >> v2 >> w; // ���ļ��ж�ȡ�ߵ���������ͱ�Ȩֵ
        int i = LocateVex(G, v1); // ���Ҷ��� v1 �������е��±�
        int j = LocateVex(G, v2); // ���Ҷ��� v2 �������е��±�

        ArcNode* p1 = new ArcNode;
        p1->adjvex = j; // �����ڽӵ��±�
        p1->info = w; // ���ñ�Ȩֵ
        p1->nextaec = G.vertices[i].firstarc;
        G.vertices[i].firstarc = p1;

        ArcNode* p2 = new ArcNode;
        p2->adjvex = i; // �����ڽӵ��±�
        p2->info = w; // ���ñ�Ȩֵ
        p2->nextaec = G.vertices[j].firstarc;
        G.vertices[j].firstarc = p2;
    }
}

vector<pair<float, float>> nodes;
vector<pair<int, int>> edges;

string filePath = "nm.txt"; // ��һ��ͼ����ļ�·��
string nextFilePath = "xzj.txt"; // �ڶ���ͼ����ļ�·��
int timerId = -1;
bool isFirstFrame = true;

void readGraphFromFile(const string& filePath) {
    edges.clear();
    nodes.clear();
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return;
    }

    int numNodes, numEdges;
    file >> numNodes;
    for (int i = 0; i < numNodes; i++) {
        float x, y;
        file >> x >> y;
        nodes.emplace_back(x, y);
    }

    file >> numEdges;
    for (int i = 0; i < numEdges; i++) {
        int a, b;
        file >> a >> b;
        edges.emplace_back(a - 1, b - 1);
    }

    file.close();
}

// Kruskal �㷨����С������
void Kruskal(ALGraph& G) {
    vector<Edge> edges; // �洢���б�
    for (int i = 0; i < G.venxnum; i++) { // �������ж���ͱߣ����ߴ���������
        for (ArcNode* p = G.vertices[i].firstarc; p; p = p->nextaec) {
            if (i < p->adjvex) { // �����ظ���ӱ�
                edges.push_back(Edge(i, p->adjvex, p->info));
            }
        }
    }
    sort(edges.begin(), edges.end(), cmp); // ���߰�Ȩֵ��С��������
    vector<int> parent(G.venxnum), rank(G.venxnum); // ��ʼ�����鼯
    for (int i = 0; i < G.venxnum; i++) {
        parent[i] = i;
        rank[i] = 1;
    }
    int ans = 0; // ��С����������Ȩֵ
    cout << "��С�������ı�Ϊ��\n";
    for (auto e : edges) { // �������б�
        int u = e.u, v = e.v, w = e.w;
        if (find(parent, u) != find(parent, v)) { // ����������㲻��ͬһ�������У�˵�������߲����γɻ�
            ans += w;
            unionn(parent, rank, u, v); // �ϲ���������
            cout << G.vertices[u].data << " " << G.vertices[v].data << " " << w << endl;
        }
    }
    cout << "��С����������ȨֵΪ��" << ans << endl;
}

void display()
{
    if (isFirstFrame) {
        readGraphFromFile(filePath);
        isFirstFrame = false;
    }
    else {
        readGraphFromFile(nextFilePath);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    // ���Ʊ�
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    for (auto edge : edges) {
        glVertex2f(nodes[edge.first].first, nodes[edge.first].second);
        glVertex2f(nodes[edge.second].first, nodes[edge.second].second);
    }
    glEnd();

    // ���ƽڵ�
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (auto node : nodes) {
        glVertex2f(node.first, node.second);
    }
    glEnd();
    glFlush();
}
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(3000, timer, 0);
}

int main(int argc, char** argv)
{
    ALGraph G;
    CreateUDG(G);
    Kruskal(G);
    glutInit(&argc, argv);
    glutCreateWindow("Undirected Graph");
    glutInitWindowSize(320, 320);
    glutInitWindowPosition(50, 50);
    glutDisplayFunc(display);
    glutTimerFunc(3000, timer, 0);// ע�ᶨʱ���ص�����
    glutMainLoop();
    return 0;
}