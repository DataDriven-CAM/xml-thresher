#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <iterator>
#include <type_traits>
#include <concepts>
#include <ranges>
#include <string_view>
#include <memory>

template<class Facet>
struct word_facet : Facet
{
    template<class... Args>
    word_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~word_facet() {}
};


namespace sylvanmats::io::xml{
            template <typename T>
            struct iterator_traits {
                using __secret_am_i_the_primary_alias = iterator_traits;
            };
            
            template <typename T>concept is_iterator_primary = std::same_as<typename iterator_traits<T>::__secret_am_i_the_primary_alias, iterator_traits<T>>;

    template<typename T> requires std::same_as<T, std::u16string> &&  std::copy_constructible<T>
    class Path {
    friend class Binder;
    protected:
        std::allocator<T> allocations;
        T::size_type n=0;
        T::size_type reserveSize=0;
        T* buffer=nullptr;
        
        template<typename I> requires std::same_as<I, std::u16string>// && std::input_or_output_iterator<I>
        struct iterator {
            template<typename D>
            using iter_difference_t = typename std::conditional_t<is_iterator_primary<D>, std::incrementable_traits<D>, iterator_traits<D>>::difference_type;
            I* buffer=nullptr;
            iterator() = default;
            iterator(I* buffer){this->buffer=buffer;};
            iterator(const iterator<I>& orig) = default;
            iterator(iterator<I>&& other) = default;
            virtual ~iterator() = default;
            iterator& operator=(const iterator& other) noexcept = default;
            iterator& operator=(iterator&& other) noexcept = default;
            
            bool operator==(const iterator<T>& other){ return this->buffer==other.buffer;};
            bool operator!=(const iterator<T>& other){ return this->buffer!=other.buffer;};
            iterator<I>& operator++(){buffer++; return *this;};
            iterator<I>& operator++(int){iterator<I>& old=*this;operator++(); return old;};
            iterator<I>& operator--(){buffer--; return *this;};
            iterator<I>& operator--(int){iterator<I>& old=*this;operator--(); return old;};
            I& operator*() {return (I&)(*buffer);};
            I* operator->() { return buffer; };
            
        };
        friend bool operator==(const iterator<T>& orig, const iterator<T>& other){ return orig.buffer==other.buffer;};
        friend bool operator!=(const iterator<T>& orig, const iterator<T>& other){ return orig.buffer!=other.buffer;};
        
    public:
        
//        iterator<T> iterator;
        
    public:
        Path() = default;
        Path(const char16_t* c){
            std::u16string pathRespresentation(c);
            constexpr std::u16string_view delim{u"/"};
            std::ranges::split_view splitting(pathRespresentation, delim);
            reserveSize=std::distance(splitting.cbegin(), splitting.cend());
            reserve(reserveSize);
            for (const auto wordRange : splitting){
                T word(wordRange.begin(), wordRange.end());
                if(!word.empty())push_back(word);
            }

        };
        Path(const Path& orig) = default;
        Path(Path&& other) = default;
        virtual ~Path(){
            //std::for_each_n(allocations, n, [](const auto& e) { e.id(); });
            std::destroy_n(buffer, n);
            allocations.deallocate(buffer, n);
        };
        Path& operator=(const Path& other) noexcept = default;
        Path& operator=(Path&& other) noexcept = default;
        
        void reserve(T::size_type reserveSize){
            buffer=allocations.allocate(reserveSize);
            this->reserveSize=reserveSize;
        };
        
        void push_back(const T& value){
            if(n>=reserveSize){
                
            }
            std::construct_at(buffer+n, value);
            n++;
            
        };
        
        iterator<T> begin(){ return iterator<T>(buffer);};

        iterator<T> end(){ return iterator<T>(buffer+n);};
        
        const iterator<T> cbegin(){ return const_cast<const T&>(iterator<T>(buffer)).begin();};

        const iterator<T> cend(){ return const_cast<const T&>(iterator<T>(buffer+n+1).end());};
        
        T::size_type size(){return n;};
        
        T::size_type max_size(){return std::allocator_traits<std::allocator<T>>::max_size;};
        
        bool empty(){return n==0;};
    };
    
}


    sylvanmats::io::xml::Path<std::u16string> operator"" _xp(const char16_t* c, size_t s){
        return sylvanmats::io::xml::Path<std::u16string>(c);
    }
