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
    Book* FindBookByIsbn(const std::string& isbn);
    void RemoveBook(const std::string& isbn);

private:
    std::vector<std::unique_ptr<Book>> books;
};

// Without this piece of code, we will obtain a reference error since the void AddBook line isn't actually declared anywhere.
// Here we also tell the program to move ownership of the created unique_ptr, thus avoid having to do that in int main.
// 
void Library::AddBook(std::unique_ptr<Book> book) {
    books.push_back(std::move(book));
}

int main(){
    Library library;

    // Creating the books using a unique pointer.
    // We then also transfer the unique_pointer ownership when we make them this way.
    // This way, we avoid having to add a move function later.
    library.AddBook(std::make_unique<Book>("The Shadow of what was lost", "James Islington", 400, 2018));
    library.AddBook(std::make_unique<Book>("Dune", "Frank Herbert", 500, 1966));

    // Make a loan/return book system using std::cin and std::cout.
    // The user types the name of the book they want to loan or return. Then remove that book from the library and set a bool as a flag that this book has been loaned or returned (remove flag).

    std::string input;
    while (true){
        std::cout << "Return or loan a book? Type 'loan' or 'return': " << std::endl;
        std::cin >> input;
        if (input == 'loan'){
            std::cout << "Type the title of the book you want to loan: " << std::endl;
            std::cin >> input;
            Book* book = library.FindBookByIsbn(input);
            if (book != nullptr && book->available){
                book->available = false;
                std::cout << "You have loaned the book: " << book->title << std::endl;
            }
            else {
                std::cout << "Book not found or unavailable." << std::endl;
            }
        }
        else if (input == 'return'){
            std::cout << "Type the title of the book you want to return: " << std::endl;
            std::cin >> input;
            Book* book = library.FindBookByIsbn(input);
            if (book != nullptr && !book->available){
                book->available = true;
                std::cout << "You have returned the book: " << book->title << std::endl;
            }
            else {
                std::cout << "Book not found" << std::endl;
            }
        }
    }

}