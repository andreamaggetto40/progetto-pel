#include "json.hpp"

template <typename T>
class LinkedList {
public: 
    struct Node {
        T data;
        Node* next;

        Node(const T& data) : data(data), next(nullptr) {}
    };

    Node* head;
    Node* tail;

    LinkedList() : head(nullptr), tail(nullptr) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current) {
                push_back(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept : head(other.head), tail(other.tail) {
        other.head = nullptr;
        other.tail = nullptr;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;

            other.head = nullptr;
            other.tail = nullptr;
        }
        return *this;
    }

    void push_back(const T& data) {
        Node* newNode = new Node(data);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void push_front(const T& data) {
        Node* newNode = new Node(data);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    void clear() {
        Node* current = head;
        while (current) {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        head = nullptr;
        tail = nullptr;
    }

    class iterator {
        Node* ptr;

    public:
        iterator(Node* p) : ptr(p) {}

        T& operator*() {
            return ptr->data;
        }

        T* operator->() {
            return &(ptr->data);
        }

        iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
    };

    class const_iterator {
        const Node* ptr;

    public:
        const_iterator(const Node* p) : ptr(p) {}

        const T& operator*() const {
            return ptr->data;
        }

        const T& operator->(){
            return &(ptr->data);
        }

        const_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
    };

    iterator begin() {
        return iterator(head);
    }

    iterator end() {
        return iterator(nullptr);  // "past-the-end" è rappresentato da nullptr
    }

    const_iterator begin() const {
        return const_iterator(head);
    }

    const_iterator end() const {
        return const_iterator(nullptr);  // "past-the-end" è rappresentato da nullptr
    }

    T& back() {
        if (tail) {
            return tail->data;
        }
        throw std::runtime_error("Called back() on an empty list");
    }

    const T& back() const {
        if (tail) {
            return tail->data;
        }
        throw std::runtime_error("Called back() on an empty list");
    }

    Node* get_head() const {
        return head;
    }

    Node* get_tail() const {
        return tail;
    }
};

enum class JsonType {
    Null,
    Number,
    Bool,
    String,
    List,
    Dict
};

struct json::impl {
    JsonType type = JsonType::Null;
    double numberValue = 0.0;
    bool boolValue = false;
    std::string stringValue;
    LinkedList<json> listValue;
    LinkedList<std::pair<std::string, json>> dictValue;

    impl() = default; // Costruttore di default
    
    impl(const impl& other) // Copy constructor
        : type(other.type),
          numberValue(other.numberValue),
          boolValue(other.boolValue),
          stringValue(other.stringValue),
          listValue(other.listValue),
          dictValue(other.dictValue) {}

    impl(impl&& other) // Move constructor
        : type(other.type),
          numberValue(other.numberValue),
          boolValue(other.boolValue),
          stringValue(std::move(other.stringValue)),
          listValue(std::move(other.listValue)),
          dictValue(std::move(other.dictValue)) {
        other.type = JsonType::Null;
    }

    impl& operator=(const impl& other) { // Copy assignment
        if (this != &other) {
            type = other.type;
            numberValue = other.numberValue;
            boolValue = other.boolValue;
            stringValue = other.stringValue;
            listValue = other.listValue;
            dictValue = other.dictValue;
        }
        return *this;
    }

    impl& operator=(impl&& other) noexcept { // Move assignment
        if (this != &other) {
            type = other.type;
            numberValue = other.numberValue;
            boolValue = other.boolValue;
            stringValue = std::move(other.stringValue);
            listValue = std::move(other.listValue);
            dictValue = std::move(other.dictValue);
            other.type = JsonType::Null;
        }
        return *this;
    }

    void clear_data() {
        switch (type) {
            case JsonType::String:
                stringValue.clear();
                break;
            case JsonType::Bool:
                // bool non necessita di operazioni particolari di pulizia
                break;
            case JsonType::Number:
                // double non necessita di operazioni particolari di pulizia
                break;
            case JsonType::List:
                listValue.clear();
                break;
            case JsonType::Dict:
                dictValue.clear();
                break;
            case JsonType::Null:
                // Null non necessita di operazioni particolari di pulizia
                break;
        }
    }
};

json::json() 
    : pimpl(new impl()) {} // Costruttore di default

json::~json() { 
    if(pimpl) delete pimpl;
}

json::json(const json& other) 
    : pimpl(new impl(*other.pimpl)) {} // Copy constructor

json::json(json&& other) 
    : pimpl(other.pimpl) {
    other.pimpl = nullptr; // Move constructor
}

json& json::operator=(const json& other) {
    if (this != &other) {
        delete pimpl;
        pimpl = new impl(*other.pimpl);
    }
    return *this;
}

json& json::operator=(json&& other) {
    if (this != &other) {
        delete pimpl;
        pimpl = other.pimpl;
        other.pimpl = nullptr; // Move assignment
    }
    return *this;
}

bool json::is_list() const {
    return pimpl->type == JsonType::List;
}

bool json::is_dictionary() const {
    return pimpl->type == JsonType::Dict;
}

bool json::is_string() const {
    return pimpl->type == JsonType::String;
}

bool json::is_number() const {
    return pimpl->type == JsonType::Number;
}

bool json::is_bool() const {
    return pimpl->type == JsonType::Bool;
}

bool json::is_null() const {
    return pimpl->type == JsonType::Null;
}

json const& json::operator[](std::string const& key) const {
    if (!is_dictionary()) {
        throw json_exception{"json object is not a dictionary"};
    }

    // Utilizziamo una ricerca lineare, dato che non ci aspettiamo che sia efficiente
    for (auto it = pimpl->dictValue.begin(); it != pimpl->dictValue.end(); ++it) {
        if (it->first == key) {
            return it->second;
        }
    }

    // Se la chiave non esiste, lanciamo un'eccezione
    throw json_exception{"Key not found in json dictionary"};
}

json& json::operator[](std::string const& key) {
    if (!is_dictionary()) {
        throw json_exception{"json object is not a dictionary"};
    }

    // Utilizziamo una ricerca lineare, dato che non ci aspettiamo che sia efficiente
    for (auto it = pimpl->dictValue.begin(); it != pimpl->dictValue.end(); ++it) {
        if (it->first == key) {
            return it->second;
        }
    }

    // Se la chiave non esiste, inseriamo un nuovo elemento con valore predefinito
    pimpl->dictValue.push_back(std::make_pair(key, json()));

    // Restituisci una reference all'elemento appena inserito
    return pimpl->dictValue.back().second;
}

double& json::get_number() {
    if (is_number()) {
        return pimpl->numberValue;
    } else {
        throw json_exception{"The JSON object is not a number."};
    }
}

double const& json::get_number() const {
    if (is_number()) {
        return pimpl->numberValue;
    } else {
        throw json_exception{"The JSON object is not a number."};
    }
}

bool& json::get_bool() {
    if (is_bool()) {
        return pimpl->boolValue;
    } else {
        throw json_exception{"The JSON object is not a boolean."};
    }
}

bool const& json::get_bool() const {
    if (is_bool()) {
        return pimpl->boolValue;
    } else {
        throw json_exception{"The JSON object is not a boolean."};
    }
}

std::string& json::get_string() {
    if (is_string()) {
        return pimpl->stringValue;
    } else {
        throw json_exception{"The JSON object is not a string."};
    }
}

std::string const& json::get_string() const {
    if (is_string()) {
        return pimpl->stringValue;
    } else {
        throw json_exception{"The JSON object is not a string."};
    }
}

void json::set_string(std::string const& x) {
    pimpl->clear_data();
    pimpl->type = JsonType::String;
    pimpl->stringValue = x;
}

void json::set_bool(bool x) {
    pimpl->clear_data();
    pimpl->type = JsonType::Bool;
    pimpl->boolValue = x;
}

void json::set_number(double x) {
    pimpl->clear_data();
    pimpl->type = JsonType::Number;
    pimpl->numberValue = x;
}

void json::set_null() {
    pimpl->clear_data();
    pimpl->type = JsonType::Null;
}

void json::set_list() {
    pimpl->clear_data();
    pimpl->type = JsonType::List;
    pimpl->listValue = LinkedList<json>();
}

void json::set_dictionary() {
    pimpl->clear_data();
    pimpl->type = JsonType::Dict;
    pimpl->dictValue = LinkedList<std::pair<std::string, json>>();
}

void json::push_front(json const& x) {
    if (!is_list()) {
        throw json_exception{"Il json non è di tipo lista."};
    }
    pimpl->listValue.push_front(x);
}

void json::push_back(json const& x) {
    if (!is_list()) {
        throw json_exception{"Il json non è di tipo lista."};
    }
    pimpl->listValue.push_back(x);
}

void json::insert(std::pair<std::string, json> const& x) {
    if (!is_dictionary()) {
        throw json_exception{"Il json non è di tipo dizionario."};
    }
    pimpl->dictValue.push_back(x);
}

struct json::list_iterator
{
    friend class json;

private:
    LinkedList<json>::Node *current;

    list_iterator(LinkedList<json>::Node *node) : current(node) {}

public:
    list_iterator() : current(nullptr) {}

    list_iterator &operator++()
    {
        if (current)
        {
            current = current->next;
        }
        return *this;
    }

    list_iterator operator++(int)
    {
        list_iterator temp = *this;
        ++(*this);
        return temp;
    }

    json &operator*() const
    {
        if (current)
        {
            return current->data;
        }
        throw json_exception{"ERRORE: Tentativo di dereferenziare un iteratore vuoto"};
    }

    json *operator->() const
    {
        return &operator*();
    }

    bool operator==(const list_iterator &other) const
    {
        return current == other.current;
    }

    bool operator!=(const list_iterator &other) const
    {
        return !(*this == other);
    }
};

struct json::dictionary_iterator
{
    friend class json;

private:
    LinkedList<std::pair<std::string, json>>::Node *current;

    dictionary_iterator(LinkedList<std::pair<std::string, json>>::Node *node) : current(node) {}

public:
    dictionary_iterator() : current(nullptr) {}

    dictionary_iterator &operator++()
    {
        if (current)
        {
            current = current->next;
        }
        return *this;
    }

    dictionary_iterator operator++(int)
    {
        dictionary_iterator temp = *this;
        ++(*this);
        return temp;
    }

    std::pair<std::string, json> &operator*() const
    {
        if (current)
        {
            return current->data;
        }
        throw json_exception{"ERRORE: Tentativo di dereferenziare un iteratore vuoto"};
    }

    std::pair<std::string, json> *operator->() const
    {
        return &operator*();
    }

    bool operator==(const dictionary_iterator &other) const
    {
        return current == other.current;
    }

    bool operator!=(const dictionary_iterator &other) const
    {
        return !(*this == other);
    }
};

struct json::const_list_iterator
{
    friend class json;

private:
    LinkedList<json>::Node *current;

    const_list_iterator(LinkedList<json>::Node *node) : current(node) {}

public:
    const_list_iterator() : current(nullptr) {}

    const_list_iterator &operator++()
    {
        if (current)
        {
            current = current->next;
        }
        return *this;
    }

    const_list_iterator operator++(int)
    {
        const_list_iterator temp = *this;
        ++(*this);
        return temp;
    }

    const json &operator*() const
    {
        if (current)
        {
            return current->data;
        }
        throw json_exception{"ERRORE: Tentativo di dereferenziare un iteratore vuoto"};
    }

    const json *operator->() const
    {
        return &operator*();
    }

    bool operator==(const const_list_iterator &other) const
    {
        return current == other.current;
    }

    bool operator!=(const const_list_iterator &other) const
    {
        return !(*this == other);
    }
};

struct json::const_dictionary_iterator
{
    friend class json;

private:
    LinkedList<std::pair<std::string, json>>::Node *current;

    const_dictionary_iterator(LinkedList<std::pair<std::string, json>>::Node *node) : current(node) {}

public:
    const_dictionary_iterator() : current(nullptr) {}

    const_dictionary_iterator &operator++()
    {
        if (current)
        {
            current = current->next;
        }
        return *this;
    }

    const_dictionary_iterator operator++(int)
    {
        const_dictionary_iterator temp = *this;
        ++(*this);
        return temp;
    }

    const std::pair<std::string, json> &operator*() const
    {
        if (current)
        {
            return current->data;
        }
        throw json_exception{"ERRORE: Tentativo di dereferenziare un iteratore vuoto"};
    }

    const std::pair<std::string, json> *operator->() const
    {
        return &operator*();
    }

    bool operator==(const const_dictionary_iterator &other) const
    {
        return current == other.current;
    }

    bool operator!=(const const_dictionary_iterator &other) const
    {
        return !(*this == other);
    }
};

json::list_iterator json::begin_list() {
    if (!is_list()) {
        throw json_exception{"ERRORE: L'oggetto json non è una lista"};
    }
    return list_iterator(pimpl->listValue.get_head());
}

json::const_list_iterator json::begin_list() const {
    if (!is_list()) {
        throw json_exception{"ERRORE: L'oggetto json non è una lista"};
    }
    return const_list_iterator(pimpl->listValue.get_head());
}

json::list_iterator json::end_list() {
    if (!is_list()) {
        throw json_exception{"ERRORE: L'oggetto json non è una lista"};
    }
    return list_iterator(nullptr);  // L'iteratore "past-the-end" è rappresentato da nullptr.
}

json::const_list_iterator json::end_list() const {
    if (!is_list()) {
        throw json_exception{"ERRORE: L'oggetto json non è una lista"};
    }
    return const_list_iterator(nullptr);  // L'iteratore "past-the-end" è rappresentato da nullptr.
}

json::dictionary_iterator json::begin_dictionary() {
    if (!is_dictionary()) {
        throw json_exception{"ERRORE: L'oggetto json non è un dizionario"};
    }
    return dictionary_iterator(pimpl->dictValue.get_head());
}

json::const_dictionary_iterator json::begin_dictionary() const {
    if (!is_dictionary()) {
        throw json_exception{"ERRORE: L'oggetto json non è un dizionario"};
    }
    return const_dictionary_iterator(pimpl->dictValue.get_head());
}

json::dictionary_iterator json::end_dictionary() {
    if (!is_dictionary()) {
        throw json_exception{"ERRORE: L'oggetto json non è un dizionario"};
    }
    return dictionary_iterator(nullptr);  // L'iteratore "past-the-end" è rappresentato da nullptr.
}

json::const_dictionary_iterator json::end_dictionary() const {
    if (!is_dictionary()) {
        throw json_exception{"ERRORE: L'oggetto json non è un dizionario"};
    }
    return const_dictionary_iterator(nullptr);  // L'iteratore "past-the-end" è rappresentato da nullptr.
}

std::ostream& operator<<(std::ostream& os, json const& rhs) {
    if (rhs.is_string()) {
        os << "\"" << rhs.get_string() << "\"";
    } else if (rhs.is_bool()) {
        os << (rhs.get_bool() ? "true" : "false");
    } else if (rhs.is_number()) {
        os << rhs.get_number();
    } else if (rhs.is_null()) {
        os << "null";
    } else if (rhs.is_list()) {
        os << "[";
        bool first = true;
        for (auto it = rhs.begin_list(); it != rhs.end_list(); ++it) {
            if (!first) {
                os << ", ";
            }
            os << *it;
            first = false;
        }
        os << "]";
    } else if (rhs.is_dictionary()) {
        os << "{";
        bool first = true;
        for (auto it = rhs.begin_dictionary(); it != rhs.end_dictionary(); ++it) {
            if (!first) {
                os << ", ";
            }
            os << "\"" << it->first << "\": " << it->second;
            first = false;
        }
        os << "}";
    }

    return os;
}

std::istream& operator>>(std::istream& lhs, json& rhs) {
    rhs = json();

    char ch;
    while (lhs.get(ch) && std::isspace(ch)) {
        // Ignora gli spazi bianchi
    }

    if (ch == 'n') { // Parsing di null
        char buffer[4];
        if (lhs.read(buffer, 4) && std::string(buffer, 4) == "null") {
            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi
            }
            rhs.set_null();
        } else {
            throw json_exception{"Errore di parsing: valore null non valido"};
        }
    } else if (ch == 't') { // Parsing di true
        char buffer[4];
        if (lhs.read(buffer, 4) && std::string(buffer, 4) == "true") {
            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi
            }
            rhs.set_bool(true);
        } else {
            throw json_exception{"Errore di parsing: valore booleano true non valido"};
        }
    } else if (ch == 'f') { // Parsing di false
        char buffer[5];
        if (lhs.read(buffer, 5) && std::string(buffer, 5) == "false") {
            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi
            }
            rhs.set_bool(false);
        } else {
            throw json_exception{"Errore di parsing: valore booleano false non valido"};
        }
    } else if (ch == '\"') { // Parsing di string
        std::string str;
        char c;
        while (lhs.get(c) && c != '\"') {
            str += c;
        }
        rhs.set_string(str);
        
        while (lhs.get(ch) && std::isspace(ch)) {
            // Ignora gli spazi bianchi dopo la stringa
        }
    } else if (std::isdigit(ch) || ch == '-') { // Parsing di number
        lhs.putback(ch);
        double num;
        lhs >> num;
        if (lhs.fail()) {
            throw json_exception{"Errore di parsing: valore numerico non valido"};
        }
        rhs.set_number(num);
    } else if (ch == '[') { // Parsing di list
         rhs.set_list();

    while (lhs.get(ch) && std::isspace(ch)) {
        // Ignora gli spazi bianchi
    }

    if (ch != ']') {
        lhs.putback(ch);
        while (true) {
            json item;
            lhs >> item;
            rhs.push_back(item);

            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi tra gli elementi
            }

            if (ch == ',') {
                // Ignora la virgola e gli spazi bianchi successivi
                while (lhs.get(ch) && std::isspace(ch)) {
                    // Ignora gli spazi bianchi dopo la virgola
                }
            } else if (ch == ']') {
                // Fine della lista
                break;
            } else {
                throw json_exception{"Errore di parsing: lista non valida"};
            }
        }
    }
    } else if (ch == '{') { // Parsing di dictionary
        rhs.set_dictionary();

    while (lhs.get(ch) && std::isspace(ch)) {
        // Ignora gli spazi bianchi
    }

    if (ch != '}') {
        lhs.putback(ch);
        while (true) {
            std::string key;
            json value;

            // Parsing della chiave
            lhs >> key;

            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi tra chiave e due punti
            }

            if (ch == ':') {
                // Consuma i due punti
                while (lhs.get(ch) && std::isspace(ch)) {
                    // Ignora gli spazi bianchi dopo i due punti
                }
            } else {
                throw json_exception{"Errore di parsing: dizionario non valido"};
            }

            // Parsing del valore associato alla chiave
            lhs >> value;
            rhs[key] = value;

            while (lhs.get(ch) && std::isspace(ch)) {
                // Ignora gli spazi bianchi tra elementi del dizionario
            }

            if (ch == ',') {
                // Ignora la virgola e gli spazi bianchi successivi
                while (lhs.get(ch) && std::isspace(ch)) {
                    // Ignora gli spazi bianchi dopo la virgola
                }
            } else if (ch == '}') {
                // Fine del dizionario
                break;
            } else {
                throw json_exception{"Errore di parsing: dizionario non valido"};
            }
        }
    }
    } else {
        throw json_exception{"Errore di parsing: carattere non valido"};
    }

    return lhs;
}

