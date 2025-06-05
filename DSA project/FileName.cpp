#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <random>
#include <stdexcept>
#include <iomanip>            
#include <limits>              
#include <unordered_set>

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
        cout << "  " << left << setw(20) << t->word << " [" << t->category << "]\n";
}

string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    size_t e = s.find_last_not_of(" \t\r\n");
    return (b == string::npos ? "" : s.substr(b, e - b + 1));
}

// Utility functions for AVL balancing
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

// Insert a new node into the AVL tree and balance if necessary
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
    while (getline(ss, t, d))
        if (!t.empty()) out.push_back(trim(t));
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

// BST LOGIC FUNCTIONS HERE 
//Node* buildSlightlyUnbalancedBST(const vector<Node*>& sortedNodes, int start, int end) {
//    if (start > end) return nullptr;
//
//    int range = end - start + 1;
//    int offsetStart = start + range * 4 / 10;
//    int offsetEnd = start + range * 6 / 10;
//
//    // Clamp to valid index range
//    offsetStart = max(start, min(offsetStart, end));
//    offsetEnd = max(start, min(offsetEnd, end));
//
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_int_distribution<> dist(offsetStart, offsetEnd);
//    int mid = dist(gen);
//
//    Node* node = sortedNodes[mid];
//    node->left = buildSlightlyUnbalancedBST(sortedNodes, start, mid - 1);
//    node->right = buildSlightlyUnbalancedBST(sortedNodes, mid + 1, end);
//    return node;
//}
//
//Node* buildBalancedBST(const vector<Node*>& sortedNodes, int start, int end) {
//    if (start > end) return nullptr;
//    int mid = start + (end - start) / 2;
//    Node* node = sortedNodes[mid];
//    node->left = buildBalancedBST(sortedNodes, start, mid - 1);
//    node->right = buildBalancedBST(sortedNodes, mid + 1, end);
//    return node;
//}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    map<string, Node*> M;
    // 1) parse meanings
    ifstream mf("merged_dictionary.txt");
    string L;
    while (getline(mf, L)) {
        size_t po = L.find('('), pc = L.find(')'), co = L.find(':');
        if (po == string::npos || pc == string::npos || co == string::npos || po == 0) continue;
        string w = L.substr(0, po - 1), p = L.substr(po + 1, pc - po - 1), m = L.substr(co + 2);
        if (!M.count(w)) M[w] = new Node(w);
        M[w]->addEntry(p, m);
    }
    // 2) parse synonyms
    auto synB = parseBlocks("synonym.txt", { "Word:","POS:","Synonyms:" });
    for (auto& blk : synB) {
        string w = blk["Word:"], p = blk["POS:"], list = blk["Synonyms:"];
        auto it = M.find(w); if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx; auto pi = n->pos_index.find(p);
        if (pi == n->pos_index.end()) idx = n->addEntry(p, "");
        else idx = pi->second;
        n->synonym[idx] = split(list, ';');
    }
    // 3) parse antonyms
    auto antB = parseBlocks("antonym.txt", { "Word:","POS:","Antonyms:" });
    for (auto& blk : antB) {
        string w = blk["Word:"], p = blk["POS:"], list = blk["Antonyms:"];
        auto it = M.find(w); if (it == M.end()) continue;
        Node* n = it->second;
        size_t idx; auto pi = n->pos_index.find(p);
        if (pi == n->pos_index.end()) idx = n->addEntry(p, "");
        else idx = pi->second;
        n->antonym[idx] = split(list, ';');
    }

    // build AVL & collect all words
    vector<Node*> Allwords;
    Node* root = nullptr;
    for (auto& kv : M) {
        root = insertAVL(root, kv.second);
        Allwords.push_back(kv.second);
    }

    // Word of the Day
    srand(time(0));
    int randomNum = rand() % Allwords.size();
    Node* numd = Allwords[randomNum];

    // BST LOGIC HERE 
// Sort nodes by word (if they aren't already sorted)
//sort(allWords.begin(), allWords.end(), [](Node* a, Node* b) { return a->word < b->word; });

//// Build balanced BST
///*Node* root = buildBalancedBST(allWords, 0, allWords.size() - 1);*/
//Node* root = buildSlightlyUnbalancedBST(allWords, 0, allWords.size() - 1);

//srand(time(0));
//int randomNum = rand() % allWords.size();
//Node* numd = allWords[randomNum];


    cout << "--- Electronic Dictionary ---\n\n"
        //cout << "Height of the Tree using AVL: " << root->height << endl;
        << "Word of the Day: " << numd->word << "\n\n";

    int choice;
    string w;
    Node* n;
    while (true) {
        cout << "1. Meaning\n"
            << "2. Synonyms\n"
            << "3. Antonyms\n"
            << "4. History\n"
            << "5. New Word of the Day\n"
            << "6. Lookup by Exact Meaning\n"
            << "7. Exit\n"
            << "Choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! please enter a number 1 - 7.\n\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 7) break;

        try {
            switch (choice) {
            case 1: {
                cout << "Enter word: ";
                getline(cin, w);
                insertInHistory(w, "meaning");
                n = searchNode(w, root);
                if (!n) throw runtime_error("Word not found.");

                unordered_map<string, vector<string>> meanings_by_pos;
                for (size_t i = 0; i < n->pos.size(); i++) {
                    const string& part_of_speech = n->pos[i];
                    const string& definition = n->meaning[i];
                    if (!definition.empty()) {
                        meanings_by_pos[part_of_speech].push_back(definition);
                    }
                }

                for (auto& kv : meanings_by_pos) {
                    const string& part_of_speech = kv.first;
                    vector<string>& defs = kv.second;
                    if (defs.empty()) continue;

                    cout << w << " (" << part_of_speech << ")\n";
                    size_t to_show = min(defs.size(), size_t(2));
                    for (size_t idx = 0; idx < to_show; idx++) {
                        cout << "  " << (idx + 1) << ". " << defs[idx] << "\n";
                    }
                    cout << "\n";
                }

                if (meanings_by_pos.empty()) {
                    cout << "No definitions found for \"" << w << "\".\n\n";
                }
                break;
            }


            case 2: {
                try {
                    cout << "Enter word: ";
                    getline(cin, w);

                    if (w.empty()) throw invalid_argument("No word entered.");

                    insertInHistory(w, "synonym");
                    n = searchNode(w, root);
                    if (!n) throw runtime_error("Word not found.");

                    if (n->pos.size() != n->synonym.size())
                        throw logic_error("Data mismatch: POS and synonym counts differ.");

                    unordered_map<string, vector<string>> syn_by_pos;

                    for (size_t i = 0; i < n->pos.size(); i++) {
                        const string& pos_tag = n->pos[i];
                        const vector<string>& raw = n->synonym[i];

                        unordered_set<string> collector;
                        for (auto& chunk : raw) {
                            if (chunk.empty()) continue;
                            size_t start = 0;
                            while (true) {
                                size_t bar = chunk.find('|', start);
                                string piece = (bar == string::npos) ?
                                    chunk.substr(start) : chunk.substr(start, bar - start);

                                piece = trim(piece);
                                if (!piece.empty()) collector.insert(piece);

                                if (bar == string::npos) break;
                                start = bar + 1;
                            }
                        }

                        if (!collector.empty()) {
                            vector<string> flatList(collector.begin(), collector.end());
                            syn_by_pos[pos_tag] = move(flatList);
                        }
                    }

                    if (!syn_by_pos.empty()) {
                        for (auto& kv : syn_by_pos) {
                            cout << w << " (" << kv.first << ")\n";
                            cout << "  Synonyms: ";
                            for (size_t i = 0; i < min(kv.second.size(), size_t(3)); i++) {
                                cout << kv.second[i];
                                if (i + 1 < min(kv.second.size(), size_t(3))) cout << ", ";
                            }
                            cout << "\n\n";
                        }
                    }
                    else {
                        cout << "No synonyms found for \"" << w << "\".\n\n";
                    }
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n\n";
                }
                break;
            }


            case 3: {
                try {
                    cout << "Enter word: ";
                    getline(cin, w);

                    if (w.empty()) throw invalid_argument("No word entered.");

                    insertInHistory(w, "antonym");
                    n = searchNode(w, root);
                    if (!n) throw runtime_error("Word not found.");

                    if (n->pos.size() != n->antonym.size())
                        throw logic_error("Data mismatch: POS and antonym counts differ.");

                    unordered_map<string, vector<string>> ant_by_pos;

                    for (size_t i = 0; i < n->pos.size(); i++) {
                        const string& pos_tag = n->pos[i];
                        const vector<string>& raw = n->antonym[i];

                        unordered_set<string> collector;
                        for (auto& chunk : raw) {
                            if (chunk.empty()) continue;
                            size_t start = 0;
                            while (true) {
                                size_t bar = chunk.find('|', start);
                                string piece = (bar == string::npos) ?
                                    chunk.substr(start) : chunk.substr(start, bar - start);

                                piece = trim(piece);
                                if (!piece.empty()) collector.insert(piece);

                                if (bar == string::npos) break;
                                start = bar + 1;
                            }
                        }

                        if (!collector.empty()) {
                            vector<string> flatList(collector.begin(), collector.end());
                            ant_by_pos[pos_tag] = move(flatList);
                        }
                    }

                    if (!ant_by_pos.empty()) {
                        for (auto& kv : ant_by_pos) {
                            cout << w << " (" << kv.first << ")\n";
                            cout << "  Antonyms: ";
                            for (size_t i = 0; i < min(kv.second.size(), size_t(3)); i++) {
                                cout << kv.second[i];
                                if (i + 1 < min(kv.second.size(), size_t(3))) cout << ", ";
                            }
                            cout << "\n\n";
                        }
                    }
                    else {
                        cout << "No antonyms found for \"" << w << "\".\n\n";
                    }
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n\n";
                }
                break;
            }


            case 4:
                printHistory();
                break;

            case 5:
                randomNum = rand() % Allwords.size();
                numd = Allwords[randomNum];
                cout << "Word of the Day: " << numd->word << "\n";
                break;

            case 6: {
                cout << "Enter exact meaning to look up: ";
                string m; getline(cin, m);
                bool found = false;
                for (auto* nn : Allwords) {
                    for (size_t i = 0; i < nn->meaning.size(); i++) {
                        if (nn->meaning[i] == m) {
                            cout << nn->word << " (" << nn->pos[i] << ")\n";
                            found = true;
                        }
                    }
                }
                if (!found) cout << "No word found with that exact meaning.\n";
                break;
            }

            default:
                cout << "Invalid choice! please select 1 - 7.\n";
            }
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
        cout << "\n";
    }
    return 0;
}