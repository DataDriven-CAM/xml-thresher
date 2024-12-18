# xml-thresher
binding from xml using c++ utf8/u16 and schema awareness

## To build and test

Note: currently working on a new package manager to use on this package.  Other managers should still 
work(as far as they do) as https://github.com/DataDriven-CAM/cnpm.git is based on the package.json npm format.

### cnpm building

```
cnpm  install

cnpm lib

#building and running unit tests
cnpm test

```

XPath 3.1:
A basic XPath of literal steps is now working:
```
    sylvanmats::io::xml::Binder xmlReaper("./examples/note.xml", "");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){});
    sylvanmats::io::xml::Path<std::u16string> path(u"/note/body");
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    xmlReaper(path, [&cv](std::u16string_view& value){
        std::cout<<" "<<cv.to_bytes(std::u16string(value.begin(), value.end()))<<std::endl;
    });

```
returns the body text from ./examples/note.xml

## Contact

My twitch stream is https://www.twitch.tv/sylvanmats
(with some hardware getting active again)