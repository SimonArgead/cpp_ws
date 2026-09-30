#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Book: data-klasse der repræsenterer én bog.
// Konstruktøren initialiserer alle felter, "available" sættes altid til true ved oprettelse.
class Book {
public:
    Book(std::string title, std::string author, int pages, int year)
        : title(title), author(author), pages(pages), year(year), available(true) {}

    std::string title;
    std::string author;
    int pages;
    int year;
    bool available;
};

// Library: ejer alle bøger via unique_ptr i en vector.
// Kun deklarationer her - de faktiske implementeringer mangler stadig (se nedenfor).
class Library {
public:
    void AddBook(std::unique_ptr<Book> book);
    Book* FindBookByTitle(const std::string& title);
    void RemoveBook(const std::string& title);
    void FindBook(const std::string& title);

private:
    std::vector<std::unique_ptr<Book>> books;
};

// Without this piece of code, we will obtain a reference error since the void AddBook line isn't actually declared anywhere.
// Here we also tell the program to move ownership of the created unique_ptr, thus avoid having to do that in int main.
// 
void Library::AddBook(std::unique_ptr<Book> book) {
    books.push_back(std::move(book));
}

// Function for removing books
void Library::RemoveBook(const std::string& title){
    auto it = std::find_if(books.begin(), books.end(),
        [&title](const std::unique_ptr<Book>& b) { return b->title == title; });
        if (it != books.end()){
            books.erase(it);
            std::cout << "Removed book: " << title << std::endl;
        }
}

// Function for finding books by their title
// Lambda funtion. Here we ask it to find the book by its titel by giving it access to the capture list "titel"  and the vector Book. The lambda function will the return "True" if it finds the book we are looking for. False if it doesn't. The "->" is just how we work with pointers. It is a dereference operator that allows us to access the member/data of the object that the pointer is pointing to.

Book* Library::FindBookByTitle(const std::string& title) {
    auto it = std::find_if(books.begin(), books.end(),
        [&title](const std::unique_ptr<Book>& b) { return b->title == title; });
    return (it != books.end()) ? it->get() : nullptr;
}

// Function for finding all books in the Library and print the name of their title.
void Library::FindBook(const std::string& title){
    std::cout << "Available book titles: " << std::endl;
    for (const auto& book : books){
        std::cout << book->title <<std::endl;
    };

}

int main(){
    Library library;

    // Creating the books using a unique pointer.
    // We then also transfer the unique_pointer ownership when we make them this way.
    // This way, we avoid having to add a move function later.

    // Make a test cin for removing, and finding a book by its' titel. Also, print which books are available
    library.AddBook(std::make_unique<Book>("The Shadow of what was lost", "James Islington", 400, 2018));
    library.AddBook(std::make_unique<Book>("Dune", "Frank Herbert", 500, 1966));
    library.FindBook(std::string());
    std::cout << "Enter the title of the vook you want to find: " << std::endl;
    std::string title;
    std::getline(std::cin, title);
    Book* foundBook = library.FindBookByTitle(title);
    std::cout << "Found book: " << (foundBook ? foundBook->title : "Not found") << std::endl;

    return 0;
}