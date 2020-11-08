#include <vector>
#include <iostream>
#include <string>
#include <fstream>

template<typename T, int ORDER>
class btree{
private:

    enum state_t {OVFLOW, UNFLOW, B_OK};

    struct node {
        std::vector<T> data;
        std::vector<node*> children;
        size_t count{0};
        
        node() 
            : data(ORDER+1), children(ORDER+2, nullptr)
        {

        }

        void insert_into(size_t index, const T& value) {
            size_t j = this->count;
            while (j > index) {
                children[j+1] = children[j];
                data[j] = data[j-1];
                j--;
            }
            children[j+1] = children[j];
            data[j] = value;
            this->count++;
        }

        void push_back(const T& value) {
            insert_into(this->count, value);
        }

        state_t insert(const T& value) {
            // binary_search
            size_t index = 0;
            while (this->data[index] < value  && index < this->count) {
                index += 1; 
            }
            if (this->children[index] == nullptr) {
                // this is a leaf node
                this->insert_into(index, value);
            } else {
                auto state = this->children[index]->insert(value);
                if (state == state_t::OVFLOW) {
                    // split 
                    this->split(index);
                }
            }
            return this->count > ORDER ? OVFLOW : B_OK;
        }

        void split(size_t position) {
            // leaf nodes / index nodes
            node* parent = this; 
            node* ptr = this->children[position];

            // TODO: reuse ptr buffer 
            node* child1 = new node();
            node* child2 = new node();
            size_t i = 0;
            for (; i < ptr->count / 2; i++) {
                child1->children[i] = ptr->children[i];
                child1->data[i] = ptr->data[i];
                child1->count++;
            }
            child1->children[i] = ptr->children[i];
            auto mid = i;
            if (ptr->children[0] != NULL) i++;
            size_t j = 0;
            for (; i < ptr->count; i++) {
                child2->children[j] = ptr->children[i];
                child2->data[j] = ptr->data[i];
                child2->count++;
                j++;
            }
            child2->children[j] = ptr->children[i];
            
            parent->insert_into(position, ptr->data[mid]); 
            parent->children[position] = child1;
            parent->children[position + 1] = child2;
        }
    };

public:
    btree() {

    }
    
    void insert(const T& value) {
        auto state = root.insert(value);
        if (state == state_t::OVFLOW) {
            // split root node
            split_root();
        }
    }

  void print() {
    print(&root, 0);
    // std::cout << "________________________\n";
  }

  void print(node *ptr, int level) {
    if (ptr) {
      int i;
      for (i = ptr->count - 1; i >= 0; i--) {
        print(ptr->children[i + 1], level + 1);

        for (int k = 0; k < level; k++) {
          std::cout << "    ";
        }
        std::cout << ptr->data[i] << "\n";
      }
      print(ptr->children[i + 1], level + 1);
    }
  }

private: 
    // 
    void split_root() {
        node* ptr = &root;
        node* child1 = new node();
        node* child2 = new node();
        size_t i = 0;
        for (; i < ptr->count / 2; i++) {
            child1->children[i] = ptr->children[i];
            child1->data[i] = ptr->data[i];
            child1->count++;
        }
        child1->children[i] = ptr->children[i];
        auto mid = i;
        // COMMENT TO MAKE BPLUS
        i += 1;
        if (ptr->children[0] != NULL) i++;
        size_t j = 0;
        for (; i < ptr->count; i++) {
            child2->children[j] = ptr->children[i];
            child2->data[j] = ptr->data[i];
            child2->count++;
            j++;
        }
        child2->children[j] = ptr->children[i];

        ptr->data[0] = ptr->data[mid];
        ptr->children[0] = child1;
        ptr->children[1] = child2;
        ptr->count = 1;
    }

public:
    node root;
};

struct Record
{
    char key[30];
    long pdir;
    long offset;  
};

class Page
{
private:
    Record  *m_ED;
    int m_size;
    int m_current;
    unsigned long m_pNext;
    char m_DataFile[100];
       
public:
    Page(char *  filename)
    {
        // Abre el archivo data.dat 
        strcpy(m_DataFile,filename);
    } 
    Record* getList() {
        return m_ED;
    }
    void Execute()
    {
        ifstream file(m_DataFile, ios::binary);
        Record r;
        char temp[200];

        while(file.peek() != EOF) {
        r.pdir = file.tellg();
        r.offset = sizeof(Record);
        file.read((char*)temp, 10);
        strcpy(r.key,temp);
        file.read((char*)temp, 200);
        m_ED.push_back(r);
        } 
            // Se encarga de indexar el archivo
    }
    void Write() {
        ofstream file("index.dat", ios::binary);
        for(auto e: m_ED) {
            file.write((char*)&e, sizeof(Record));
        }
    }  // Guarda en Disco el archico index.dat 

    void Read() {
        ifstream file("index.dat", ios::binary);
        while(file.peek() != EOF) {
            Record r;
            file.read((char*)&r, sizeof(Record));
            m_ED.push_back(r);
        }
    }

    void Find(char * key) {
        for(auto e: m_ED) {
            if(strcmp(e.key,key) == 0 ) {
            ifstream file(m_DataFile, ios::binary);
            file.seekg(e.pdir);
            Record P;
            file.read((char*)&P, sizeof(Record));
            P.Print();
            return;
            }
        }
    }
};