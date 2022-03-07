#include <cstddef>
#include <iostream>
#include <algorithm>

template<class ValueType>
struct Node {
    Node* left;
    Node* right;
    Node* prev;
    ValueType value;
    int height;

    Node(const ValueType& value_) {
        value = value_;
        left = nullptr;
        right = nullptr;
        prev = nullptr;
        height = 1;
    }

    ~Node() {
        if (left != nullptr) {
            delete left;
        }
        if (right != nullptr) {
            delete right;
        }
    }

    Node* GetMaxElem(Node* root) const {
        if (root == nullptr) {
            return nullptr;
        }
        Node* current = root;
        while (current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    Node* GetNext(Node* root) const {
        if (root == nullptr) {
            return nullptr;
        }

        Node* current = root;
        if (current->right != nullptr) {
            current = current->right;
            while (current->left != nullptr) {
                current = current->left;
            }
            return current;
        }

        Node* last_current = nullptr;
        while (current != nullptr && current->right == last_current) {
            last_current = current;
            current = current->prev;
        }

        return current;
    }

    Node* GetPrev(Node* root) const {
        if (root == nullptr) {
            return nullptr;
        }

        Node* current = root;
        if (current->left != nullptr) {
            current = current->left;
            while (current->right != nullptr) {
                current = current->right;
            }
            return current;
        }

        Node* last_current = nullptr;
        while (current != nullptr && current->left == last_current) {
            last_current = current;
            current = current->prev;
        }

        return current;
    }
};

template<class ValueType>
class Set {
public:
    typedef Node<ValueType> Node;
    class iterator {
    public:
        iterator() :
                node_(nullptr), is_next_last_(true) {
        };

        iterator(Node* node, bool is_next_last) :
                node_(node), is_next_last_(is_next_last) {
        };

        iterator(const iterator& other) :
                node_(other.node_), is_next_last_(other.is_next_last_) {
        }

        iterator& operator= (const iterator& other) {
            node_ = other.node_;
            is_next_last_ = other.is_next_last_;
            return *this;
        }

        ValueType operator*() const {
            return node_->value;
        };

        ValueType* operator->() {
            return &(node_->value);
        }

        iterator& operator++() {
            Node* help_node = (*node_).GetMaxElem(node_);
            node_ = (*node_).GetNext(node_);
            if (node_ == nullptr) {
                node_ = help_node;
                is_next_last_ = true;
            }
            return *this;
        };

        iterator operator++(int) {
            Node* help_node = (*node_).GetMaxElem(node_);
            node_ = (*node_).GetNext(node_);
            if (node_ == nullptr) {
                node_ = help_node;
                is_next_last_ = true;
            }
            return *this;
        };

        iterator& operator--() {
            if (is_next_last_) {
                is_next_last_ = false;
                return *this;
            }
            node_ = (*node_).GetPrev(node_);
            return *this;
        };

        iterator operator--(int) {
            if (is_next_last_) {
                is_next_last_ = false;
                return *this;
            }
            node_ = (*node_).GetPrev(node_);
            return *this;
        };

        bool operator!= (const iterator& other) const {
            return (node_ != other.node_ || is_next_last_ != other.is_next_last_);
        };

        bool operator== (const iterator& other) const {
            return (node_ == other.node_ && is_next_last_ == other.is_next_last_);
        }

        ~iterator() = default;

    private:
        Node* node_;
        bool is_next_last_;
    };

    Set() {
        root_ = nullptr;
        size_ = 0;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        root_ = nullptr;
        size_ = 0;
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<ValueType> elems) {
        root_ = nullptr;
        size_ = 0;
        for (size_t i = 0; i < elems.size(); ++i) {
            insert(elems.begin()[i]);
        }
    }

    Set(const Set& other) {
        size_ = other.size_;
        root_ = CopyTree(other.root_, nullptr);
    }

    Set& operator=(const Set& other) {
        if (this == &other) {
            return *this;
        }
        delete root_;
        size_ = other.size_;
        root_ = CopyTree(other.root_, nullptr);
        return *this;
    }

    ~Set() {
        delete root_;
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const ValueType& elem) {
        root_ = InsertElem(root_, nullptr, elem);
    }

    void erase(const ValueType& elem) {
        if (find(elem) == end()) {
            return;
        }
        --size_;
        root_ = EraseElem(root_, elem);
    }

    iterator begin() const {
        if (empty()) {
            return iterator(GetMinElem(root_), true);
        }
        return iterator(GetMinElem(root_), false);
    }

    iterator end() const {
        return iterator(GetMaxElem(root_), true);
    }

    iterator find(const ValueType& elem) const {
        Node* help_node = FindElem(root_, elem);
        if (help_node == nullptr) {
            return iterator(GetMaxElem(root_), true);
        }
        else {
            return iterator(help_node, false);
        }
    }

    iterator lower_bound(const ValueType& elem) const {
        Node* help_node = FindLowerBoundElem(root_, elem);
        if (help_node == nullptr) {
            return iterator(GetMaxElem(root_), true);
        }
        else {
            return iterator(help_node, false);
        }
    }

    void print() {
        PrintTree(root_);
    }

private:
    int GetHeight(Node* root) const {
        if (root == nullptr) {
            return 0;
        }
        return root->height;
    }

    void UpdateHeight(Node* root) {
        if (root == nullptr) {
            return;
        }
        root->height = std::max(GetHeight(root->left), GetHeight(root->right)) + 1;
    }

    int GetBalanceFactor(Node* root) const {
        if (root == nullptr) {
            return 0;
        }
        return GetHeight(root->left) - GetHeight(root->right);
    }

    Node* RightRotation(Node* root) {
        if (root == nullptr) {
            return root;
        }

        Node* new_root = root->right;
        root->right = new_root->left;
        if (root->right != nullptr) {
            root->right->prev = root;
        }
        new_root->left = root;

        UpdateHeight(root);
        UpdateHeight(new_root);

        new_root->prev = root->prev;
        root->prev = new_root;
        return new_root;
    }

    Node* LeftRotation(Node* root) {
        if (root == nullptr) {
            return root;
        }

        Node* new_root = root->left;
        root->left = new_root->right;
        if (root->left != nullptr) {
            root->left->prev = root;
        }
        new_root->right = root;

        UpdateHeight(root);
        UpdateHeight(new_root);

        new_root->prev = root->prev;
        root->prev = new_root;
        return new_root;
    }

    Node* BigLeftRotation(Node* root) {
        if (root == nullptr) {
            return root;
        }

        Node* help_root = root->left; // мб утечка
        root->left = RightRotation(help_root);
        Node* new_root = LeftRotation(root);

        UpdateHeight(new_root);
        return new_root;
    }

    Node* BigRightRotation(Node* root) {
        if (root == nullptr) {
            return root;
        }

        Node* help_root = root->right; // мб утечка
        root->right = LeftRotation(help_root);
        Node* new_root = RightRotation(root);

        UpdateHeight(new_root);
        return new_root;
    }

    Node* Balance(Node* root) {
        UpdateHeight(root);
        if (GetBalanceFactor(root) > 1 && GetBalanceFactor(root->left) > 0) {
            root = LeftRotation(root);
        }
        else if (GetBalanceFactor(root) > 1 && GetBalanceFactor(root->left) < 0) {
            root = BigLeftRotation(root);
        }
        else if (GetBalanceFactor(root) < -1 && GetBalanceFactor(root->right) < 0) {
            root = RightRotation(root);
        }
        else if (GetBalanceFactor(root) < -1 && GetBalanceFactor(root->right) > 0) {
            root = BigRightRotation(root);
        }
        UpdateHeight(root);
        return root;
    }

    Node* GetMaxElem(Node* root) const {
        if (root == nullptr) {
            return nullptr;
        }
        Node* current = root;
        while (current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    Node* GetMinElem(Node* root) const {
        if (root == nullptr) {
            return nullptr;
        }
        Node* current = root;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    Node* InsertElem(Node* root, Node* prev, const ValueType& elem) {
        if (root == nullptr) {
            root = new Node(elem);
            root->prev = prev;
            ++size_;
        }
        else if (elem < root->value) {
            root->left = InsertElem(root->left, root, elem);
            root = Balance(root);
        }
        else if (root->value < elem) {
            root->right = InsertElem(root->right, root, elem);
            root = Balance(root);
        }
        return root;
    }

    Node* FindElem(Node* root, const ValueType& elem) const {
        if (root == nullptr) {
            return nullptr;
        }
        if (elem < root->value) {
            return FindElem(root->left, elem);
        }
        else if (root->value < elem) {
            return FindElem(root->right, elem);
        }
        else {
            return root;
        }
    }

    Node* FindLowerBoundElem(Node* root, const ValueType& elem) const {
        if (root == nullptr) {
            return nullptr;
        }
        if (root->value < elem) {
            return FindLowerBoundElem(root->right, elem);
        }
        else if (elem < root->value) {
            Node* res = FindLowerBoundElem(root->left, elem);
            if (res == nullptr) {
                return root;
            }
            else {
                return res;
            }
        }
        else {
            return root;
        }
    }

    Node* EraseElem(Node* root, const ValueType& elem) {
        if (root == nullptr) {
            return nullptr;
        }

        if (elem < root->value) {
            root->left = EraseElem(root->left, elem);
            root = Balance(root);
        }
        else if (root->value < elem) {
            root->right = EraseElem(root->right, elem);
            root = Balance(root);
        }
        else {
            if (root->left != nullptr && root->right != nullptr) {
                if (GetBalanceFactor(root) > 0) {
                    root->value = GetMaxElem(root->left)->value;
                    root->left = EraseElem(root->left, root->value);
                }
                else {
                    root->value = GetMinElem(root->right)->value;
                    root->right = EraseElem(root->right, root->value);
                }
            }
            else if (root->left == nullptr) {
                Node* help_node = root;
                root = root->right;
                if (root != nullptr) {
                    root->prev = help_node->prev;
                }
                help_node->right = nullptr;
                delete help_node;
            }
            else {
                Node* help_node = root;
                root = root->left;
                root->prev = help_node->prev;
                help_node->left = nullptr;
                delete help_node;
            }
        }
        return root;
    }

    void PrintTree(Node* root) const {
        if (root == nullptr) {
            return;
        }
        PrintTree(root->left);
        std::cout << root->value << " ";
        PrintTree(root->right);
    }

    Node* CopyTree(Node* root, Node* prev) const {
        if (root == nullptr) {
            return nullptr;
        }
        Node* new_root = new Node(root->value);
        new_root->prev = prev;
        new_root->left = CopyTree(root->left, new_root);
        new_root->right = CopyTree(root->right, new_root);
        return new_root;
    }

    Node* root_;
    size_t size_;

};
