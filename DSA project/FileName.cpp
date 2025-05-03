#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include<random>
#include <stdexcept>

using namespace std;

struct Node {
    string word;
    vector<string> pos, meaning;
    vector<vector<string>> synonym, antonym;
    unordered_map<string, size_t> pos_index;
    Node* left = nullptr;
    Node* right = nullptr;
    int height = 1;

    Node(const string& w) : word(w) {}
    size_t addEntry(const string& p, const string& m) {
        size_t idx = pos.size();
        pos.push_back(p);
        meaning.push_back(m);
        synonym.emplace_back();
        antonym.emplace_back();
        pos_index[p] = idx;
        return idx;
    }
};

struct history {
    string word, category;
    history* next;
    history(string w, string c) : word(w), category(c), next(nullptr) {}
};
history* head = nullptr;
void insertInHistory(const string& w, const string& c) {
    history* nn = new history(w, c);
    nn->next = head;
    head = nn;
}
void printHistory() {
    for (history* t = head; t; t = t->next)
        cout << t->word << " (" << t->category << ")\n";
}

string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    size_t e = s.find_last_not_of(" \t\r\n");
    return (b == string::npos ? "" : s.substr(b, e - b + 1));
}

int heightOf(Node* n) { return n ? n->height : 0; }
int balanceOf(Node* n) { return n ? heightOf(n->left) - heightOf(n->right) : 0; }

Node* rightRotate(Node* y) {
    Node* x = y->left; Node* T2 = x->right;
    x->right = y; y->left = T2;
    y->height = max(heightOf(y->left), heightOf(y->right)) + 1;
    x->height = max(heightOf(x->left), heightOf(x->right)) + 1;
    return x;
}
Node* leftRotate(Node* x) {
    Node* y = x->right; Node* T2 = y->left;
    y->left = x; x->right = T2;
    x->height = max(heightOf(x->left), heightOf(x->right)) + 1;
    y->height = max(heightOf(y->left), heightOf(y->right)) + 1;
    return y;
}

Node* insertAVL(Node* root, Node* node) {
    if (!root) return node;
    if (node->word < root->word) root->left = insertAVL(root->left, node);
    else if (node->word > root->word) root->right = insertAVL(root->right, node);
    else return root;
    root->height = 1 + max(heightOf(root->left), heightOf(root->right));
    int bf = balanceOf(root);
    if (bf > 1 && node->word < root->left->word) return rightRotate(root);
    if (bf < -1 && node->word > root->right->word) return leftRotate(root);
    if (bf > 1 && node->word > root->left->word) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (bf < -1 && node->word < root->right->word) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

vector<string> split(const string& s, char d) {
    vector<string> out; string t;
    istringstream ss(s);
    while (getline(ss, t, d)) if (!t.empty()) out.push_back(trim(t));
    return out;
}

vector<map<string, string>> parseBlocks(const string& fname, const vector<string>& fieldNames) {
    ifstream in(fname);
    vector<map<string, string>> blocks;
    map<string, string> cur;
    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) {
            if (!cur.empty()) { blocks.push_back(cur); cur.clear(); }
            continue;
        }
        for (auto& f : fieldNames) {
            if (line.rfind(f, 0) == 0) {
                cur[f] = trim(line.substr(f.size()));
                break;
            }
        }
    }
    if (!cur.empty()) blocks.push_back(cur);
    return blocks;
}

Node* searchNode(const string& w, Node* root) {
    if (!root) return nullptr;
    if (w < root->word) return searchNode(w, root->left);
    if (w > root->word) return searchNode(w, root->right);
    return root;
}

int main() {
    map<string, Node*> M;
    // 1) meanings
    ifstream mf("merged_dictionary.txt");
    string L;
    while (getline(mf, L)) {
        size_t po = L.find('('), pc = L.find(')'), co = L.find(':');
        if (po == string::npos || pc == string::npos || co == string::npos) continue;
        string w = L.substr(0, po - 1), p = L.substr(po + 1, pc - po - 1), m = L.substr(co + 2);
        if (!M.count(w)) M[w] = new Node(w);
        M[w]->addEntry(p, m);
    }
    // 2) synonyms
    auto synB = parseBlocks("synonym.txt", { "Word:","POS:","Synonyms:" });
    for (auto& blk : synB) {
        string w = blk["Word:"], p = blk["POS:"], list = blk["Synonyms:"];
        auto it = M.find(w); if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx;
        auto pi = n->pos_index.find(p);
        if (pi == n->pos_index.end()) idx = n->addEntry(p, "");
        else idx = pi->second;
        n->synonym[idx] = split(list, ';');
    }
    // 3) antonyms
    auto antB = parseBlocks("antonym.txt", { "Word:","POS:","Antonyms:" });
    for (auto& blk : antB) {
        string w = blk["Word:"], p = blk["POS:"], list = blk["Antonyms:"];
        auto it = M.find(w); if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx;
        auto pi = n->pos_index.find(p);
        if (pi == n->pos_index.end()) idx = n->addEntry(p, "");
        else idx = pi->second;
        n->antonym[idx] = split(list, ';');
    }
    // build AVL
    vector<Node*> Allwords;
    Node* root = nullptr;
    for (auto& kv : M) {
        root = insertAVL(root, kv.second);
        Allwords.push_back(kv.second);
    }

    srand(time(0));
    int randomNum = rand() % Allwords.size();
    Node* numd = Allwords[randomNum];

    int choice; string w; Node* n;
    cout << "--- Electronic Dictionary ---\n\n";
    cout <<"Word of the Day: " << numd->word<< endl;
    while (true) {
        cout << "\n1. Meaning\n2. Synonyms\n3. Antonyms\n4. History\n5. Exit\nChoice: ";
        cin >> choice; cin.ignore();
        if (choice == 5) break;
        try {
            switch (choice) {
            case 1:
                cout << "Enter word: "; getline(cin, w);
                insertInHistory(w, "meaning");
                n = searchNode(w, root);
                if (!n) throw runtime_error("Word not found.");
                for (size_t i = 0;i < n->pos.size();i++)
                    cout << w << " (" << n->pos[i] << "): " << n->meaning[i] << "\n";
                break;
            case 2:
                cout << "Enter word: "; getline(cin, w);
                insertInHistory(w, "synonym");
                n = searchNode(w, root);
                if (!n) throw runtime_error("Word not found.");
                for (size_t i = 0;i < n->pos.size();i++) {
                    for (auto& a : n->synonym[i]) {
                        if (a == "") {
                            continue;
                        }
                        cout << w << " (" << n->pos[i] << "): " << a << " " << endl;
                    }
                }
                break;
            case 3:
                cout << "Enter word: "; getline(cin, w);
                insertInHistory(w, "antonym");
                n = searchNode(w, root);
                if (!n) throw runtime_error("Word not found.");
                for (size_t i = 0;i < n->pos.size();i++) {
                    for (auto& a: n->antonym[i]) {
                        if (a == "") {
                            continue;
                        }
                        cout << w << " (" << n->pos[i] << "): " << a << " " << endl;
                    }
                }
                break;
            case 4:
                printHistory();
                break;
            default:
                 randomNum = rand() % Allwords.size();
                 numd = Allwords[randomNum];
                cout << "Word of the Day: " << numd->word << endl;
                //cout << "Invalid choice." << "\n";
            }
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
