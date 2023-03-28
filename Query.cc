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

QueryResult NotQuery::eval(const TextQuery &tq) const {
    QueryResult result = query.eval(tq);
    auto ret_lines = std::make_shared<std::set<line_no>>();
    auto beg = result.get_lines_begin(), end = result.get_lines_end();
    auto sz = result.get_file()->size();
    for(size_t i = 0; i != sz; i++){
        if(beg == end || *beg != i)
            ret_lines->insert(i);
        else if(beg != end)
            ++beg;
    }
    return QueryResult(rep(), ret_lines, result.get_file());
};


QueryResult AndQuery::eval(const TextQuery &tq) const {
    QueryResult left = lhs.eval(tq), right = rhs.eval(tq);
    auto ret_lines = std::make_shared<std::set<line_no>>();
    // 标注库算法set_intersection，将两个范围的交集写入一个目的迭代器中
    std::set_intersection(left.get_lines_begin(), left.get_lines_end(), 
        right.get_lines_begin(), right.get_lines_end(), std::inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left.get_file());
};

QueryResult OrQuery::eval(const TextQuery &tq) const {
    QueryResult left = lhs.eval(tq), right = rhs.eval(tq);
    // 得到左侧行号
    auto ret_lines = std::make_shared<std::set<line_no>>(left.get_lines_begin(), left.get_lines_end());
    // 插入右侧行号
    ret_lines->insert(right.get_lines_begin(), right.get_lines_end());
    return QueryResult(rep(), ret_lines, left.get_file());
};

}
