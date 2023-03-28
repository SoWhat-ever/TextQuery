#include "Query.h"
#include <sstream>

namespace Query{

void QueryResult::print(std::ostream& os){
    os << sought << " occurs " << lines->size() << " time " << std::endl;
    for(auto num : *lines)
        os << "\t(line " << num + 1 << ") " << *(file->begin() + num) << std::endl;
}

TextQuery::TextQuery(std::ifstream &is) : file(new std::vector<std::string>) {
    std::string text;
    // 逐行
    while(getline(is, text)) {
        file->push_back(text);
        int n = file->size() - 1;
        // 逐词
        std::istringstream line(text);
        std::string word;
        while(line >> word) {
            // lines是一个shared_ptr<std::set<line_no>>的引用，另外会添加还不存在的word
            auto &lines = word_map[word];
            if(!lines) {
                // 分配新的set
                lines.reset(new std::set<line_no>);
            }
            lines->insert(n);
        }
    }
    
}

QueryResult TextQuery::query(const std::string& sought) const{
    // 定义空指针用于返回未查找到；因为每次都会用到，设为静态
    static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
    // 用fing()而非下表，避免添加单词
    auto loc = word_map.find(sought);
    if(loc == word_map.end())
        return QueryResult(sought, nodata, file);
    else
        return QueryResult(sought, loc->second, file);
}

}