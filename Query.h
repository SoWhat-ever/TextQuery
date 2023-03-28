#ifndef __QUERY_H__
#define __QUERY_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

namespace Query{

using line_no = std::vector<std::string>::size_type;

// 保存一个query结果
class QueryResult {
public:
    QueryResult(std::string p_sought, 
                std::shared_ptr<std::set<line_no>> p_lines,
                std::shared_ptr<std::vector<std::string>> p_file) :
                sought(p_sought), lines(p_lines), file(p_file) { }
    void print(std::ostream&);
private:
    // 查找单词
    std::string sought;
    // 出现的行号
    std::shared_ptr<std::set<line_no>> lines;
    // 输入文件
    std::shared_ptr<std::vector<std::string>> file;
};



class TextQuery {
public:
    // 读入给定文件，构建查找图
    TextQuery(std::ifstream &s);
    // query操作：接收string，返回QueryResult对象
    QueryResult query(const std::string&) const;
private:
    // 输入文件
    std::shared_ptr<std::vector<std::string>> file;
    // 每个单词到所在行号集合的映射
    std::map<std::string, std::shared_ptr<std::set<line_no>>> word_map;

};

// class Query_base {
//     friend class Query;
// protected:
//     virtual ~Query_base() = default;
// private:
//     virtual QueryResult eval(const TextQuery& ) const = 0;
//     virtual std::string rep() const = 0;
// };

// class WordQuery : public Query_base {
//     // QueryResult eval(const TextQuery& t) const { return t.q}
//     std::string rep() const { return query_word; }
//     std::string query_word;
// };

// ccccc

}
#endif