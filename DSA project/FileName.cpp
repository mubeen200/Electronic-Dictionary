#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include<unordered_map>

using namespace std;

struct Node {
    string word;
    vector<string> pos, meaning;
    vector<vector<string>> synonym, antonym;
    unordered_map<string, size_t> pos_index;
    Node* left = nullptr;  Node* right = nullptr;  int height = 1;

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
    string word;
    string category;
    history* next;
    history(string word, string category): word(word), category(category), next(nullptr) {}
};
history* head = nullptr;

void insertInHistory(string word, string category) {
    history* nn = new history(word, category);
    if (!head) {
        head = nn;
        return;
    }
    nn->next = head;
    head = nn;
    return;
}

void deleteInHistory() {
    if (!head) {
        return;
    }
    history* temp1 = head;
    head = head->next;
    delete temp1;
    return;
}

void printHistory() {
    history* temp = head;
    while (temp != NULL) {
        cout << temp->word<< " "<< temp->category << endl;
        temp = temp->next;
    }
    return;
}

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos || end == string::npos) ? "" : s.substr(start, end - start + 1);
}


int heightOf(Node* n) { return n ? n->height : 0; }
int balanceOf(Node* n) { return n ? heightOf(n->left) - heightOf(n->right) : 0; }

Node* rightRotate(Node* y) {
    Node* x = y->left;  Node* T2 = x->right;
    x->right = y;  y->left = T2;
    y->height = max(heightOf(y->left), heightOf(y->right)) + 1;
    x->height = max(heightOf(x->left), heightOf(x->right)) + 1;
    return x;
}
Node* leftRotate(Node* x) {
    Node* y = x->right;  Node* T2 = y->left;
    y->left = x;  x->right = T2;
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

    if (bf<-1 && node->word>root->right->word) return leftRotate(root);

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

void printInOrder(Node* r) {
    if (!r) return;
    printInOrder(r->left);
    cout << "Word: " << r->word << "\n";
    for (size_t i = 0;i < r->pos.size();i++) {
        cout << "  POS: " << r->pos[i] << "\n"
            << "  Meaning: " << r->meaning[i] << "\n"
            << "  Syn: ";
        for (auto& s : r->synonym[i]) cout << s << "; ";
        cout << "\n  Ant: ";
        for (auto& a : r->antonym[i]) cout << a << "; ";
        cout << "\n\n";
    }
    printInOrder(r->right);
}

// split on delimiter
vector<string> split(const string& s, char d) {
    vector<string> out; string t;
    istringstream ss(s);
    while (getline(ss, t, d)) if (!t.empty()) out.push_back(t);
    return out;
}

// parse a “block” file like parsed_output.txt
// fieldNames e.g. {"Word:","POS:","Meaning:","Synonyms:"}
vector<map<string, string>> parseBlocks(const string& fname, const vector<string>& fieldNames) {
    ifstream in(fname);
    vector<map<string, string>> blocks;
    map<string, string> cur;
    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) {
            if (!cur.empty()) {
                blocks.push_back(cur);
                cur.clear();
            }
            continue;
        }
        for (auto& f : fieldNames) {
            if (line.rfind(f, 0) == 0) {
                string value = line.substr(f.size());
                cur[f] = trim(value);
                break;
            }
        }
    }
    if (!cur.empty()) blocks.push_back(cur);
    return blocks;
}

Node* search(string word, 
   /* map<string, Node*> M*/
    Node* root
) {
    /*Node* temp = M[word];
    return temp->meaning;*/
    vector<string> t;
    if (!root) return nullptr;
    if (word < root->word) {
        return search(word, root->left);
    }
    else if (word > root->word) {
        return search(word, root->right);
    }
    return root;
}

int main() {
    map<string, Node*> M;

    // 1) parse meanings.txt (format: word (POS): meaning)
    ifstream meanF("merged_dictionary.txt");
    string L;
    while (getline(meanF, L)) {
        size_t po = L.find('('), pc = L.find(')'), co = L.find(':');
        if (po == string::npos || pc == string::npos || co == string::npos) continue;
        string w = L.substr(0, po - 1),
            p = L.substr(po + 1, pc - po - 1),
            m = L.substr(co + 2);
        if (!M.count(w)) M[w] = new Node(w);
       
        M[w]->addEntry(p, m);
    }

    // 2) parse synonyms.txt (blocks with Word:, POS:, Synonyms:)
    auto synBlocks = parseBlocks("synonym.txt",
        { "Word:","POS:","Synonyms:" });
    for (auto& blk : synBlocks) {
        string w = blk["Word:"], p = blk["POS:"];
        string list = blk["Synonyms:"];
        auto it = M.find(w);
        if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx;
        auto posIt = n->pos_index.find(p);
        if (posIt == n->pos_index.end()) {
            idx = n->addEntry(p, "");
        }
        else {
            idx = posIt->second;
        }
        n->synonym[idx] = split(list, ';');
    }
    

    // 3) parse antonyms.txt (blocks with Word:, POS:, Antonyms:)
    auto antBlocks = parseBlocks("antonym.txt",
        { "Word:","POS:","Antonyms:" });
    for (auto& blk : antBlocks) {
        string w = blk["Word:"], p = blk["POS:"];
        string list = blk["Antonyms:"];
        auto it = M.find(w);
        if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx;
        auto posIt = n->pos_index.find(p);
        if (posIt == n->pos_index.end()) idx = n->addEntry(p, "");
        else idx = posIt->second;
        n->antonym[idx] = split(list, ';');
    }

    // 4) build AVL
    Node* root = nullptr;
    for (auto& kv : M) root = insertAVL(root, kv.second);

    // 5) print
    //printInOrder(root);

    int choice;
    cout << "-------------Welcome To Electronic Dictionary-------------" << endl << endl;
    bool go = true;
    while (go){
        cout << "  ENTER YOUR CHOICE   " << endl << endl;
    cout << "1.Meaning of the Word" << endl;
    cout << "2.Synonym of the Word" << endl;
    cout << "3.Part of the Speech" << endl;
    cout << "4.Example Sentence" << endl;
    cout << "5.Word of the Day" << endl;
    cout << "6.Search history of the words" << endl;
    cout << "7. Exit" << endl;
    cin >> choice;
    cin.ignore();
    string w;
    Node* n;
    //string meaning;

    switch (choice)
    {
    case 1:
        cout << "Enter the word: ";
        getline(cin, w);
        insertInHistory(w, "meaning");
        n = search(w, root);
       
        for (int i = 0; i < n->pos.size();i++) {
            cout << w << "(" << n->pos[i] << ") : " << n->meaning[i] << endl;
        }
        break;
    

    case 2:
        cout << "Enter the word: ";
        getline(cin, w);
        insertInHistory(w, "synonym");
         n = search(w, root);

        for (int i = 0; i < n->pos.size();i++) {
            for (int j = 0; j < n->synonym.size();j++) {
                for (auto a : n->synonym[j]) {
                    cout << a << " " << endl;
                }
            }
        }
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        printHistory();
        break;
    case 7:
        go = false;
        break;
    default:
        cout << "Wrong Choice!";
        break;
    }
}
}