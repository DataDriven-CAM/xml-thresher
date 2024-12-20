#pragma once
#include <cstdio>
#include <string>
#include <locale>
#include <codecvt>
#include <iterator>
#include <type_traits>
#include <concepts>
#include <ranges>
#include <string_view>
#include <memory>
#include <vector>
#include <tuple>
#include <map>
#include <iomanip>
#include <functional>

#include "graph/container/compressed_graph.hpp"

#include "antlr4-runtime.h"
#include "io/xpath/XPath31ParserBase.h"
#include "parsing/XPath31Lexer.h"
#include "parsing/XPath31Parser.h"

template<class Facet>
struct word_facet : Facet
{
    template<class... Args>
    word_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~word_facet() {}
};


namespace sylvanmats::io::xml{
    class Binder;

    enum ACTION{
        COMPARE,
        TEST
    };

    inline const std::u16string_view substr_view(const std::u16string& source, size_t offset = 0,
                std::u16string_view::size_type count = 
                std::numeric_limits<std::u16string_view::size_type>::max()) {
    if (offset < source.size() && count>offset) 
        return std::u16string_view(source.data() + offset, count - offset);
    return {};
    }


    struct tag_indexer{
        size_t index=0;
        size_t angle_start=0;
        size_t forward_slashA=0;
        size_t space=0;
        size_t forward_slashB=0;
        size_t angle_end=0;
        size_t depth=0;
        std::vector<std::pair<std::u16string_view, std::u16string_view>> attributes;
    };

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
            
            bool operator==(const iterator<I>& other){ return this->buffer==other.buffer;};
            bool operator!=(const iterator<I>& other){ return this->buffer!=other.buffer;};
            iterator<I>& operator++(){buffer++; return *this;};
            iterator<I>& operator++(int){iterator<I>& old=*this;operator++(); return old;};
            iterator<I>& operator--(){buffer--; return *this;};
            iterator<I>& operator--(int){iterator<I>& old=*this;operator--(); return old;};
            I& operator*() {return (I&)(*buffer);};
            I* operator->() { return buffer; };
            
        };
        friend bool operator==(const iterator<T>& orig, const iterator<T>& other){ return orig.buffer==other.buffer;};
        friend bool operator!=(const iterator<T>& orig, const iterator<T>& other){ return orig.buffer!=other.buffer;};
        
        std::wstring_convert<word_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
        std::string content{};
        std::shared_ptr<antlr4::ANTLRInputStream> input;
        std::shared_ptr<sylvanmats::XPath31Lexer> lexer;
        std::shared_ptr<antlr4::CommonTokenStream> tokens;
        std::shared_ptr<sylvanmats::XPath31Parser> parser;
        sylvanmats::XPath31Parser::XpathContext* tree;
        sylvanmats::XPath31Parser::ExprContext* expr;
        std::vector<ACTION> actions;
        std::u16string currentAttribute{};
        std::u16string currentValue{};
    public:
        
//        iterator<T> iterator;
        
    public:
        Path() = default;
        //Path(const char16_t* c) : Path((char16_t*)c) {};
        Path(const char16_t* c){
            //std::cout<<"Path construct "<<std::endl;
            T pathRespresentation(c);
            constexpr std::u16string_view delim{u"/"};
            std::ranges::split_view splitting(pathRespresentation, delim);
            //reserveSize=std::distance(splitting.cbegin(), splitting.cend());
            //std::cout<<"word "<<reserveSize<<std::endl;
            for (const auto wordRange : splitting){
                if(!wordRange.empty())reserveSize++;
                //std::cout<<"word "<<cv.to_bytes(std::u16string(wordRange.begin(), wordRange.end()))<<std::endl;
            }
            reserve(reserveSize);
            /*for (const auto wordRange : splitting){
                T word(wordRange.begin(), wordRange.end());
                if(!word.empty()){
                    typename T::size_type sqOffset=word.find(u"[");
                    if(sqOffset!=T::npos){
                        push_back(word.substr(sqOffset));
                        actions.push_back(TEST);
                    }
                    else{
                        push_back(word);
                        actions.push_back(COMPARE);
                    }
                }
            }*/
            content=cv.to_bytes(pathRespresentation);
            input.reset (new antlr4::ANTLRInputStream(content));
            lexer.reset(new sylvanmats::XPath31Lexer(input.get()));
            tokens.reset(new antlr4::CommonTokenStream(lexer.get()));
            parser.reset(new sylvanmats::XPath31Parser(tokens.get()));
            parser->setBuildParseTree(true);
            tree=parser->xpath();    
            
                expr=tree->expr();
                init(expr);
            };
        Path(const Path& orig) = default;
        Path(Path&& other) = default;
        virtual ~Path(){
            //std::for_each_n(allocations, n, [](const auto& e) { e.id(); });
            if(n>0){
                std::destroy_n(buffer, n);
                allocations.deallocate(buffer, n);
                n=0;
            }
        };
        
        Path& operator=(const Path& other) noexcept = default;
        Path& operator=(Path&& other) noexcept = default;

            void init(sylvanmats::XPath31Parser::ExprContext* expr){
            std::vector<sylvanmats::XPath31Parser::ExprsingleContext *> es=expr->exprsingle();
            for(std::vector<sylvanmats::XPath31Parser::ExprsingleContext *>::iterator it=es.begin();it!=es.end();it++){
                //std::cout<<"\tes "<<(*it)->getText()<<" "<<((*it)->forexpr())<<" "<<((*it)->letexpr())<<" "<<((*it)->quantifiedexpr())<<" "<<((*it)->ifexpr())<<" "<<((*it)->orexpr())<<std::endl;
                if((*it)->quantifiedexpr()!=nullptr){
                    //std::cout<<"\nvarname size "<<(*it)->quantifiedexpr()->varname().size()<<std::endl;
                }
                if((*it)->orexpr()!=nullptr){
                    std::vector<sylvanmats::XPath31Parser::AndexprContext *> ac=(*it)->orexpr()->andexpr();
                    for(std::vector<sylvanmats::XPath31Parser::AndexprContext *>::iterator andIt=ac.begin();andIt!=ac.end();andIt++){
                    std::vector<sylvanmats::XPath31Parser::ComparisonexprContext *> cc=(*andIt)->comparisonexpr();
                    if(!cc.empty()){
                        std::vector<sylvanmats::XPath31Parser::StringconcatexprContext *> concat=cc[0]->stringconcatexpr();
                        //std::cout<<"concat "<<concat.size()<<" "<<concat[0]->rangeexpr().size()<<" "<<concat[0]->rangeexpr()[0]->additiveexpr().size()<<std::endl;
                        for(std::vector<sylvanmats::XPath31Parser::StringconcatexprContext *>::iterator concatIt=concat.begin();concatIt!=concat.end();concatIt++){
                        if(!(*concatIt)->rangeexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr()[0]->instanceofexpr().empty()){
                        sylvanmats::XPath31Parser::TreatexprContext* tc=(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr()[0]->instanceofexpr()[0]->treatexpr();
                        //std::cout<<"tc "<<tc<<std::endl;
                        //std::cout<<"tc "<<tc->castableexpr()<<std::endl;
                        sylvanmats::XPath31Parser::ArrowexprContext* aec=tc->castableexpr()->castexpr()->arrowexpr();
                        //std::cout<<"aec pathexpr "<<aec->unaryexpr()->valueexpr()->simplemapexpr()->pathexpr().size()<<std::endl;
                        if(aec->unaryexpr()!=nullptr && aec->unaryexpr()->valueexpr()!=nullptr && aec->unaryexpr()->valueexpr()->simplemapexpr()!=nullptr && !aec->unaryexpr()->valueexpr()->simplemapexpr()->pathexpr().empty() && aec->unaryexpr()->valueexpr()->simplemapexpr()->pathexpr()[0]->relativepathexpr()!=nullptr){
                        std::vector<sylvanmats::XPath31Parser::StepexprContext *> sec=aec->unaryexpr()->valueexpr()->simplemapexpr()->pathexpr()[0]->relativepathexpr()->stepexpr();
                        for(std::vector<sylvanmats::XPath31Parser::StepexprContext *>::iterator stepIt=sec.begin();stepIt!=sec.end();stepIt++){
                            if((*stepIt)->axisstep()!=nullptr){
                            //std::cout<<"stepexpr size "<<" "<<(*stepIt)->axisstep()->reversestep()<<" "<<(*stepIt)->axisstep()->forwardstep()<<" "<<(*stepIt)->axisstep()->predicatelist()->predicate().size()<<std::endl;
                                if((*stepIt)->axisstep()->forwardstep()!=nullptr){
                                    //std::cout<<"forwardstep size "<<(*stepIt)->axisstep()->forwardstep()->forwardaxis()<<" nt "<<(*stepIt)->axisstep()->forwardstep()->nodetest()<<" "<<(*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()<<std::endl;
                                    if((*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()!=nullptr){
                                        if((*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()->nodetest()->nametest()!=nullptr){
                                            //if((*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()->nodetest()->nametest()->eqname()!=nullptr)
                                            //std::cout<<"\t@? init "<<((*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()->AT())<<" eqname "<<(*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()->nodetest()->nametest()->eqname()->getText()<<" "<<(*stepIt)->axisstep()->forwardstep()->abbrevforwardstep()->nodetest()->nametest()->wildcard()<<" "<<reserveSize<<std::endl;
                                        }
                                    }
                                }
                                if((*stepIt)->axisstep()->predicatelist()!=nullptr){
                                if((*stepIt)->axisstep()->predicatelist()->predicate().empty() && reserveSize>0 && n<reserveSize){
                                    T word=cv.from_bytes((*stepIt)->axisstep()->getText());
                                    push_back(word);
                                    actions.push_back(COMPARE);
                                }
                                else if(!(*stepIt)->axisstep()->predicatelist()->predicate().empty() && (*stepIt)->axisstep()->predicatelist()->predicate()[0]->expr()!=nullptr){
                                    init((*stepIt)->axisstep()->predicatelist()->predicate()[0]->expr());
                                }
                                }
                            }
                            if((*stepIt)->postfixexpr()!=nullptr){
                            //if((*stepIt)->axisstep()!=nullptr)std::cout<<"axisstep "<<(*stepIt)->axisstep()->getText()<<std::endl;
                            //std::cout<<"postfixexpr "<<" "<<(*stepIt)->postfixexpr()->primaryexpr()<<" "<<(*stepIt)->postfixexpr()->predicate().size()<<" "<<(*stepIt)->postfixexpr()->argumentlist().size()<<" "<<(*stepIt)->postfixexpr()->lookup().size()<<std::endl;
                                if((*stepIt)->postfixexpr()->primaryexpr()!=nullptr){
                                    if((*stepIt)->postfixexpr()->primaryexpr()->literal()!=nullptr){
                                        //std::cout<<"\tprim lit "<<(*stepIt)->postfixexpr()->primaryexpr()->literal()->getText()<<std::endl;
                                    }
                                }
                            }
                        }
                        }
                        }
                        }
                    }
                    }
                }
            }

            /*if (sylvanmats::XMPath31Parser::XpathContext* r=dynamic_cast<sylvanmats::XPath31Parser::XpathContext*>((*tree)) {
           std::cout<<" is ExprContext "<<std::endl;
            }*/

        };

        std::tuple<bool, sylvanmats::XPath31Parser::ExprContext*> expressPath(sylvanmats::io::xml::Binder& binding, size_t index, std::u16string_view& text, sylvanmats::XPath31Parser::ExprContext* expr, size_t depth=0){
            std::vector<sylvanmats::XPath31Parser::ExprsingleContext *> es=expr->exprsingle();
            bool ret=false;
            sylvanmats::XPath31Parser::ExprContext* currentExpr=nullptr;
            for(std::vector<sylvanmats::XPath31Parser::ExprsingleContext *>::iterator it=es.begin();it!=es.end();it++){
                //std::cout<<"\tes "<<(*it)->getText()<<" "<<((*it)->forexpr())<<" "<<((*it)->letexpr())<<" "<<((*it)->quantifiedexpr())<<" "<<((*it)->ifexpr())<<" "<<((*it)->orexpr())<<std::endl;
                if(antlrcpp::is<sylvanmats::XPath31Parser::QuantifiedexprContext*>((*it)->quantifiedexpr())){
                    sylvanmats::XPath31Parser::QuantifiedexprContext* qc=(*it)->quantifiedexpr();
                    //std::cout<<"\nvarname size "<<qc->varname().size()<<std::endl;
                }
                if(antlrcpp::is<sylvanmats::XPath31Parser::OrexprContext*>((*it)->orexpr())){
                    sylvanmats::XPath31Parser::OrexprContext* oc=(*it)->orexpr();
                    std::vector<sylvanmats::XPath31Parser::AndexprContext *> ac=oc->andexpr();
                    for(std::vector<sylvanmats::XPath31Parser::AndexprContext *>::iterator andIt=ac.begin();andIt!=ac.end();andIt++){
                    std::vector<sylvanmats::XPath31Parser::ComparisonexprContext *> cc=(*andIt)->comparisonexpr();
                    if(!cc.empty()){
                        std::vector<sylvanmats::XPath31Parser::StringconcatexprContext *> concat=cc[0]->stringconcatexpr();
                        //std::cout<<"concat "<<concat.size()<<" "<<concat[0]->rangeexpr().size()<<" "<<concat[0]->rangeexpr()[0]->additiveexpr().size()<<std::endl;
                        for(std::vector<sylvanmats::XPath31Parser::StringconcatexprContext *>::iterator concatIt=concat.begin();concatIt!=concat.end();concatIt++){
                        if(!(*concatIt)->rangeexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr().empty() && !(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr()[0]->instanceofexpr().empty()){
                        sylvanmats::XPath31Parser::TreatexprContext* tc=(*concatIt)->rangeexpr()[0]->additiveexpr()[0]->multiplicativeexpr()[0]->unionexpr()[0]->intersectexceptexpr()[0]->instanceofexpr()[0]->treatexpr();
                        //std::cout<<"tc "<<tc<<std::endl;
                        //std::cout<<"tc "<<tc->castableexpr()<<std::endl;
                        if(antlrcpp::is<sylvanmats::XPath31Parser::CastableexprContext*>(tc->castableexpr())){
                        sylvanmats::XPath31Parser::CastableexprContext* cec=tc->castableexpr();
                        if(antlrcpp::is<sylvanmats::XPath31Parser::CastexprContext*>(cec->castexpr())){
                        sylvanmats::XPath31Parser::CastexprContext* castec=cec->castexpr();
                        sylvanmats::XPath31Parser::ArrowexprContext* aec=castec->arrowexpr();
                        //std::cout<<"aec pathexpr "<<aec->unaryexpr()->valueexpr()->simplemapexpr()->pathexpr().size()<<std::endl;
                        if(antlrcpp::is<sylvanmats::XPath31Parser::UnaryexprContext*>(aec->unaryexpr())){
                            sylvanmats::XPath31Parser::UnaryexprContext* uc=aec->unaryexpr();
                            if(antlrcpp::is<sylvanmats::XPath31Parser::ValueexprContext*>(uc->valueexpr())){
                                sylvanmats::XPath31Parser::ValueexprContext* vc=uc->valueexpr();
                                if(antlrcpp::is<sylvanmats::XPath31Parser::SimplemapexprContext*>(vc->simplemapexpr())){
                                sylvanmats::XPath31Parser::SimplemapexprContext* smc=vc->simplemapexpr();
                                    std::vector<sylvanmats::XPath31Parser::PathexprContext*> pc=smc->pathexpr();
                                    for(std::vector<sylvanmats::XPath31Parser::PathexprContext *>::iterator pcIt=pc.begin();pcIt!=pc.end();pcIt++){
                                        if(antlrcpp::is<sylvanmats::XPath31Parser::RelativepathexprContext*>((*pcIt)->relativepathexpr())){
                                            sylvanmats::XPath31Parser::RelativepathexprContext* rpc=(*pcIt)->relativepathexpr();
                                            std::vector<sylvanmats::XPath31Parser::StepexprContext *> sec=rpc->stepexpr();
                                            for(std::vector<sylvanmats::XPath31Parser::StepexprContext *>::iterator stepIt=sec.begin();stepIt!=sec.end();stepIt++){
                                                if(antlrcpp::is<sylvanmats::XPath31Parser::AxisstepContext*>((*stepIt)->axisstep())){
                                                sylvanmats::XPath31Parser::AxisstepContext* asc=dynamic_cast<sylvanmats::XPath31Parser::AxisstepContext*>((*stepIt)->axisstep());
                                                //std::cout<<"stepexpr size "<<" "<<asc->reversestep()<<" "<<asc->forwardstep()<<" "<<asc->predicatelist()->predicate().size()<<std::endl;
                                                    if(antlrcpp::is<sylvanmats::XPath31Parser::ForwardstepContext*>(asc->forwardstep())){
                                                        sylvanmats::XPath31Parser::ForwardstepContext* fsc=dynamic_cast<sylvanmats::XPath31Parser::ForwardstepContext*>(asc->forwardstep());
                                                        //T rep(text.begin(), text.end());
                                                        //std::cout<<cv.to_bytes(rep)<<" forwardstep size "<<fsc->forwardaxis()<<" nt "<<fsc->nodetest()<<" "<<fsc->abbrevforwardstep()<<std::endl;
                                                        if(antlrcpp::is<sylvanmats::XPath31Parser::AbbrevforwardstepContext*>(fsc->abbrevforwardstep())){
                                                        sylvanmats::XPath31Parser::AbbrevforwardstepContext* afsc=dynamic_cast<sylvanmats::XPath31Parser::AbbrevforwardstepContext*>(fsc->abbrevforwardstep());
                                                        if(antlrcpp::is<sylvanmats::XPath31Parser::NodetestContext*>(afsc->nodetest())){
                                                            sylvanmats::XPath31Parser::NodetestContext* ntc=dynamic_cast<sylvanmats::XPath31Parser::NodetestContext*>(afsc->nodetest());
                                                            if(antlrcpp::is<sylvanmats::XPath31Parser::NametestContext*>(ntc->nametest())){
                                                              sylvanmats::XPath31Parser::NametestContext* nametc=dynamic_cast<sylvanmats::XPath31Parser::NametestContext*>(ntc->nametest());
                                                                //if(antlrcpp::is<sylvanmats::XPath31Parser::WildcardContext*>(nametc->wildcard()))std::cout<<"wildcard "<<std::endl;
                                                                if(antlrcpp::is<sylvanmats::XPath31Parser::WildcardContext*>(nametc->wildcard()))ret=true;
                                                                if(antlrcpp::is<sylvanmats::XPath31Parser::EqnameContext*>(nametc->eqname())){
                                                                sylvanmats::XPath31Parser::EqnameContext* enc=dynamic_cast<sylvanmats::XPath31Parser::EqnameContext*>(nametc->eqname());
                                                                //antlr4::tree::TerminalNode* kw_eq=dynamic_cast<antlr4::tree::TerminalNode*>(enc->KW_EQ());
                                                                //antlr4::tree::TerminalNode* qName=dynamic_cast<antlr4::tree::TerminalNode*>(enc->QName());
                                                                //if(antlrcpp::is<antlr4::tree::TerminalNode*>(enc->KW_EQ()))std::cout<<"KW_EQ "<<std::endl;
                                                                if(!antlrcpp::is<antlr4::tree::ErrorNode*>(enc->QName()) && antlrcpp::is<antlr4::tree::TerminalNode*>(enc->QName())){
                                                                //std::cout<<"qName "<<enc->QName()->getText()<<std::endl;
                                                                if(antlrcpp::is<antlr4::tree::TerminalNode*>(afsc->AT()))currentAttribute=cv.from_bytes(enc->QName()->getText());
                                                                T word=cv.from_bytes(enc->QName()->getText());
                                                                if(text.compare(word)==0)ret=true;
                                                                //std::cout<<"\t? match "<<" "<<reserveSize<<std::endl;
                                                                }
                                                                }
                                                            }
                                                        }
                                                        }
                                                    }
                                                    if(antlrcpp::is<sylvanmats::XPath31Parser::PredicatelistContext*>(asc->predicatelist())){
                                                        sylvanmats::XPath31Parser::PredicatelistContext* plc=asc->predicatelist();
                                                        if(plc->predicate().empty() && reserveSize>0 && n<reserveSize){
                                                            T word=cv.from_bytes(asc->getText());
                                                            if(text.compare(word)==0)ret=true;
                                                        }
                                                        else if(!plc->predicate().empty() && plc->predicate()[0]->expr()!=nullptr){
                                                            //std::cout<<"predicate "<<plc->predicate()[0]->expr()->getText()<<std::endl;
                                                            currentExpr=plc->predicate()[0]->expr();
                                                            currentAttribute=u"";
                                                            currentValue=u"";
                                                            expressPath(binding, index, text, currentExpr, depth+1);
                                                            if(!currentAttribute.empty()){
                                                            std::u16string value=binding.findAttribute(currentAttribute, binding.vertices[index].angle_start+1,  binding.vertices[index].angle_end);
                                                            if(!value.empty()){
                                                            ret=false;
                                                            //if(currentValue.compare(value)==0)std::cout<<"\t"<<cv.to_bytes(currentAttribute)<<" "<<cv.to_bytes(currentValue)<<" "<<cv.to_bytes(value)<<"|"<<std::endl;
                                                            if(currentValue.compare(value)==0)ret=true;
                                                            }
                                                            }
                                                        }
                                                    }
                                                }
                                                if(antlrcpp::is<sylvanmats::XPath31Parser::PostfixexprContext*>((*stepIt)->postfixexpr())){
                                                    sylvanmats::XPath31Parser::PostfixexprContext* pc=(*stepIt)->postfixexpr();
                                                //if(asc!=nullptr)std::cout<<"axisstep "<<asc->getText()<<std::endl;
                                                //std::cout<<"postfixexpr "<<" "<<(*stepIt)->postfixexpr()->primaryexpr()<<" "<<(*stepIt)->postfixexpr()->predicate().size()<<" "<<(*stepIt)->postfixexpr()->argumentlist().size()<<" "<<(*stepIt)->postfixexpr()->lookup().size()<<std::endl;
                                                    if(pc->primaryexpr()!=nullptr){
                                                        if(pc->primaryexpr()->literal()!=nullptr){
                                                            if(antlrcpp::is<antlr4::tree::TerminalNode*>(pc->primaryexpr()->literal()->StringLiteral())){
                                                                currentValue=cv.from_bytes(pc->primaryexpr()->literal()->StringLiteral()->getText().substr(1, pc->primaryexpr()->literal()->StringLiteral()->getText().size()-2));
                                                            }
                                                            else
                                                            currentValue=cv.from_bytes(pc->primaryexpr()->literal()->getText());
                                                            //std::cout<<"\tprim lit "<<pc->primaryexpr()->literal()->getText()<<std::endl;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        }
                        }
                        }
                        }
                    }
                    }
                }
            }
             return std::make_tuple(ret, currentExpr);       
        };

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
        
        T& operator[](size_t i){
            return *(buffer+i);
        };

        iterator<T> begin(){ return iterator<T>(buffer);};

        iterator<T> end(){ return iterator<T>(buffer+n);};
        
        const iterator<T> cbegin(){ return const_cast<const T&>(iterator<T>(buffer)).begin();};

        const iterator<T> cend(){ return const_cast<const T&>(iterator<T>(buffer+n+1)).end();};
        
        T::size_type size(){return n;};
        
        T::size_type max_size(){return std::allocator_traits<std::allocator<T>>::max_size;};
        
        bool empty(){return n==0;};

        friend std::ostream& operator<<(std::ostream& s, Path& xp) {
            std::wstring_convert<word_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
            for(sylvanmats::io::xml::Path<std::u16string>::iterator it=xp.begin();it!=xp.end();it++){
                s <<"/"<< cv.to_bytes(*it);
            }
          return s;
        }
    };
    
}


    sylvanmats::io::xml::Path<std::u16string> operator"" _xp(const char16_t* c, size_t s);
